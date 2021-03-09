#!/bin/sh
# SDCC
wget https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.0.0/sdcc-4.0.0-amd64-unknown-linux2.5.tar.bz2 -O /tmp/sdcc-4.0.0-amd64-unknown-linux2.5.tar.bz2
tar xf /tmp/sdcc-4.0.0-amd64-unknown-linux2.5.tar.bz2 -C /tmp
cp -r /tmp/sdcc-4.0.0/* /usr/local

# GCC
wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2 -O /tmp/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2
tar xf /tmp/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2 -C /opt

# SDK
wget http://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/nRF5_SDK_15.3.0_59ac345.zip -O /tmp/nRF5_SDK_15.3.0_59ac345.zip
unzip -q /tmp/nRF5_SDK_15.3.0_59ac345.zip -d /tmp
rm -r /tmp/nRF5_SDK_15.3.0_59ac345/examples /tmp/nRF5_SDK_15.3.0_59ac345/documentation /tmp/nRF5_SDK_15.3.0_59ac345/*.msi
mkdir ./SDK
mv /tmp/nRF5_SDK_15.3.0_59ac345/* ./SDK/

# Tool
wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/Desktop-software/nRF-command-line-tools/sw/Versions-10-x-x/10-12-1/nRFCommandLineTools10121Linuxamd64.tar.gz -O /tmp/nRF-command-line-tools.tar.gz
mkdir /tmp/tools
tar xf /tmp/nRF-command-line-tools.tar.gz -C /tmp/tools
tar xf /tmp/tools/nRF-Command-Line-Tools_10_12_1.tar -C /tmp/tools
cp -r /tmp/tools/mergehex /opt
ln -s /opt/mergehex/mergehex /usr/local/bin/mergehex

# Cleanup
rm -rf /tmp/*
