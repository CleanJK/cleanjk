#!/bin/sh

set -e
set -v
set -x

ARCH="$1"
COMPILER="$2"
CPU_ARCH="$3"

APT_GET_INSTALL='sudo -E apt-get -yq --no-install-suggests --no-install-recommends install'

if [ "${CPU_ARCH}" = "amd64" ]; then
	sudo mkdir -p cmake-3.16.3 && sudo wget -qO- "https://cmake.org/files/v3.16/cmake-3.16.3-Linux-x86_64.tar.gz"  | sudo tar --strip-components=1 -xz -C cmake-3.16.3 ;
fi

case "${COMPILER}" in
	gcc)
		;;

	clang)
		sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
		;;
esac

sudo apt-get update -yq

case "${ARCH}" in
	x64)
		${APT_GET_INSTALL} \
			gcc-9 \
			g++-9 \
			libsdl2-dev
		;;

	x86)
		${APT_GET_INSTALL} \
			gcc-9-multilib \
			g++-9-multilib \
			libc6:i386 \
			libstdc++6:i386 \
			zlib1g-dev:i386 \
			libpng-dev:i386 \
			libjpeg-turbo8-dev:i386 \
			libsdl2-dev:i386
		;;
esac