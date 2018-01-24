include_directories(
  ../lib/3rdparty
  ../lib/app
  ../lib/session
  ../lib/webengine
  ../lib/webtab
)
set(SRCS main.cpp)
if (WIN32)
    set(SRCS ${SRCS} appicon.rc)
endif()
if (${CMAKE_SYSTEM} MATCHES "OS2")
    set(SRCS ${SRCS} appicon_os2.rc)
endif()
add_executable(falkon ${SRCS})

target_link_libraries(falkon FalkonPrivate)

if (APPLE)
    set_target_properties(falkon PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/main/Info.plist)
    set_target_properties(falkon PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.falkon")
    set_target_properties(falkon PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Falkon")
    install(FILES lib/data/icons/exeicons/qupzilla.icns DESTINATION ${BUNDLE_INSTALL_DIR}/falkon.app/Contents/Resources)
    #install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/locale DESTINATION ${BUNDLE_INSTALL_DIR}/falkon.app/Contents/Resources)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${BUNDLE_INSTALL_DIR}/falkon.app/Contents/Resources)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins DESTINATION ${BUNDLE_INSTALL_DIR}/falkon.app/Contents/Resources)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES FreeBSD OR CMAKE_SYSTEM_NAME MATCHES OpenBSD)
    target_link_libraries(falkon execinfo)
endif()

if (UNIX AND NOT APPLE)
    install(TARGETS falkon ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
    #install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/locale DESTINATION ${CMAKE_INSTALL_DATAROOTDIR})
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/falkon)

    install(PROGRAMS ../../linux/applications/org.kde.falkon.desktop DESTINATION ${KDE_INSTALL_APPDIR})

    install(FILES ../../linux/pixmaps/qupzilla.png DESTINATION ${KDE_INSTALL_DATADIR}/pixmaps)

    ecm_install_icons(ICONS
        ../../linux/hicolor/16-apps-qupzilla.png
        ../../linux/hicolor/32-apps-qupzilla.png
        ../../linux/hicolor/48-apps-qupzilla.png
        ../../linux/hicolor/64-apps-qupzilla.png
        ../../linux/hicolor/128-apps-qupzilla.png
        ../../linux/hicolor/256-apps-qupzilla.png
        DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor)

    install(FILES ../../linux/completion/falkon DESTINATION ${KDE_INSTALL_DATADIR}/bash-completion/completions)
    install(FILES ../../linux/appdata/org.kde.falkon.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
endif()

