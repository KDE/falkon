#!/bin/sh
###############################################################################
# Compile Falkon source and pack it as Appimage.
###############################################################################
set -e

SCRIPT_PATH="$(dirname "$(readlink -f "$0")")"
NCPUS=$(getconf _NPROCESSORS_ONLN)  || :

which mksquashfs >/dev/null 2>&1 || TEST=no
which chrpath >/dev/null 2>&1 || TEST=no
TEST=${TEST:-yes}

if [ $(which qmake 2>/dev/null) ]; then
SYSTEM_QMAKE=$(which qmake)
elif [ $(which qmake-qt5 2>/dev/null) ]; then
SYSTEM_QMAKE=$(which qmake-qt5)
fi

BLD1="\033[1m"
BLD0="\033[21m"
ITL1="\033[3m"
ITL0="\033[23m"
UDR1="\033[4m"
UDR0="\033[24m"
CRS1="\033[9m"
CRS0="\033[29m"
RDFG="\033[31m"
RDBG="\033[41m"
DFFG="\033[39m"
DFBG="\033[49m"
ALL0="\033[00m"

LIBDIRPATH=lib ; export LIBDIRPATH
SOURCE_DIR=${SOURCE_DIR:-${SCRIPT_PATH}/..} ; export SOURCE_DIR
QMAKE=${QMAKE:-$SYSTEM_QMAKE} ; export QMAKE
DEBUG_BUILD="-DCMAKE_BUILD_TYPE=Debug" ; export DEBUG_BUILD

CFLAGS="${CFLAGS:--O2 -g -pipe -Wall }" ; export CFLAGS ;
CXXFLAGS="${CXXFLAGS:--O2 -g -pipe -Wall }" ; export CXXFLAGS ;
LDFLAGS="${LDFLAGS:--Wl,-z,relro }"; export LDFLAGS ;

optPrint(){
printf "\n\t\t${ITL1}VALID OPTIONS ARE${ITL0}:\n
         --sourcedir=[path]
         --outdir=[path]
         --qmake=[path to executable]
         --disable-debug | -D
         --runtime=[path]
         --disable-x11
         --disable-dbus
         --update-source | -U
         --help | -h | help |-H\n\n"
}

helpPrint(){
printf "\n\t\t\t${ITL1}PARAMETERS${ITL0}:

${BLD1}--disable-x11${BLD0}

          Disable all X11 calls.
          Enable this when building for Wayland-only.
          All X11 calls are guarded by runtime X11 platform check
          even without this option.


${BLD1}--disable-dbus${BLD0}

          Build without QtDBus module. Native desktop notifications
          will be disabled.


${BLD1}--sourcedir=${BLD0}

          Assuming this script is located in ${ITL1}falkon/linux${ITL0},
          otherwise you must specify the path to
          Falkon source directory.

               ${UDR1}example:--sourcedir="/home/build/falkon"${UDR0}

${BLD1}--outdir=${BLD0}

          Where to copy final AppImage.

${BLD1}--runtime=[path]${BLD0}

          Path to precompiled „${BLD1}runtime.c${BLD0}“ ${ITL1}(part of AppImageKit)${ITL0}.
          More info at: ${UDR1}https://github.com/probonopd/AppImageKit${UDR0}
          ${BLD1}Mandatory option${BLD0}

${BLD1}--disable-debug | -D${BLD0}

          You may want to disable debug build.
          (enabled by default)


${BLD1}--update-source | -U${BLD0}

          Fetches the information from Falkon online git repository
          and merges it with your local copy


${BLD1}--qmake=${BLD0}

          Full path to qmake executable.
          This option is mandatory in case you want
          to create an AppImage.
\n"
}

printConf(){
printf "\n\tBuild configuration:\n
    Library path=${LIBDIRPATH}
    Source dir=${SOURCE_DIR}
    Debug build=${DEBUG_BUILD}
    Disable X11=${YNOX11}
    Disable DBUS=${DISABLE_DBUS}
    Runtime binary=${RUNTIME_BINARY}
    Qmake=${QMAKE}\n" | sed -r 's/=$/ » Not set/g'
}

getVal(){
echo $* | sed -r 's/([[:graph:]]*=)//'
}

varAssign(){
while [ $# != 0 ] ;do
        CFG_OPT="$1"
        case "${CFG_OPT}" in
         --sourcedir=*)
               SOURCE_DIR=$(getVal "${CFG_OPT}")
                export SOURCE_DIR
             ;;
         --outdir=*)
                OUT_DIR=$(getVal "${CFG_OPT}")
                export OUT_DIR
             ;;
         --disable-debug|-D)
                unset DEBUG_BUILD
             ;;
         --runtime=*)
                RUNTIME_BINARY=$(getVal "${CFG_OPT}")
                export RUNTIME_BINARY
             ;;
         --disable-x11)
                YNOX11="-DNO_X11:BOOL=TRUE"
                export YNOX11
             ;;
         --disable-bus)
                DISABLE_DBUS="-DDISABLE_DBUS:BOOL=TRUE"
                export DISABLE_DBUS
             ;;
         --qmake=*)
                QMAKE=$(getVal "${CFG_OPT}")
                export QMAKE
             ;;
         --update-source|-U)
                UPDATE_SOURCE="true"
                export UPDATE_SOURCE
             ;;
         --help|help|-h|-H)
                helpPrint
                exit 1
             ;;
        *)
                printf "\n${RDBG}unknown parameter: ${CFG_OPT}${DFBG}\n"
                optPrint
                exit 1
                ;;
     esac
     shift
