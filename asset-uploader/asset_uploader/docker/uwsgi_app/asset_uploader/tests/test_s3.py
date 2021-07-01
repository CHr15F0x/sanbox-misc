# -*- coding: utf-8 -*-

from unittest import TestCase, main as ut_main
from unittest.mock import patch
import logging

import asset_uploader.app.s3


class TestS3(TestCase):

    @classmethod
    def setUpClass(cls):
        logging.disable(logging.CRITICAL)
        # Make sure we begin with clear state
        asset_uploader.app.s3.S3.client_instance = None

    def test_bucket(self):
        self.assertEqual(asset_uploader.app.s3.S3.bucket(), asset_uploader.app.s3._BUCKET)

    def test_client_instance_is_none_at_first(self):
        self.assertIsNone(asset_uploader.app.s3.S3.client_instance)

    @patch('boto3.client')
    def test_client_is_none_when_boto3_client_raises_an_exception(self, mock_class):
        mock_class.side_effect = Exception
        self.assertIsNone(asset_uploader.app.s3.S3.client())

    @patch('boto3.client')
    def test_client_is_none_when_boto3_client_returns_none(self, mock_class):
        mock_class.return_value = None
        self.assertIsNone(asset_uploader.app.s3.S3.client())

    @patch('boto3.client')
    def test_client_is_not_none_when_boto3_client_returns_not_none(self, mock_class):
        mock_class.return_value = 'non-None value'
        self.assertIsNotNone(asset_uploader.app.s3.S3.client())


if __name__ == '__main__':
    ut_main(verbosity=2)
