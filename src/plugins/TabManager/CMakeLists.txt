add_definitions(-DTRANSLATION_DOMAIN="falkon_tabmanager")

set( TabManager_SRCS
	tabmanagerplugin.cpp
	tabmanagerwidget.cpp
	tabmanagerwidgetcontroller.cpp
	tabmanagersettings.cpp
	tabmanagerdelegate.cpp
	tldextractor/tldextractor.cpp
	)

set( TabManager_UIS
	tabmanagerwidget.ui
	tabmanagersettings.ui
	)
ki18n_wrap_ui(UIS ${TabManager_UIS})

set( TabManager_RSCS
	tabmanagerplugin.qrc
	tldextractor/tldextractor.qrc
	)
qt5_add_resources(RSCS ${TabManager_RSCS})

add_library(TabManager MODULE ${TabManager_SRCS} ${UIS} ${RSCS})
install(TARGETS TabManager DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(TabManager FalkonPrivate)

