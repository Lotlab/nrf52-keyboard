#!/bin/sh

cd /work

cp template/Makefile.posix.template template/Makefile.posix
sed -i "s/\/usr\/local/\/opt/g" template/Makefile.posix
cp template/Makefile.posix SDK/components/toolchain/gcc/Makefile.posix
