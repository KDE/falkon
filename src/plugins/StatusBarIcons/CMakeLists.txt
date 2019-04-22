add_definitions(-DTRANSLATION_DOMAIN="falkon_statusbaricons")

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
	)

set( StatusBarIcons_UIS
	sbi_proxywidget.ui
	sbi_networkicondialog.ui
	sbi_settingsdialog.ui
	)
ki18n_wrap_ui(UIS ${StatusBarIcons_UIS})

set( StatusBarIcons_RSCS
	statusbaricons.qrc
	)
qt5_add_resources(RSCS ${StatusBarIcons_RSCS})

add_library(StatusBarIcons MODULE ${StatusBarIcons_SRCS} ${UIS} ${RSCS})
install(TARGETS StatusBarIcons DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(StatusBarIcons FalkonPrivate)

