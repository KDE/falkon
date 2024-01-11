# CMake version required. This must be the very first line, because it sets default policies affecting everything else
cmake_minimum_required(VERSION 3.18)

# KDE Gear Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "24")
set(RELEASE_SERVICE_VERSION_MINOR "04")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

# Project name and version
project(Falkon VERSION ${RELEASE_SERVICE_VERSION})

set(KF_MAJOR_VERSION "6")
set(QT_MIN_VERSION "6.4.0")
set(KF_MIN_VERSION "5.240.0")

# Find ECM, with nice error handling in case of failure
include(FeatureSummary)
find_package(ECM ${KF_MIN_VERSION} CONFIG)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://invent.kde.org/frameworks/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Many includes from ECM, to get all the nice cmake functions and settings
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)
include(ECMPoQmTools)

# Output dirs (like ECM 5.38 does)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (UNIX AND NOT APPLE)
    set(FALKON_INSTALL_PLUGINDIR "${KDE_INSTALL_PLUGINDIR}/falkon")
else()
    set(FALKON_INSTALL_PLUGINDIR "${KDE_INSTALL_PLUGINDIR}")
endif()
if (IS_ABSOLUTE ${FALKON_INSTALL_PLUGINDIR})
    set(PLUGIN_PATH "${FALKON_INSTALL_PLUGINDIR}")
else()
    set(PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/${FALKON_INSTALL_PLUGINDIR}")
endif()
if (NOT WIN32)
    set(FALKON_PLUGIN_PATH "${PLUGIN_PATH}" CACHE PATH "Default plugin search path")
endif()

# Defines that are always set
add_definitions(-DQT_NO_URL_CAST_FROM_STRING -DQT_USE_QSTRINGBUILDER -DQT_NO_CAST_TO_ASCII)

# Mandatory: Qt5/6
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Widgets Network Sql QuickWidgets PrintSupport WebChannel)
find_package(Qt6WebEngineCore ${QT_MIN_VERSION} REQUIRED)
find_package(Qt6WebEngineWidgets ${QT_MIN_VERSION} REQUIRED)
if (BUILD_TESTING)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test)
endif()
if (NOT DISABLE_DBUS)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS DBus)
endif()

if (UNIX AND NOT APPLE AND NOT NO_X11)
    add_definitions(-DQZ_WS_X11)
    find_package(XCB REQUIRED COMPONENTS XCB UTIL)
endif()
if (WIN32)
    add_definitions(-DW7API)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS WinExtras)

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

find_package(Qt6Core5Compat)

# Mandatory: OpenSSL
find_package(OpenSSL REQUIRED)

# Mandatory: KF6
find_package(KF6 REQUIRED COMPONENTS Archive)

# KF5I18n: Mandatory with downloaded translations (only for ki18n_install)
if (EXISTS "${CMAKE_SOURCE_DIR}/po")
    find_package(KF6I18n REQUIRED)
endif()

# Optional: GnomeKeyring
find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    option(BUILD_KEYRING "Gnome keyring password plugin" ON)
    if (BUILD_KEYRING)
        pkg_check_modules(GNOME_KEYRING IMPORTED_TARGET gnome-keyring-1 )
    endif()
endif()

# Optional: KWallet, KIO, KCrash, KCoreAddons
find_package(KF6Wallet ${KF_MIN_VERSION} CONFIG)
set_package_properties(KF6Wallet PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
find_package(KF6KIO ${KF_MIN_VERSION} CONFIG)
set_package_properties(KF6KIO PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
find_package(KF6Crash ${KF_MIN_VERSION} CONFIG)
set_package_properties(KF6Crash PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
find_package(KF6CoreAddons ${KF_MIN_VERSION} CONFIG)
set_package_properties(KF6CoreAddons PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
find_package(KF6Purpose ${KF_MIN_VERSION} CONFIG)
set_package_properties(KF6Purpose PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
find_package(KF6JobWidgets ${KF_MIN_VERSION} CONFIG)
set_package_properties(KF6JobWidgets PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
if (KF6Wallet_FOUND AND KF6KIO_FOUND AND KF6Crash_FOUND AND KF6CoreAddons_FOUND AND KF6Purpose_FOUND AND KF6JobWidgets_FOUND)
    set(ENABLE_KDE_FRAMEWORKS_INTEGRATION_PLUGIN TRUE)
endif()

# Optional: PySide6
option(BUILD_PYTHON_SUPPORT "Support for Python plugins" ON)
if (BUILD_PYTHON_SUPPORT)
    find_package(Python3 COMPONENTS Development)
    find_package(PySide6 "6.4.0")
    find_package(Shiboken6 "6.4.0")
    find_package(Shiboken6Tools "6.4.0")
    set_package_properties(Python3 PROPERTIES DESCRIPTION "Python plugins" TYPE OPTIONAL)
    set_package_properties(PySide6 PROPERTIES DESCRIPTION "Python plugins" TYPE OPTIONAL)
    set_package_properties(Shiboken6 PROPERTIES DESCRIPTION "Python plugins" TYPE OPTIONAL)
    set_package_properties(Shiboken6Tools PROPERTIES DESCRIPTION "Python plugins" TYPE OPTIONAL)
    if (PySide6_FOUND AND Shiboken6_FOUND AND Shiboken6Tools_FOUND AND Python3_FOUND)
        set(ENABLE_PYTHON_PLUGINS TRUE)
    endif()
endif()

find_package(Intl)
if (Intl_FOUND)
    set(HAVE_LIBINTL TRUE)
endif()

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
    set(FALKON_GIT_REVISION "${GIT_REVISION}")
  else()
    message(STATUS "Git revision could not be determined")
  endif()
endif()

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

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

if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests/benchmarks)
endif()

# Tell releaseme that po is already taken care of
# SKIP_PO_INSTALL

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
