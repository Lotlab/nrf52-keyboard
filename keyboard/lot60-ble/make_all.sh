#!/bin/sh

COMPILE_FILES="rev_c rev_e rev_e_3led"
OUTPUT_DIR="_build"

VERSION=`git describe --always --dirty || echo 'unknown'`
BUILDTIME=`date +"%Y_%m_%d"`
FILE_VERSION="$BUILDTIME-$VERSION"
ARG=$1

if [ "$ARG" = "clean" ]; then
    rm -rf _build
    for var in $COMPILE_FILES; do
        make -C $var clean
    done
else
    if [ ! -d "$OUTPUT_DIR" ]; then 
        mkdir "$OUTPUT_DIR"
    fi
    for var in $COMPILE_FILES; do
        NRF_PACKAGE_NAME=../$OUTPUT_DIR/$var-nrf52-$FILE_VERSION.zip make -C $var package -j
        make -C $var usb -j
        cp $var/_build/ch554.hex $OUTPUT_DIR/$var-ch554-$FILE_VERSION.hex
        echo "$var-$FILE_VERSION"
    done
fi

