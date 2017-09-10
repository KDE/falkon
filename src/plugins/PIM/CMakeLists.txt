set( PIM_SRCS
	PIM_plugin.cpp
	PIM_handler.cpp
	PIM_settings.cpp
	)

set( PIM_UIS
	PIM_settings.ui
	)
qt5_wrap_ui(UIS ${PIM_UIS})

set( PIM_RSCS
	PIM_res.qrc
	)
qt5_add_resources(RSCS ${PIM_RSCS})

add_library(PIM MODULE ${PIM_SRCS} ${UIS} ${RSCS})
install(TARGETS PIM DESTINATION ${KDE_INSTALL_PLUGINDIR})
target_link_libraries(PIM FalkonPrivate)

