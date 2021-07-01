# -*- coding: utf-8 -*-


def add_routes(config):
    config.add_route('new_asset', '/asset')
    config.add_route('existing_asset', '/asset/{_asset_id:[a-f0-9]{32}}')  # UUID has 32 hex digits
