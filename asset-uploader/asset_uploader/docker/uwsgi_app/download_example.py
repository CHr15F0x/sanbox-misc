#! /usr/bin/env python3
# -*- coding: utf-8 -*-
import requests
from sys import argv
from os import getenv


def print_usage_and_exit():
    print("""\
Usage: download_example.py asset_id

Upload server:
    http://localhost or override with UPLOAD_SERVER environment variable
""")
    exit(-1)


def main():
    if len(argv) != 2:
        print_usage_and_exit()

    server_url = getenv('UPLOAD_SERVER', 'http://localhost')

    try:
        asset_id = argv[1]

        response = requests.get(server_url + '/asset/' + asset_id)

        if response.status_code != 200 and 'download_url' not in response.json():
            print('Invalid asset-id: ' + asset_id)
            print_usage_and_exit()

        response = requests.get(response.json()['download_url'])

        if response.status_code != 200:
            print('Download failed: ' + response.text)
            print_usage_and_exit()

        print('Asset ' + asset_id + ' content was saved to file')

        with open(asset_id, 'wb') as file:
            file.write(response.content)

    except Exception as e:
        print(str(e))
        print_usage_and_exit()


if __name__ == '__main__':
    main()
