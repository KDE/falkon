#!/usr/bin/env bash

QT_INSTALL_URL="https://download.qt.io/official_releases/qt/5.10/5.10.1/qt-opensource-linux-x64-5.10.1.run"
EXTRA_CMAKE_MODULES_URL="https://download.kde.org/stable/frameworks/5.45/extra-cmake-modules-5.45.0.tar.xz"
KI18N_URL="https://download.kde.org/stable/frameworks/5.45/ki18n-5.45.0.tar.xz"

QTDIR="/root/Qt/5.10.1/gcc_64"

# Install Qt
cd /root
wget $QT_INSTALL_URL -O qt-installer
chmod u+x qt-installer
./qt-installer --script qt-installer-noninteractive.qs --platform minimal -v

echo "export QTDIR=$QTDIR" > /root/env.sh

# Install ECM
cd /root
wget $EXTRA_CMAKE_MODULES_URL -O extra-cmake-modules.tar.xz
tar xf extra-cmake-modules.tar.xz
cd extra-cmake-modules-*
mkdir build && cd build
cmake -DBUILD_TESTING=OFF -DBUILD_QCH=OFF -DCMAKE_PREFIX_PATH=$QTDIR/lib/cmake -DCMAKE_INSTALL_PREFIX=$QTDIR -DCMAKE_INSTALL_LIBDIR=lib ..
make && make install

# Install KI18n
cd /root
wget $KI18N_URL -O ki18n.tar.xz
tar xf ki18n.tar.xz
cd ki18n-*
mkdir build && cd build
cmake -DBUILD_TESTING=OFF -DBUILD_QCH=OFF -DCMAKE_PREFIX_PATH=$QTDIR/lib/cmake -DCMAKE_INSTALL_PREFIX=$QTDIR -DCMAKE_INSTALL_LIBDIR=lib ..
make && make install

# Cleanup
cd /root
rm -r qt-installer* extra-cmake-modules* ki18n* setup.sh
