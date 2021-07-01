#!/usr/bin/env bash

# Stop all services
echo "---> Stopping asset-uploader service ..."

systemctl stop asset-uploader.service > /dev/null
systemctl disable asset-uploader.service > /dev/null

# Stop all docker containers, remove all containers and all images
echo "---> Removing docker containers and images..."

docker rmi -f nginx-asset-uploader
docker rmi -f uwsgi-asset-uploader

echo "---> Removing other files ..."
rm -rf /etc/asset-uploader
rm -f /etc/systemd/system/asset-uploader.service

# Make systemd aware of the changes
systemctl daemon-reload

echo "---> Done!"
