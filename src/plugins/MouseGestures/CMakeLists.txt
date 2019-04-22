add_definitions(-DTRANSLATION_DOMAIN="falkon_mousegestures")

include_directories(3rdparty)

set( MouseGestures_HDRS
	3rdparty/QjtMouseGestureFilter.h
	)

set( MouseGestures_SRCS
	3rdparty/mousegesturerecognizer.cpp
	3rdparty/QjtMouseGesture.cpp
	3rdparty/QjtMouseGestureFilter.cpp
	3rdparty/adv_recognizer.cpp
	mousegestures.cpp
	mousegesturesplugin.cpp
	mousegesturessettingsdialog.cpp
	)

set( MouseGestures_UIS
	mousegesturessettingsdialog.ui
	)
ki18n_wrap_ui(UIS ${MouseGestures_UIS})

set( MouseGestures_RSCS
	mousegestures.qrc
	)
qt5_add_resources(RSCS ${MouseGestures_RSCS})

KDE_ENABLE_EXCEPTIONS()

add_library(MouseGestures MODULE ${MouseGestures_SRCS} ${UIS} ${RSCS})
install(TARGETS MouseGestures DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(MouseGestures FalkonPrivate)

