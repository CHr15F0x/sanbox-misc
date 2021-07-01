# -*- coding: utf-8 -*-

from logging import getLogger

import boto3

LOG = getLogger(__name__)

_AWS_ACCESS_KEY_ID = 'add-access-key-here'
_AWS_SECRET_ACCESS_KEY = 'add-secret-access-key-here'
_BUCKET = 'add-bucket-name-here'
_REGION = 'add-region-here'


class S3:
    client_instance = None

    @classmethod
    def client(cls):
        """
        Get AWS S3 client

        :return: Lazily created AWS S3 client or None if error occurred
        """

        if not cls.client_instance:
            try:
                cls.client_instance = boto3.client(
                    's3',
                    aws_access_key_id=_AWS_ACCESS_KEY_ID,
                    aws_secret_access_key=_AWS_SECRET_ACCESS_KEY,
                    region_name=_REGION,
                )
            except Exception as e:
                LOG.critical('EXCEPTION: ' + str(e))

        return cls.client_instance

    @classmethod
    def bucket(cls):
        return _BUCKET
