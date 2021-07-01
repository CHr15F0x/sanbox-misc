# -*- coding: utf-8 -*-

from unittest import TestCase, main as ut_main
from unittest.mock import call, MagicMock

import asset_uploader.app.routes


class TestRoutes(TestCase):

    def test_add_routes(self):
        mock_instance = MagicMock()
        asset_uploader.app.routes.add_routes(mock_instance)
        expected_calls = [call.add_route('new_asset', '/asset'),
                          call.add_route('existing_asset', '/asset/{_asset_id:[a-f0-9]{32}}')]
        mock_instance.assert_has_calls(expected_calls)


if __name__ == '__main__':
    ut_main(verbosity=2)
