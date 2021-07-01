# -*- coding: utf-8 -*-

from unittest import TestCase, main as ut_main
from asset_uploader.app.responses import Responses


class TestResponses(TestCase):

    def test_forbidden(self):
        response = Responses.forbidden()

        self.assertEqual(response.status_code, 403)
        self.assertEqual(response.status, '403 Forbidden')
        self.assertEqual(response.text, Responses._BODY_403)

    def test_not_found(self):
        response = Responses.not_found()

        self.assertEqual(response.status_code, 404)
        self.assertEqual(response.status, '404 Not Found')
        self.assertEqual(response.text, Responses._BODY_404)

    def test_ok(self):
        response = Responses.ok({'field': 'value'})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.status, '200 OK')
        self.assertDictEqual(response.json, {'field': 'value'})

    def test_created(self):
        response = Responses.created({'field': 'value'})

        self.assertEqual(response.status_code, 201)
        self.assertEqual(response.status, '201 Created')
        self.assertDictEqual(response.json, {'field': 'value'})


if __name__ == '__main__':
    ut_main(verbosity=2)
