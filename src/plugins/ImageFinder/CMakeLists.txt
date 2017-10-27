set( ImageFinder_SRCS
	imagefinderplugin.cpp
	imagefinder.cpp
	imagefindersettings.cpp
	)

set( ImageFinder_UIS
	imagefindersettings.ui
	)
qt5_wrap_ui(UIS ${ImageFinder_UIS})

set( ImageFinder_RSCS
	imagefinder.qrc
	)
qt5_add_resources(RSCS ${ImageFinder_RSCS})

add_library(ImageFinder MODULE ${ImageFinder_SRCS} ${UIS} ${RSCS})
install(TARGETS ImageFinder DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(ImageFinder FalkonPrivate)

