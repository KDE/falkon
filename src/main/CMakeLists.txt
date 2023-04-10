include_directories(
  ../lib/3rdparty
  ../lib/app
  ../lib/session
  ../lib/webengine
  ../lib/webtab
)
set(SRCS main.cpp)
if (WIN32)
    set(SRCS ${SRCS} version.rc)
    configure_file(falkonversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/falkonversion.h)
endif()
ecm_create_qm_loader(SRCS falkon_qt)
add_executable(falkon ${SRCS})

target_link_libraries(falkon FalkonPrivate)

if (APPLE)
    set_target_properties(falkon PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    set_target_properties(falkon PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.falkon")
    set_target_properties(falkon PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Falkon")
    install(FILES lib/data/icons/exeicons/falkon.icns DESTINATION ${BUNDLE_INSTALL_DIR}/falkon.app/Contents/Resources)
    #install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/locale DESTINATION ${BUNDLE_INSTALL_DIR}/falkon.app/Contents/Resources)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${BUNDLE_INSTALL_DIR}/falkon.app/Contents/Resources)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins DESTINATION ${BUNDLE_INSTALL_DIR}/falkon.app/Contents/Resources)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES FreeBSD OR CMAKE_SYSTEM_NAME MATCHES OpenBSD)
    target_link_libraries(falkon execinfo)
endif()

if (UNIX AND NOT APPLE)
    install(TARGETS falkon ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/falkon)
    if (KF${KF_MAJOR_VERSION}I18n_FOUND)
        ki18n_install(${CMAKE_SOURCE_DIR}/po)
    endif()
    ecm_install_po_files_as_qm(${CMAKE_SOURCE_DIR}/poqm)

    install(PROGRAMS ../../linux/applications/org.kde.falkon.desktop DESTINATION ${KDE_INSTALL_APPDIR})

    ecm_install_icons(ICONS
        ../../linux/hicolor/16-apps-falkon.png
        ../../linux/hicolor/32-apps-falkon.png
        ../../linux/hicolor/48-apps-falkon.png
        ../../linux/hicolor/64-apps-falkon.png
        ../../linux/hicolor/128-apps-falkon.png
        ../../linux/hicolor/256-apps-falkon.png
        ../../linux/hicolor/sc-apps-falkon.svg
        DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor)

    install(FILES ../../linux/completion/falkon DESTINATION ${KDE_INSTALL_DATADIR}/bash-completion/completions)
    install(FILES ../../linux/appdata/org.kde.falkon.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
endif()

