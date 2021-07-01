# -*- coding: utf-8 -*-

from json import dumps

from pyramid.response import Response as PyramidResponse


class Responses:
    _BODY_403 = """\
    <html>
     <head>
      <title>403 Forbidden</title>
     </head>
     <body>
      <h1>403 Forbidden</h1>
      Invalid AWS credentials.<br/><br/>
     </body>
    </html>
    """

    _BODY_404 = """\
    <html>
     <head>
      <title>404 Not Found</title>
     </head>
     <body>
      <h1>404 Not Found</h1>
      The resource could not be found.<br/><br/>
     </body>
    </html>
    """

    @classmethod
    def ok(cls, json_dict):
        return PyramidResponse(body=dumps(json_dict), charset='us-ascii', content_type='application/json')

    @classmethod
    def created(cls, json_dict):
        return PyramidResponse(status='201 Created', status_int=201, body=dumps(json_dict), charset='us-ascii',
                               content_type='application/json')

    @classmethod
    def forbidden(cls):
        return PyramidResponse(status='403 Forbidden', status_int=403, body=cls._BODY_403, charset='us-ascii',
                               content_type='html/text')

    @classmethod
    def not_found(cls):
        return PyramidResponse(status='404 Not Found', status_int=404, body=cls._BODY_404, charset='us-ascii',
                               content_type='html/text')
