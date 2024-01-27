set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins")

add_subdirectory(AutoScroll)
add_subdirectory(FlashCookieManager)
add_subdirectory(GreaseMonkey)
add_subdirectory(MouseGestures)
add_subdirectory(PIM)
add_subdirectory(StatusBarIcons)
add_subdirectory(TabManager)
add_subdirectory(VerticalTabs)

if (GNOME_KEYRING_FOUND)
    add_subdirectory(GnomeKeyringPasswords)
endif()

if (ENABLE_KDE_FRAMEWORKS_INTEGRATION_PLUGIN)
    add_subdirectory(KDEFrameworksIntegration)
endif()

if (ENABLE_PYTHON_PLUGINS)
    add_subdirectory(PyFalkon)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(TestPlugin)
    add_subdirectory(SiteSettingsView)
endif()
