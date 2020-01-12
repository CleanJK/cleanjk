#!/bin/sh

set -e
set -v
set -x

ARCH="$1"
BUILD_TYPE="$2"

mkdir build
cd build

case "${ARCH}" in
	x64)
		cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
		;;

	x86)
		cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32
		;;
esac

make