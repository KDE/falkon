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

ecm_create_qm_loader( MouseGestures_SRCS falkon_mousegestures_qt )

set( MouseGestures_UIS
	mousegesturessettingsdialog.ui
	)
qt_wrap_ui(UIS ${MouseGestures_UIS})

set( MouseGestures_RSCS
	mousegestures.qrc
	)
qt_add_resources(RSCS ${MouseGestures_RSCS})

KDE_ENABLE_EXCEPTIONS()

add_library(MouseGestures MODULE ${MouseGestures_SRCS} ${UIS} ${RSCS})
install(TARGETS MouseGestures DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(MouseGestures FalkonPrivate)

