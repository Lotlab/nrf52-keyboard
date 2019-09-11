#!/bin/sh

VERSION=`git describe --always --dirty || echo 'unknown'`
BUILDTIME=`date +"%Y_%m_%d"`
FILE_VERSION="$BUILDTIME-$VERSION"
COMPILE_FILES="rev_c rev_e rev_e_3led"

rm -rf _build
mkdir -p _build

for var in $COMPILE_FILES; do
    NRF_PACKAGE_NAME=../_build/$var-nrf52-$FILE_VERSION.zip make -C $var package -j
    make -C $var usb -j
    cp $var/_build/ch554.hex _build/$var-ch554-$FILE_VERSION.hex
    echo "$var-$FILE_VERSION"
done