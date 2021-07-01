#! /usr/bin/env python3
# -*- coding: utf-8 -*-
import requests
from sys import argv
from os import getenv


def print_usage_and_exit():
    print("""\
Usage: upload_example.py file_name

Upload server:
    http://localhost or override with UPLOAD_SERVER environment variable
""")
    exit(-1)


def main():
    if len(argv) != 2:
        print_usage_and_exit()

    server_url = getenv('UPLOAD_SERVER', 'http://localhost')

    try:
        response = requests.post(server_url + '/asset')

        with open(argv[1], 'rb') as file:
            contents = file.read(-1)

        if response.status_code != 201 or 'upload_url' not in response.json() or 'id' not in response.json():
            print(str(response.text))
            print_usage_and_exit()

        asset_id = response.json()['id']

        response = requests.put(
            url=response.json()['upload_url'],
            data=contents)

        if response.status_code != 200:
            print(str(response.text))
            print_usage_and_exit()

        response = requests.put(server_url + '/asset/' + asset_id)

        if response.status_code != 200 or 'status' not in response.json() or response.json()['status'] != 'uploaded':
            print(str(response.text))
            print_usage_and_exit()
        else:
            print('Upload OK, asset-id: ' + asset_id)
            exit(0)

    except Exception as e:
        print(str(e))
        print_usage_and_exit()


if __name__ == '__main__':
    main()
