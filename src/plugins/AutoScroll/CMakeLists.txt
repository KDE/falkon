set( AutoScroll_SRCS
	autoscrollplugin.cpp
	autoscroller.cpp
	framescroller.cpp
	autoscrollsettings.cpp
        autoscrollplugin.h
        autoscroller.h
        framescroller.h
        autoscrollsettings.h
        )

ecm_create_qm_loader( AutoScroll_SRCS falkon_autoscroll_qt )

set( AutoScroll_UIS
	autoscrollsettings.ui
	)
qt_wrap_ui(UIS ${AutoScroll_UIS})

set( AutoScroll_RSCS
	autoscroll.qrc
	)
qt_add_resources(RSCS ${AutoScroll_RSCS})

add_library(AutoScroll MODULE ${AutoScroll_SRCS} ${UIS} ${RSCS})
install(TARGETS AutoScroll DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(AutoScroll FalkonPrivate)

