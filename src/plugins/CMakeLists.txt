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
