set( TabManager_SRCS
	tabmanagerplugin.cpp
	tabmanagerwidget.cpp
	tabmanagerwidgetcontroller.cpp
	tabmanagersettings.cpp
	tabmanagerdelegate.cpp
	tldextractor/tldextractor.cpp
        tabmanagerplugin.h
        tabmanagerwidget.h
        tabmanagerwidgetcontroller.h
        tabmanagersettings.h
        tabmanagerdelegate.h
        tldextractor/tldextractor.h
        )

ecm_create_qm_loader( TabManager_SRCS falkon_tabmanager_qt )

set( TabManager_UIS
	tabmanagerwidget.ui
	tabmanagersettings.ui
	)
qt_wrap_ui(UIS ${TabManager_UIS})

set( TabManager_RSCS
	tabmanagerplugin.qrc
	tldextractor/tldextractor.qrc
	)
qt_add_resources(RSCS ${TabManager_RSCS})

add_library(TabManager MODULE ${TabManager_SRCS} ${UIS} ${RSCS})
install(TARGETS TabManager DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(TabManager FalkonPrivate)

