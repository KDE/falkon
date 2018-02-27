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
ecm_create_qm_loader(SRCS falkon_qt)
add_executable(falkon ${SRCS})

target_link_libraries(falkon FalkonPrivate)

# Same as ecm_install_po_files_as_qm, but only for *_qt.po
function(install_po_files_as_qm podir)
    if (LOCALE_INSTALL_DIR)
        set(locale_install_destination "${LOCALE_INSTALL_DIR}")
    elseif (CMAKE_INSTALL_LOCALEDIR)
        set(locale_install_destination "${CMAKE_INSTALL_LOCALEDIR}")
    else()
        set(locale_install_destination share/locale)
    endif()
    file(GLOB po_files "${podir}/*/*_qt.po")
    foreach(po_file ${po_files})
        get_filename_component(po_dir ${po_file} DIRECTORY)
        get_filename_component(lang ${po_dir} NAME)
        ecm_process_po_files_as_qm(
            ${lang} ALL
            PO_FILES ${po_file}
            INSTALL_DESTINATION ${locale_install_destination}
        )
    endforeach()
endfunction()

# Same as ki18n_install, but skips *_qt.po
function(install_po_files podir)
    if (LOCALE_INSTALL_DIR)
        set(locale_install_destination "${LOCALE_INSTALL_DIR}")
    elseif (CMAKE_INSTALL_LOCALEDIR)
        set(locale_install_destination "${CMAKE_INSTALL_LOCALEDIR}")
    else()
        set(locale_install_destination share/locale)
    endif()
    get_filename_component(dirname ${locale_install_destination} NAME)
    get_filename_component(destname ${locale_install_destination} DIRECTORY)

    get_filename_component(absolute_podir ${podir} ABSOLUTE)
    string(MD5 pathmd5 ${absolute_podir})

    add_custom_target(pofiles-${pathmd5} ALL
        COMMENT "Generating mo..."
        COMMAND ${CMAKE_COMMAND}
                -DGETTEXT_MSGFMT_EXECUTABLE=${GETTEXT_MSGFMT_EXECUTABLE}
                -DCOPY_TO=${CMAKE_CURRENT_BINARY_DIR}/${dirname}
                -DPO_DIR=${absolute_podir}
                -P ${CMAKE_SOURCE_DIR}/cmake/build-po-files.cmake
    )

    if (NOT TARGET pofiles)
        add_custom_target(pofiles)
    endif()
    add_dependencies(pofiles pofiles-${pathmd5})

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dirname})
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dirname} DESTINATION ${destname})
endfunction()

if (APPLE)
    set_target_properties(falkon PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/main/Info.plist)
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
    install_po_files_as_qm(${CMAKE_SOURCE_DIR}/po)
    install_po_files(${CMAKE_SOURCE_DIR}/po)

    install(PROGRAMS ../../linux/applications/org.kde.falkon.desktop DESTINATION ${KDE_INSTALL_APPDIR})

    install(FILES ../../linux/pixmaps/falkon.png DESTINATION ${KDE_INSTALL_DATADIR}/pixmaps)

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

