set( FlashCookieManager_SRCS
	fcm_plugin.cpp
	fcm_dialog.cpp
	fcm_notification.cpp
	)

set( FlashCookieManager_UIS
	fcm_dialog.ui
	fcm_notification.ui
	)
qt5_wrap_ui(UIS ${FlashCookieManager_UIS})

set( FlashCookieManager_RSCS
	flashcookiemanager.qrc
	)
qt5_add_resources(RSCS ${FlashCookieManager_RSCS})

add_library(FlashCookieManager MODULE ${FlashCookieManager_SRCS} ${UIS} ${RSCS})
install(TARGETS FlashCookieManager DESTINATION ${KDE_INSTALL_PLUGINDIR})
target_link_libraries(FlashCookieManager FalkonPrivate)

