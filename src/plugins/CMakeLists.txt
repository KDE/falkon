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
     ${CMAKE_SOURCE_DIR}/src/lib/rss
     ${CMAKE_SOURCE_DIR}/src/lib/session
     ${CMAKE_SOURCE_DIR}/src/lib/sidebar
     ${CMAKE_SOURCE_DIR}/src/lib/tabwidget
     ${CMAKE_SOURCE_DIR}/src/lib/tools
     ${CMAKE_SOURCE_DIR}/src/lib/webengine
     ${CMAKE_SOURCE_DIR}/src/lib/webtab
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins")

# QtWebEngine disable
#add_subdirectory(AccessKeysNavigation)

add_subdirectory(AutoScroll)
add_subdirectory(FlashCookieManager)

# GnomeKeyringPasswords only with GNOME_INTEGRATION and gnome-keyring pkg-config
if (GNOME_INTEGRATION AND GNOME_KEYRING_FOUND)
    add_subdirectory(GnomeKeyringPasswords)
endif()

add_subdirectory(GreaseMonkey)
add_subdirectory(ImageFinder)

# KWalletPasswords only with KDE_INTEGRATION and KWallet framework
if (KDE_INTEGRATION AND KF5Wallet_FOUND)
    add_subdirectory(KWalletPasswords)
endif()

add_subdirectory(MouseGestures)
add_subdirectory(PIM)
add_subdirectory(StatusBarIcons)
add_subdirectory(TabManager)

# TestPlugin only in debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(TestPlugin)
endif()

# Not ported to cmake, for lack of a testcase: addSubdir($$(FALKON_PLUGINS_SRCDIR))
