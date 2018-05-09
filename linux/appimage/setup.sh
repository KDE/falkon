#!/usr/bin/env bash
set -e

NCPUS=$(getconf _NPROCESSORS_ONLN)

QT_INSTALL_URL="https://download.qt.io/official_releases/qt/5.10/5.10.1/qt-opensource-linux-x64-5.10.1.run"

FRAMEWORK_VERSION="5.45"
CMAKE_FRAMEWORKS_URL="https://download.kde.org/stable/frameworks/${FRAMEWORK_VERSION}"
MODULE_VERSION="${FRAMEWORK_VERSION}.0"

QTDIR="/root/Qt/5.10.1/gcc_64"

trap "rm -rf *${MODULE_VERSION} *.tar.xz phonon* qt-installer* setup.sh" EXIT 1 2

# Install Qt
cd /root
wget $QT_INSTALL_URL -O qt-installer
chmod u+x qt-installer
./qt-installer --script qt-installer-noninteractive.qs --platform minimal -v || :

echo "export QTDIR=$QTDIR" > /root/env.sh

NEEDED_MODULES="ki18n
kcoreaddons
kconfig
kwindowsystem
kauth
kcodecs
kguiaddons
kwidgetsaddons
kconfigwidgets
kdbusaddons
karchive
kitemviews
kiconthemes
knotifications
kcrash
kservice
kwallet
solid
sonnet
kcompletion
ktextwidgets
attica
kglobalaccel
kxmlgui
kbookmarks
kjobwidgets
kio"

moduleBuild() {
    mkdir build && cd build
    cmake -DBUILD_TESTING=OFF -DBUILD_QCH=OFF -DCMAKE_PREFIX_PATH=$QTDIR/lib/cmake -DCMAKE_INSTALL_PREFIX=$QTDIR -DCMAKE_INSTALL_LIBDIR=lib ..
    make -j${NCPUS}
    make install
}

# Install ECM
wget "${CMAKE_FRAMEWORKS_URL}/extra-cmake-modules-${MODULE_VERSION}.tar.xz" -O extra-cmake-modules.tar.xz
tar xf extra-cmake-modules.tar.xz
pushd extra-cmake-modules-${MODULE_VERSION}
moduleBuild
popd

# Install PHONON
wget https://download.kde.org/stable/phonon/4.10.1/phonon-4.10.1.tar.xz -O phonon.tar.xz
tar xf phonon.tar.xz
pushd phonon-4.10.1
mkdir build && cd build
cmake \
    -DCMAKE_INSTALL_PREFIX:PATH=$QTDIR \
    -DCMAKE_INSTALL_LIBDIR:PATH=lib \
    -DPLUGIN_INSTALL_DIR:PATH=$QTDIR/plugins \
    -DPHONON_BUILD_PHONON4QT5=ON -DCMAKE_PREFIX_PATH=$QTDIR ..
make -j${NCPUS}
make install
popd

# Install RQUIRED MODULES
for MODULE_NAME in ${NEEDED_MODULES}; do
    wget "${CMAKE_FRAMEWORKS_URL}/${MODULE_NAME}-${MODULE_VERSION}.tar.xz" -O ${MODULE_NAME}.tar.xz
    tar xf ${MODULE_NAME}.tar.xz
    pushd ${MODULE_NAME}-${MODULE_VERSION}
    moduleBuild
    popd
done

pushd $QTDIR/plugins/kf5
chrpath --replace '$ORIGIN/../../lib' *.so
chrpath --replace '$ORIGIN/../../../lib' */*.so
popd
exit
