# -*- coding: utf-8 -*-

from logging import getLogger
from uuid import uuid4

from pyramid.view import view_config

from .responses import Responses
from .s3 import S3

LOG = getLogger(__name__)


@view_config(route_name='new_asset', request_method='POST')
def asset_post(_request):
    if not S3.client():
        return Responses.forbidden()

    uuid = uuid4()

    try:
        url = S3.client().generate_presigned_url(
            ClientMethod='put_object',
            Params={
                'Bucket': S3.bucket(),
                'Key': uuid.hex,
                'ACL': 'public-read-write'
            },
            HttpMethod='PUT'
        )

    except Exception as e:
        LOG.error('EXCEPTION: ' + str(e))
        return Responses.not_found()

    return Responses.created({'upload_url': str(url), 'id': uuid.hex})


def _asset_id_from_request(request):
    return request.path.split('/')[-1]


def _asset_is_available(request):
    try:
        _response = S3.client().head_object(Bucket=S3.bucket(), Key=_asset_id_from_request(request))
    except Exception as e:
        LOG.error('EXCEPTION: ' + str(e))
        return False

    return True


@view_config(route_name='existing_asset', request_method='PUT')
def asset_put(request):
    if not S3.client():
        return Responses.forbidden()

    if not _asset_is_available(request):
        return Responses.not_found()

    return Responses.ok({'status': 'uploaded'})


@view_config(route_name='existing_asset', request_method='GET')
def asset_get(request):
    if not S3.client():
        return Responses.forbidden()

    if not _asset_is_available(request):
        return Responses.not_found()

    timeout_sec = 60
    query_elems = request.query_string.split('=')

    if len(query_elems) == 2 and query_elems[0] == 'timeout' and query_elems[1].isdecimal():
        timeout_sec = int(query_elems[1])

    try:
        url = S3.client().generate_presigned_url(
            ClientMethod='get_object',
            ExpiresIn=timeout_sec,
            Params={
                'Bucket': S3.bucket(),
                'Key': _asset_id_from_request(request)
            },
            HttpMethod='GET'
        )
    except Exception as e:
        LOG.error('EXCEPTION: ' + str(e))
        return Responses.not_found()

    return Responses.ok({'download_url': str(url)})
