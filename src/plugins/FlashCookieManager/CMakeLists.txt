set( FlashCookieManager_SRCS
	fcm_plugin.cpp
	fcm_dialog.cpp
	fcm_notification.cpp
        fcm_plugin.h
        fcm_dialog.h
        fcm_notification.h
        )

ecm_create_qm_loader( FlashCookieManager_SRCS falkon_flashcookiemanager_qt )

set( FlashCookieManager_UIS
	fcm_dialog.ui
	fcm_notification.ui
	)
qt_wrap_ui(UIS ${FlashCookieManager_UIS})

set( FlashCookieManager_RSCS
	flashcookiemanager.qrc
	)
qt_add_resources(RSCS ${FlashCookieManager_RSCS})

add_library(FlashCookieManager MODULE ${FlashCookieManager_SRCS} ${UIS} ${RSCS})
install(TARGETS FlashCookieManager DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(FlashCookieManager FalkonPrivate)

