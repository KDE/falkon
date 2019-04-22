add_definitions(-DTRANSLATION_DOMAIN="falkon_autoscroll")

set( AutoScroll_SRCS
	autoscrollplugin.cpp
	autoscroller.cpp
	framescroller.cpp
	autoscrollsettings.cpp
	)

set( AutoScroll_UIS
	autoscrollsettings.ui
	)
ki18n_wrap_ui(UIS ${AutoScroll_UIS})

set( AutoScroll_RSCS
	autoscroll.qrc
	)
qt5_add_resources(RSCS ${AutoScroll_RSCS})

add_library(AutoScroll MODULE ${AutoScroll_SRCS} ${UIS} ${RSCS})
install(TARGETS AutoScroll DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(AutoScroll FalkonPrivate)

