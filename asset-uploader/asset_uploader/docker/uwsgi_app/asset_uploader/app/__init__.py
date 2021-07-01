# -*- coding: utf-8 -*-
from asset_uploader.app.routes import add_routes


def includeme(config):
    add_routes(config)
