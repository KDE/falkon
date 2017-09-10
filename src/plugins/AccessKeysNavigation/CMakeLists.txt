set( AccessKeysNavigation_SRCS
	akn_plugin.cpp
	akn_handler.cpp
	akn_settings.cpp
	)

set( AccessKeysNavigation_UIS
	akn_settings.ui
	)
qt5_wrap_ui(UIS ${AccessKeysNavigation_UIS})

set( AccessKeysNavigation_RSCS
	akn_res.qrc
	)
qt5_add_resources(RSCS ${AccessKeysNavigation_RSCS})

add_library(AccessKeysNavigation MODULE ${AccessKeysNavigation_SRCS} ${UIS} ${RSCS})
install(TARGETS AccessKeysNavigation DESTINATION ${KDE_INSTALL_PLUGINDIR})
target_link_libraries(AccessKeysNavigation FalkonPrivate)

