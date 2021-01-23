#!/bin/sh

cd /work

cp template/Makefile.posix.template template/Makefile.posix
sed -i "s/\/usr\/local/\/opt\/gcc-arm/g" template/Makefile.posix
ls -lh /opt/sdk
cp -r /opt/sdk/nRF5_SDK_15.3.0_59ac345/* SDK/
rm -rf tmk/tmk_core/protocol/usb_hid/USB_Host_Shield_2.0-tmk
