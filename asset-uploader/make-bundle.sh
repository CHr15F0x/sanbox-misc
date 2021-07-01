#!/usr/bin/env bash

BUNDLE_NAME=asset-uploader-bundle

[[ `whoami` = root ]] || { echo "You need to be root!"; exit 1; }

echo "---> Building ${BUNDLE_NAME} ..."

install_package_arch() {
	echo "---> Looking for package:" $1
	pacman -Qi $1 > /dev/null || pacman -Sy --noconfirm $1 || { echo "Error: Failed to install package: $1" ; exit 1; }
}

install_package_ubuntu() {
	echo "---> Looking for package: "$1
	dpkg -l $1 > /dev/null || apt-get install -y $1 || { echo "Error: Failed to install package: $1" ; exit 1; }
}

PACKAGES_ARCH=( docker docker-compose )
PACKAGES_UBUNTU=( docker.io )

# Need to figure out distro
source /etc/os-release

case $NAME in
"Arch Linux")
	for i in "${PACKAGES_ARCH[@]}"
	do
		install_package_arch "$i"
	done
	;;
"Ubuntu")
	for i in "${PACKAGES_UBUNTU[@]}"
	do
		install_package_ubuntu "$i"
	done

	echo "---> Looking for: docker-compose"
	# Special procedure for docker-compose, which is too old in Ubuntu repos
	if [[ $(which docker-compose) == "" ]] ; then
		curl -L https://github.com/docker/compose/releases/download/1.23.2/docker-compose-`uname -s`-`uname -m` -o /usr/local/bin/docker-compose
		chmod a+rx /usr/local/bin/docker-compose
	fi

	;;
*)
	echo "---> Cannot determine distribution, no packages installed!"
	exit 1;
	;;
esac

# Sometimes docker daemon is down after install...
[[ $(systemctl is-active docker) == "inactive" ]] && { systemctl restart docker; }

echo "---> Building docker images ..."

cd asset_uploader/docker
docker build -f Dockerfile-nginx -t nginx-asset-uploader .
docker build -f Dockerfile-uwsgi -t uwsgi-asset-uploader .
docker save nginx-asset-uploader > ../bundle/nginx-asset-uploader.tar
docker save uwsgi-asset-uploader > ../bundle/uwsgi-asset-uploader.tar

cd ../../

echo "---> Packing files..."

third_party/makeself/makeself.sh --gzip --needroot asset_uploader/bundle ${BUNDLE_NAME}.run ${BUNDLE_NAME} ./post-inflate.sh

rm -f asset_uploader/bundle/nginx-asset-uploader.tar
rm -f asset_uploader/bundle/uwsgi-asset-uploader.tar

echo "---> Done!"

