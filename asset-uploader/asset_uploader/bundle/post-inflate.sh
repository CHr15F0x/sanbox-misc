#!/usr/bin/env bash
# Should be run as root

# Convenience script
cp uninstall-asset-uploader.sh /bin

# Need to figure out distribution
source /etc/os-release

echo "---> Installing asset-uploader ..."

install_package_arch() {
	echo "---> Looking for package:" $1
	pacman -Qi $1 > /dev/null || pacman -Sy --noconfirm $1 || { echo "Error: Failed to install package: $1" ; exit 1; }
}

install_package_ubuntu() {
	echo "---> Looking for package:" $1
	dpkg -l $1 > /dev/null || apt-get install -y $1 || { echo "Error: Failed to install package: $1" ; exit 1; }
}

PACKAGES_ARCH=( docker docker-compose )
PACKAGES_UBUNTU=( docker.io )

case ${NAME} in
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

	# Special procedure for docker-compose, which is too old in Ubuntu repos
	echo "---> Looking for: docker-compose"
	if [[ $(which docker-compose) == "" ]] ; then
		curl -L https://github.com/docker/compose/releases/download/1.23.2/docker-compose-`uname -s`-`uname -m` -o /usr/local/bin/docker-compose
		chmod a+rx /usr/local/bin/docker-compose
	fi

	;;
*)
	echo "---> Cannot determine distribution, no packages installed!"
	exit 1

	;;
esac

# Sometimes docker daemon is down after install ...
[[ $(systemctl is-active docker) == "inactive" ]] && { systemctl restart docker; }

# Load docker images
echo "---> Loading docker images ..."

cd etc/asset-uploader
docker-compose down
cd ../../
docker load < nginx-asset-uploader.tar
docker load < uwsgi-asset-uploader.tar

# Enable and run the service
echo "---> Enabling and running asset-uploader service ..."

cp -R etc/* /etc
systemctl daemon-reload
systemctl enable asset-uploader.service > /dev/null
systemctl restart asset-uploader.service > /dev/null

echo "---> Done!"
