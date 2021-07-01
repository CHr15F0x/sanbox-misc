# -*- coding: utf-8 -*-
import pyramid.config


def main(_global_config, **settings):
    """\
    Create, configure and return the WSGI application.

    """
    config = pyramid.config.Configurator(settings=settings)

    # ``app`` module providing app views:
    config.include('asset_uploader.app')

    # Done, good to go and look for view configs:
    config.scan(package='asset_uploader')

    return config.make_wsgi_app()
