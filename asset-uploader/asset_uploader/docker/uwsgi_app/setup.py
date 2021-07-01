# -*- coding: utf-8 -*-
from setuptools import (
    setup,
    find_packages,
)

requires = [
    # AWS SDK for Python
    'boto3 >=1.9.98,<=1.9.999',
    # Pyramid-Paste integration
    'plaster_pastedeploy >=0.4.1,<=0.4.999',
    # Pyramid web-framework
    'pyramid >=1.9.1,<=1.9.999',
]

setup(
    name='asset_uploader',
    version='0.1',
    description='Asset uploader example',
    author='Krzysiek Lis',
    author_email='klis33@gmail.com',
    url='',
    packages=find_packages(),
    include_package_data=False,
    zip_safe=True,
    install_requires=requires,
    entry_points={
        'paste.app_factory': [
            'main = asset_uploader:main',
        ],
    },
    setup_requires=['pytest-runner'],
    tests_require=['pytest'],
)
