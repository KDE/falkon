set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins")

add_subdirectory(AutoScroll)
add_subdirectory(FlashCookieManager)
add_subdirectory(GreaseMonkey)
add_subdirectory(ImageFinder)
add_subdirectory(MouseGestures)
add_subdirectory(PIM)
add_subdirectory(StatusBarIcons)
add_subdirectory(TabManager)
add_subdirectory(VerticalTabs)
add_subdirectory(YoutubeDownload)

if (GNOME_KEYRING_FOUND)
    add_subdirectory(GnomeKeyringPasswords)
endif()

if (KF5Wallet_FOUND)
    add_subdirectory(KWalletPasswords)
endif()

if (ENABLE_PYTHON_PLUGINS)
    add_subdirectory(PyFalkon)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(TestPlugin)
endif()
