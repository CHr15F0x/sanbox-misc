# -*- coding: utf-8 -*-

from unittest import TestCase, main as ut_main
from unittest.mock import patch

import asset_uploader.app


class TestAppInit(TestCase):

    @patch('asset_uploader.app.add_routes')
    def test_includeme(self, mock_class):
        asset_uploader.app.includeme('some-config')
        mock_class.assert_called_once_with('some-config')


if __name__ == '__main__':
    ut_main(verbosity=2)
