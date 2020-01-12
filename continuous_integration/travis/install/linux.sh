#!/bin/sh

set -e
set -v
set -x

ARCH="$1"

sudo apt-get update -yq

APT_GET_INSTALL='sudo -E apt-get -yq --no-install-suggests --no-install-recommends install'

case "${ARCH}" in
	x64)
		${APT_GET_INSTALL} libsdl2-dev
		;;

	x86)
		${APT_GET_INSTALL} libc6:i386 libstdc++6:i386 gcc-multilib g++-multilib zlib1g-dev:i386 libpng-dev:i386 libjpeg-turbo8-dev:i386 libsdl2-dev:i386
		;;
esac