done
}

nowBldImg(){

cd "${SOURCE_DIR}"

if [[ "${UPDATE_SOURCE}" == "true" ]]; then
git pull || :
fi

rm -fr build || :
mkdir build && cd build

QTFILESARETHERE=$(${QMAKE} -query | grep INSTALL_PREFIX | sed 's/QT_INSTALL_PREFIX://')
LIBSARETHERE=$(${QMAKE} -query | grep INSTALL_LIBS | sed 's/QT_INSTALL_LIBS://')
PLUGINSARETHERE=$(${QMAKE} -query | grep INSTALL_PLUGINS | sed 's/QT_INSTALL_PLUGINS://')
QMLSARETHERE=$(${QMAKE} -query | grep INSTALL_QML | sed 's/QT_INSTALL_QML://')
TRANSLATIONSARETHERE=$(${QMAKE} -query | grep INSTALL_TRANSLATIONS | sed 's/QT_INSTALL_TRANSLATIONS://')
LIBEXECSARETHERE=$(${QMAKE} -query | grep INSTALL_LIBEXECS | sed 's/QT_INSTALL_LIBEXECS://')

NODEFOPT="${DEBUG_BUILD} ${YNOX11} ${DISABLE_DBUS}"

cmake ${NODEFOPT}                     \
-DBUILD_SHARED_LIBS:BOOL=TRUE          \
-DCMAKE_SKIP_RPATH:BOOL=OFF             \
-DCMAKE_SKIP_INSTALL_RPATH:BOOL=NO       \
-DQMAKE_EXECUTABLE:FILEPATH=${QMAKE}      \
-DCMAKE_PREFIX_PATH=${QTFILESARETHERE}     \
-DFALKON_PLUGIN_PATH=""                     \
-DKDE_INSTALL_LIBDIR:PATH="${LIBDIRPATH}" ..

printf "Compiling Falkon with the following settings:\n"
printConf

make -j$NCPUS
if [[ $? == 0 ]] ; then
make DESTDIR="${PWD}" install
fi

mv usr/local bundle_build_dir
pushd bundle_build_dir/lib/plugins/falkon
chrpath --replace '$ORIGIN/../..' *.so
popd

NEEDEDLIBSLIST="libicudata.so.56
libicui18n.so.56
libicuuc.so.56
libQt5Core.so.5
libQt5DBus.so.5
libQt5Gui.so.5
libQt5Multimedia.so.5
libQt5MultimediaWidgets.so.5
libQt5Network.so.5
libQt5OpenGL.so.5
libQt5Positioning.so.5
libQt5PrintSupport.so.5
libQt5Qml.so.5
libQt5Quick.so.5
libQt5QuickWidgets.so.5
libQt5Sql.so.5
libQt5Svg.so.5
libQt5WebChannel.so.5
libQt5WebEngineCore.so.5
libQt5WebEngine.so.5
libQt5WebEngineWidgets.so.5
libQt5Widgets.so.5
libQt5X11Extras.so.5
libQt5XcbQpa.so.5
libQt5Concurrent.so.5
libQt5Xml.so.5"

NEEDEDPLUGINSLIST="bearer
generic
iconengines
imageformats
platforminputcontexts
platformthemes
printsupport
xcbglintegrations"

mkdir -p bundle_build_dir/plugins/{platforms,sqldrivers} \
bundle_build_dir/qtwebengine_dictionaries \
bundle_build_dir/qml \
bundle_build_dir/translations || :
for L in ${NEEDEDLIBSLIST} ; do
  cp -d ${LIBSARETHERE}/${L}* bundle_build_dir/lib ;
done

for P in ${NEEDEDPLUGINSLIST} ; do
  cp -r ${PLUGINSARETHERE}/${P} bundle_build_dir/plugins ;
done
if [[ -d "${PLUGINSARETHERE}/kf5/org.kde.kwindowsystem.platforms" ]]; then
mkdir bundle_build_dir/plugins/kf5
cp -r ${PLUGINSARETHERE}/kf5/org.kde.kwindowsystem.platforms bundle_build_dir/plugins/kf5
fi
install ${PLUGINSARETHERE}/platforms/libqxcb.so bundle_build_dir/plugins/platforms
install ${PLUGINSARETHERE}/sqldrivers/libqsqlite.so bundle_build_dir/plugins/sqldrivers
cp -r ${QMLSARETHERE}/{QtQuick.2,QtWebEngine} bundle_build_dir/qml
cp -r ${QTFILESARETHERE}/resources bundle_build_dir
cp -r ${TRANSLATIONSARETHERE}/qtwebengine_locales bundle_build_dir/translations
cp ${LIBEXECSARETHERE}/QtWebEngineProcess bundle_build_dir

CRYPTONEEDED="$(ldd 'bin/falkon'| grep libcrypto | sed 's/.*=>//;s/(.*//')"
LIBSSLNEEDED="$(echo "${CRYPTONEEDED}"  | sed 's/crypto/ssl/')"
install ${CRYPTONEEDED} ${LIBSSLNEEDED} bundle_build_dir/lib

CRYPTOLINKED="$(basename "${CRYPTONEEDED}")"
LIBSSLLINKED="$(basename "${LIBSSLNEEDED}")"

#Try to include all ssl v1.0 variants.
for LINK in {libcrypto.so,libcrypto.so.10,libcrypto.so.1.0.0} ; do
  if [[ ! -e bundle_build_dir/lib/${LINK} ]] ; then
    ln -s ${CRYPTOLINKED} bundle_build_dir/lib/${LINK}
  fi
done

for LNKS in {libssl.so,libssl.so.10,libssl.so.1.0.0} ; do
  if [[ ! -e bundle_build_dir/lib/${LNKS} ]] ; then
    ln -s ${CRYPTOLINKED} bundle_build_dir/lib/${LNKS}
  fi
done

cp ../linux/applications/org.kde.falkon.desktop bundle_build_dir
cp ../linux/hicolor/128-apps-falkon.png bundle_build_dir/falkon.png
ln -s falkon.png bundle_build_dir/.DirIcon

pushd bundle_build_dir
mv bin/falkon ./ && rm -fr bin
chrpath --replace '$ORIGIN' lib/libFalkonPrivate.so.3.*
chrpath --replace '$ORIGIN/lib' falkon
chrpath --replace '$ORIGIN/lib' QtWebEngineProcess

cat <<EOQTCFG >qt.conf
[Paths]
Plugins=plugins
Imports=qml
Qml2Imports=qml
LibraryExecutables=.
EOQTCFG

cat <<EOF >AppRun
#!/bin/sh

set -e

FALKON_DIR="\$(dirname "\$(readlink -f "\$0")")"

XDG_DATA_DIRS="\${FALKON_DIR}/share:\${XDG_DATA_DIRS}"
FALKON_PLUGIN_PATH="\${FALKON_DIR}/lib/plugins/falkon"
export XDG_DATA_DIRS FALKON_PLUGIN_PATH

cd "\${FALKON_DIR}/"
exec ./falkon "\$@"
EOF
chmod +x AppRun
popd

printf "Generating app image\n"
mksquashfs bundle_build_dir falkon.squashfs -root-owned -noappend

cat "${RUNTIME_BINARY}" >bin/Falkon.AppImage
cat falkon.squashfs >>bin/Falkon.AppImage
chmod a+x bin/Falkon.AppImage
}

