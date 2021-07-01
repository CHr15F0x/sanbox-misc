#! /usr/bin/env python3
# -*- coding: utf-8 -*-

from unittest import TestCase, main as ut_main
from subprocess import Popen, DEVNULL
from time import sleep
from urllib.parse import urlparse
from os import getenv
from requests import post, put, get

from asset_uploader.app.s3 import S3

import pytest

pytestmark = pytest.mark.skip("You need to set proper AWS credentials in asset_uploader.app.s3 to run end to end tests")

class TestAssetUploaderEnd2End(TestCase):
    SPAWN_PSERVE = getenv('END2END_SPAWN_PSERVE', False)
    SERVER_URL = getenv('END2END_SERVER', 'http://localhost')
    _pserve_proc = None
    _NONEXISTENT_ASSET_ID_URL = SERVER_URL + '/asset/12345678901234567890123456789012'

    @classmethod
    def setUpClass(cls):
        if cls.SPAWN_PSERVE:
            cls._pserve_proc = Popen(['pserve', 'dev.ini'], stdout=DEVNULL, stderr=DEVNULL)
            sleep(2)

    @classmethod
    def tearDownClass(cls):
        if cls.SPAWN_PSERVE:
            cls._pserve_proc.terminate()
            cls._pserve_proc.wait()

    def setUp(self):
        self._asset_id = None

    def tearDown(self):
        if self._asset_id:
            print('--->Removing test asset from S3 bucket...')
            response = S3.client().delete_object(
                Bucket=S3.bucket(),
                Key=self._asset_id
            )
            self.assertEqual(response['ResponseMetadata']['HTTPStatusCode'], 204)

    def test_get_upload_status_of_nonexistent_asset(self):
        response = put(url=TestAssetUploaderEnd2End._NONEXISTENT_ASSET_ID_URL)
        self.assertEqual(response.status_code, 404)

    def test_get_download_url_of_nonexistent_asset(self):
        response = get(url=TestAssetUploaderEnd2End._NONEXISTENT_ASSET_ID_URL)
        self.assertEqual(response.status_code, 404)

    def test_upload_and_download(self):
        # Get asset-ID & pre-signed upload URL
        print("\n--->Getting upload url...")
        response = post(url=TestAssetUploaderEnd2End.SERVER_URL + '/asset')
        self.assertEqual(response.status_code, 201)
        self._asset_id = response.json()['id']
        self.assertRegex(self._asset_id, '[a-f0-9]{32}')
        upload_url = response.json()['upload_url']
        result = urlparse(upload_url)
        self.assertEqual(result.scheme, 'https')
        self.assertTrue(all([result.scheme, result.netloc, result.path]))

        # Upload content using pre-signed URL
        print('--->Uploading...')
        asset_data = 'Test content for asset ID ' + self._asset_id
        response = put(url=upload_url, data=asset_data)
        self.assertEqual(response.status_code, 200)

        # Check upload status
        print('--->Getting upload status...')
        response = put(url=TestAssetUploaderEnd2End.SERVER_URL + '/asset/' + self._asset_id)
        self.assertEqual(response.status_code, 200)
        self.assertDictEqual(response.json(), {'status': 'uploaded'})

        # Get pre-signed download url
        print('--->Getting download url...')
        response = get(url=TestAssetUploaderEnd2End.SERVER_URL + '/asset/' + self._asset_id)
        self.assertEqual(response.status_code, 200)
        download_url = response.json()['download_url']
        result = urlparse(download_url)
        self.assertEqual(result.scheme, 'https')
        self.assertTrue(all([result.scheme, result.netloc, result.path]))

        # Download content
        print('--->Downloading...')
        response = get(url=download_url)
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.text, asset_data)

        # Now check if a timeout will really occur
        print('--->Getting download url with timeout=1...')
        response = get(url=TestAssetUploaderEnd2End.SERVER_URL + '/asset/' + self._asset_id + '?timeout=1')
        self.assertEqual(response.status_code, 200)
        download_url = response.json()['download_url']
        result = urlparse(download_url)
        self.assertEqual(result.scheme, 'https')
        self.assertTrue(all([result.scheme, result.netloc, result.path]))
        # Let the pre-signed URL expire
        sleep(2)
        print('--->Checking if timeout occured...')
        response = get(url=download_url)
        self.assertEqual(response.status_code, 403)
        self.assertIn('<Error><Code>AccessDenied</Code><Message>Request has expired</Message>', response.text)


if __name__ == '__main__':
    ut_main(verbosity=2)
