# asset-uploader

[![Build Status](https://travis-ci.com/CHr15F0x/asset-uploader.svg?token=hWa4W6EnvgUqM3Wa4JzH&branch=master)](https://travis-ci.com/CHr15F0x/asset-uploader) [![codecov](https://codecov.io/gh/CHr15F0x/asset-uploader/branch/master/graph/badge.svg)](https://codecov.io/gh/CHr15F0x/asset-uploader)
# Get the sources
```
git clone https://github.com/CHr15F0x/asset-uploader.git
cd asset-uploader
git submodule update --init --recursive
```
# Set up your AWS S3 bucket first
in __asset_uploader/docker/uwsgi_app/asset_uploader/app/s3.py__:
```
_AWS_ACCESS_KEY_ID = 'add-access-key-here'
_AWS_SECRET_ACCESS_KEY = 'add-secret-access-key-here'
_BUCKET = 'add-bucket-name-here'
_REGION = 'add-region-here'
```
# Build installation bundle

```
sudo ./make-bundle.sh
```

# Install service from bundle
```
sudo ./asset-uploader-bundle.run
```
After successful installation you can check if the service is running:
```
systemctl is-active asset-uploader.service
```
You can perform a quick test to check the service is replying correctly:
```
curl -X POST localhost/asset
{"upload_url": "https://some-bucket-name.s3.some-region-name.amazonaws.com/99d19b7238404e3fb193fb4abb68129a?(...)", "id": "99d19b7238404e3fb193fb4abb68129a"}
```

# Instructions for developers
## Setup and run python virtual environment

```
python3 -m venv ./venv
source ./venv/bin/activate
```
## Install asset-uploader package and run unit tests
```
cd asset_uploader/docker/uwsgi_app
pip3 install -e .
pip3 install -r ./requirements-dev.txt 
python3 ./setup.py test
```
## Run end to end test on the asset-uploader service
```
./end2end_test.py
```
## Run upload and download examples on the asset-uploader service
```
./upload_example.py a-file.txt 
Upload OK, asset-id: 55ef2670f9d14c64aabe99b10971b44b
./download_example.py 55ef2670f9d14c64aabe99b10971b44b
Asset 55ef2670f9d14c64aabe99b10971b44b content was saved to file
```
## Run end to end test using local ad-hoc pserve dev server
```
export END2END_SPAWN_PSERVE=1
export END2END_SERVER='http://localhost:8888'
./end2end_test.py
```
## Run pserve dev server
```
./run-dev-server.sh
```
## Run upload and download examples using dev server
```
export UPLOAD_SERVER='http://localhost:8888'
./upload_example.py a-file.txt 
Upload OK, asset-id: c5be537cefa143a6960c0145951d4e23
./download_example.py c5be537cefa143a6960c0145951d4e23
Asset c5be537cefa143a6960c0145951d4e23 content was saved to file
```
## Tested on
__Ubuntu 16.04.5__

__Arch__