varAssign $*

if [[ ! -x ${QMAKE} ]] ;then
printf "${RDFG}ERROR${DFFG}: ${BLD1}qmake${BLD0} was not found! Please use ${BLD1}--qmake=${BLD0} option to specify the path where it is located!\n"
exit 1
fi

if [[ ! -d "${SOURCE_DIR}/src" ]]; then
printf "Please install ${UDR1}$0${UDR0} in „${BLD1}scripts${BLD0}“ ${ITL1}(a sub folder in Falkon source directory)${ITL0},
or specify the source path with ${BLD1}--sourcedir=${BLD0}full/path!\n"
exit 1
fi

if [[ ${TEST} != "yes" ]]  ; then
printf "${RDFG}You must have the following tools installed:${DFFG}
          ${ITL1}mksquashfs, chrpath${ITL0}!\n"
exit 1
fi

if [[ "${QMAKE}" == "${SYSTEM_QMAKE}" ]] ; then
printf "${RDFG}You should use precompiled Qt package${DFFG}
downloaded from ${UDR1}${ITL1}http://download.qt.io/official_releases/qt/${ALL0}\n"
exit 1
elif
[[ -z ${RUNTIME_BINARY} ]] ; then
printf "\n${RDFG}Required precompiled „${BLD1}runtime${BLD0}“ binary!${DFFG}
It's a part of ${ITL1}AppImageKit${ITL0}
${UDR1}https://github.com/probonopd/AppImageKit${UDR0}\n"
exit 1
fi

nowBldImg

if [[ $? == 0 ]] && [[ -x bin/Falkon.AppImage ]]; then
 printf "\\033c"
 printf "Done!\nThe compiled files are in "${PWD}"/bin\n"
 if [ ! -z "$OUT_DIR" ]; then
     cp bin/Falkon.AppImage "$OUT_DIR"
 fi
fi

exit 0
