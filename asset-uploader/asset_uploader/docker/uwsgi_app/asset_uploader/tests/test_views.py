# -*- coding: utf-8 -*-

import logging
from unittest import TestCase, main as ut_main
from unittest.mock import MagicMock, PropertyMock, patch
from uuid import UUID

import asset_uploader.app.views as views
from asset_uploader.app.responses import Responses
from asset_uploader.app.s3 import S3


class TestViews(TestCase):

    @classmethod
    def setUpClass(cls):
        logging.disable(logging.CRITICAL)

    def assertResponseEqual(self, first, second):
        self.assertEqual(first.status_int, second.status_int)
        self.assertEqual(first.status_code, second.status_code)
        self.assertEqual(first.content_type, second.content_type)
        if first.content_type == 'application/json':
            self.assertDictEqual(first.json_body, second.json_body)
        else:
            self.assertEqual(first.text, second.text)

    _MOCK_PRESIGNED_URL = 'https://pre-signed-url.com/blahs?blah=blah'
    _MOCK_ASSET_ID = '0123456789abcdef0123456789abcdef'

    @staticmethod
    def _mock_request():
        mock_request = MagicMock()
        type(mock_request).path = PropertyMock(return_value='server.com/asset/' + TestViews._MOCK_ASSET_ID)
        return mock_request

    def test__asset_id_from_request(self):
        self.assertEqual(views._asset_id_from_request(TestViews._mock_request()), TestViews._MOCK_ASSET_ID)

    @patch('asset_uploader.app.s3.S3.client')
    def test__asset_is_available_returns_true_if_head_object_returns_a_dict(self, mock_class):
        mock_class.return_value = MagicMock()
        mock_class.return_value.head_object.return_value = {'some-key': 'some-value'}
        self.assertTrue(views._asset_is_available(TestViews._mock_request()))
        mock_class.return_value.head_object.assert_called_once_with(Bucket=S3.bucket(), Key=TestViews._MOCK_ASSET_ID)

    @patch('asset_uploader.app.s3.S3.client')
    def test__asset_is_available_returns_false_if_head_object_raises_an_exception(self, mock_class):
        mock_class.return_value = MagicMock()
        mock_class.return_value.head_object.side_effect = Exception
        self.assertFalse(views._asset_is_available(TestViews._mock_request()))

    @patch('asset_uploader.app.s3.S3.client')
    def test_asset_post_returns_forbidden_if_s3_client_is_none(self, mock_class):
        mock_class.return_value = None
        self.assertResponseEqual(views.asset_post('whatever'), Responses.forbidden())

    @patch('asset_uploader.app.s3.S3.client')
    def test_asset_post_returns_not_available_if_generate_presigned_url_fails(self, mock_class):
        mock_class.return_value = MagicMock()
        mock_class.return_value.generate_presigned_url.side_effect = Exception
        self.assertResponseEqual(views.asset_post('whatever'), Responses.not_found())

    @patch('asset_uploader.app.views.uuid4')
    @patch('asset_uploader.app.s3.S3.client')
    def test_asset_post_returns_ok_if_generate_presigned_url_succeeds(self, mock_client_class, mock_uuid4_class):
        mock_client_class.return_value = MagicMock()
        mock_client_class.return_value.generate_presigned_url.return_value = TestViews._MOCK_PRESIGNED_URL
        mock_uuid4_class.return_value = UUID(TestViews._MOCK_ASSET_ID)
        self.assertResponseEqual(views.asset_post('whatever'),
                                 Responses.created(
                                   {'id': TestViews._MOCK_ASSET_ID, 'upload_url': TestViews._MOCK_PRESIGNED_URL}))
        mock_client_class.return_value.generate_presigned_url.assert_called_once_with(
            ClientMethod='put_object',
            Params={
                'Bucket': S3.bucket(),
                'Key': TestViews._MOCK_ASSET_ID,
                'ACL': 'public-read-write'
            },
            HttpMethod='PUT')

    @patch('asset_uploader.app.s3.S3.client')
    def test_asset_put_returns_forbidden_if_s3_client_is_none(self, mock_class):
        mock_class.return_value = None
        self.assertResponseEqual(views.asset_put('whatever'), Responses.forbidden())

    @patch('asset_uploader.app.views._asset_is_available')
    def test_asset_put_returns_not_available_if_asset_not_found(self, mock_class):
        mock_class.return_value = False
        self.assertResponseEqual(views.asset_put('whatever'), Responses.not_found())

    @patch('asset_uploader.app.views._asset_is_available')
    def test_asset_put_returns_ok_if_asset_found(self, mock_class):
        mock_class.return_value = True
        self.assertResponseEqual(views.asset_put('whatever'), Responses.ok({'status': 'uploaded'}))

    @patch('asset_uploader.app.s3.S3.client')
    def test_asset_get_returns_forbidden_if_s3_client_is_none(self, mock_class):
        mock_class.return_value = None
        self.assertResponseEqual(views.asset_get('whatever'), Responses.forbidden())

    @patch('asset_uploader.app.views._asset_is_available')
    def test_asset_get_returns_not_available_if_asset_not_found(self, mock_class):
        mock_class.return_value = False
        self.assertResponseEqual(views.asset_get('whatever'), Responses.not_found())

    @patch('asset_uploader.app.views._asset_is_available')
    @patch('asset_uploader.app.s3.S3.client')
    def test_asset_get_returns_not_available_if_generate_presigned_url_fails(self, mock_client_class,
                                                                             mock_available_class):
        mock_client_class.return_value = MagicMock()
        mock_client_class.return_value.generate_presigned_url.side_effect = Exception
        mock_available_class.return_value = True
        mock_request = TestViews._mock_request()
        self.assertResponseEqual(views.asset_get(mock_request), Responses.not_found())

    @patch('asset_uploader.app.s3.S3.client')
    def test_asset_get_returns_ok_if_generate_presigned_url_succeeds(self, mock_class):
        mock_class.return_value = MagicMock()
        mock_class.return_value.generate_presigned_url.return_value = TestViews._MOCK_PRESIGNED_URL
        mock_request = TestViews._mock_request()

        for query_str, timeout in [('', 60), ('timeout', 60), ('timeout=', 60), ('timeout=-123', 60),
                                   ('timeout=hello_world', 60), ('timeout=1', 1), ('timeout=100', 100)]:
            type(mock_request).query_string = PropertyMock(return_value=query_str)

            self.assertResponseEqual(views.asset_get(mock_request),
                                     Responses.ok({'download_url': TestViews._MOCK_PRESIGNED_URL}))

            mock_class.return_value.generate_presigned_url.assert_called_with(
                ClientMethod='get_object',
                ExpiresIn=timeout,
                Params={
                    'Bucket': S3.bucket(),
                    'Key': TestViews._MOCK_ASSET_ID
                },
                HttpMethod='GET')


if __name__ == '__main__':
    ut_main(verbosity=2)
