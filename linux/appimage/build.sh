#!/usr/bin/env bash

FALKON_URL=$1
RUNTIME_URL="https://github.com/AppImage/AppImageKit/releases/download/continuous/runtime-x86_64"

if [ -z "$FALKON_URL" ]; then
    echo "No url specified!"
    exit 1
fi

source /root/env.sh
source /opt/rh/devtoolset-4/enable

cd /root

wget $RUNTIME_URL -O runtime

wget $FALKON_URL -O falkon.tar.xz
tar xf falkon.tar.xz
cd falkon-*

/root/build-appimage.sh \
    --sourcedir=`pwd` \
    --outdir=/out \
    --runtime=/root/runtime \
    --qmake=$QTDIR/bin/qmake
