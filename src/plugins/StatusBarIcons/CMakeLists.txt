set( StatusBarIcons_SRCS
	statusbariconsplugin.cpp
	sbi_iconsmanager.cpp
	sbi_imagesicon.cpp
	sbi_javascripticon.cpp
	sbi_networkicon.cpp
	sbi_networkproxy.cpp
	sbi_proxywidget.cpp
	sbi_networkicondialog.cpp
	sbi_networkmanager.cpp
	sbi_settingsdialog.cpp
	sbi_icon.cpp
	sbi_zoomwidget.cpp

        statusbariconsplugin.h
        sbi_iconsmanager.h
        sbi_imagesicon.h
        sbi_javascripticon.h
        sbi_networkicon.h
        sbi_networkproxy.h
        sbi_proxywidget.h
        sbi_networkicondialog.h
        sbi_networkmanager.h
        sbi_settingsdialog.h
        sbi_icon.h
        sbi_zoomwidget.h
    )

ecm_create_qm_loader( StatusBarIcons_SRCS falkon_statusbaricons_qt )

set( StatusBarIcons_UIS
	sbi_proxywidget.ui
	sbi_networkicondialog.ui
	sbi_settingsdialog.ui
	)
qt_wrap_ui(UIS ${StatusBarIcons_UIS})

set( StatusBarIcons_RSCS
	statusbaricons.qrc
	)
qt_add_resources(RSCS ${StatusBarIcons_RSCS})

add_library(StatusBarIcons MODULE ${StatusBarIcons_SRCS} ${UIS} ${RSCS})
install(TARGETS StatusBarIcons DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(StatusBarIcons FalkonPrivate)

