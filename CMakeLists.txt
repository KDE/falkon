# CMake version required. This must be the very first line, because it sets default policies affecting everything else
cmake_minimum_required(VERSION 3.1)

# Project name and version
project(Falkon VERSION 2.1.99)

# Find ECM, with nice error handling in case of failure
include(FeatureSummary)
find_package(ECM 5.27.0 CONFIG)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/frameworks/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Many includes from ECM, to get all the nice cmake functions and settings
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)

# Output dirs (like ECM 5.38 does)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (UNIX AND NOT APPLE)
    set(FALKON_INSTALL_PLUGINDIR "${KDE_INSTALL_PLUGINDIR}/falkon")
else()
    set(FALKON_INSTALL_PLUGINDIR "${KDE_INSTALL_PLUGINDIR}")
endif()
add_definitions(-DPLUGIN_PATH=\"${CMAKE_INSTALL_PREFIX}/${FALKON_INSTALL_PLUGINDIR}\")

# Version (TODO: move to a generated header once qmake support is dropped, to avoid full recompilations when changing this
add_definitions(-DFALKON_VERSION=\"${PROJECT_VERSION}\")

# Defines that are always set
add_definitions(-DQT_NO_URL_CAST_FROM_STRING -DQT_USE_QSTRINGBUILDER -DQT_NO_CAST_TO_ASCII)

# Configurable options (TODO: move all defines to a generated header)
option(NO_SYSTEM_DATAPATH "TODO" OFF)
if (NO_SYSTEM_DATAPATH)
    add_definitions(-DNO_SYSTEM_DATAPATH)
endif()
option(NO_X11 "TODO" OFF)
if (NO_X11)
    add_definitions(-DNO_X11)
endif()
option(PORTABLE_BUILD "TODO" OFF)
if (PORTABLE_BUILD)
    add_definitions(-DPORTABLE_BUILD)
endif()
option(NONBLOCK_JS_DIALOGS "TODO" OFF)
if (NONBLOCK_JS_DIALOGS)
    add_definitions(-DNONBLOCK_JS_DIALOGS)
endif()
option(DISABLE_DBUS "TODO" OFF)
if (DISABLE_DBUS)
    add_definitions(-DDISABLE_DBUS)
endif()
option(DISABLE_UPDATES_CHECK "TODO" OFF)
if (DISABLE_UPDATES_CHECK)
    add_definitions(-DDISABLE_UPDATES_CHECK)
endif()
# Note: the old qmake option DEBUG_BUILD is now -DCMAKE_BUILD_TYPE=Debug, and FALKON_PREFIX is now -DCMAKE_INSTALL_PREFIX
#       SHARE_FOLDER is now auto-detected.

# Mandatory: Qt5
set(QT_MIN_VERSION "5.8.0")
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Widgets Network Sql QuickWidgets PrintSupport WebEngineWidgets WebChannel Test)
if (NOT DISABLE_DBUS)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS DBus)
endif()

if (UNIX AND NOT APPLE AND NOT NO_X11)
    find_package(X11)
    if (X11_FOUND)
        add_definitions(-DQZ_WS_X11)
    endif()
    find_package(XCB COMPONENTS XCB)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS X11Extras)
endif()
if (WIN32)
    add_definitions(-DW7API)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS WinExtras)

    # taken from https://stackoverflow.com/a/40217291
    macro(get_WIN32_WINNT version)
        if (CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if ("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif ("${verMajor}" MATCHES "10")
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif(CMAKE_SYSTEM_VERSION)
    endmacro(get_WIN32_WINNT)

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
endif()

# Optional: GnomeKeyring
find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    option(BUILD_KEYRING "Gnome keyring password plugin" ON)
    if (BUILD_KEYRING)
        pkg_check_modules(GNOME_KEYRING gnome-keyring-1)
    endif()
endif()

# Optional: KWallet
set(KF5_MIN_VERSION "5.27.0")
find_package(KF5Wallet ${KF5_MIN_VERSION} CONFIG)
set_package_properties(KF5Wallet PROPERTIES DESCRIPTION "KWallet password backend plugin" TYPE OPTIONAL)

# Git revision
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git QUIET)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_REVISION
    )
    string(REGEX REPLACE "\n" "" GIT_REVISION "${GIT_REVISION}")
    add_definitions(-DGIT_REVISION=\"${GIT_REVISION}\")
  else()
    message(STATUS "Git revision could not be determined")
  endif()
endif()

# Include dirs used everywhere
include_directories(
     ${CMAKE_SOURCE_DIR}/src/lib/3rdparty
     ${CMAKE_SOURCE_DIR}/src/lib/adblock
     ${CMAKE_SOURCE_DIR}/src/lib/app
     ${CMAKE_SOURCE_DIR}/src/lib/autofill
     ${CMAKE_SOURCE_DIR}/src/lib/bookmarks
     ${CMAKE_SOURCE_DIR}/src/lib/cookies
     ${CMAKE_SOURCE_DIR}/src/lib/downloads
     ${CMAKE_SOURCE_DIR}/src/lib/history
     ${CMAKE_SOURCE_DIR}/src/lib/navigation
     ${CMAKE_SOURCE_DIR}/src/lib/network
     ${CMAKE_SOURCE_DIR}/src/lib/notifications
     ${CMAKE_SOURCE_DIR}/src/lib/opensearch
     ${CMAKE_SOURCE_DIR}/src/lib/other
     ${CMAKE_SOURCE_DIR}/src/lib/plugins
     ${CMAKE_SOURCE_DIR}/src/lib/popupwindow
     ${CMAKE_SOURCE_DIR}/src/lib/preferences
     ${CMAKE_SOURCE_DIR}/src/lib/session
     ${CMAKE_SOURCE_DIR}/src/lib/sidebar
     ${CMAKE_SOURCE_DIR}/src/lib/tabwidget
     ${CMAKE_SOURCE_DIR}/src/lib/tools
     ${CMAKE_SOURCE_DIR}/src/lib/webengine
     ${CMAKE_SOURCE_DIR}/src/lib/webtab
)

# Finally, go into the subdirs
add_subdirectory(src)
add_subdirectory(tests/autotests)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
