# -*- coding: utf-8 -*-

from unittest import TestCase, main as ut_main
from unittest.mock import patch

import asset_uploader


class TestMain(TestCase):

    @patch('pyramid.config.Configurator')
    def test_main(self, mock_class):
        asset_uploader.main({})

        mock_class.assert_called_once_with(settings={})

        mock_instance = mock_class.return_value
        mock_instance.include.assert_called_once_with('asset_uploader.app')
        mock_instance.scan.assert_called_once_with(package='asset_uploader')
        mock_instance.make_wsgi_app.assert_called_once_with()


if __name__ == '__main__':
    ut_main(verbosity=2)
