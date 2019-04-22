add_definitions(-DTRANSLATION_DOMAIN="falkon_kdeframeworksintegration")

set(KDEFrameworksIntegration_SRCS
    kdeframeworksintegrationplugin.cpp
    kwalletpasswordbackend.cpp
    kioschemehandler.cpp
)

set(KDEFrameworksIntegration_RSCS
    kdeframeworksintegration.qrc
)
qt5_add_resources(RSCS ${KDEFrameworksIntegration_RSCS})

add_library(KDEFrameworksIntegration MODULE ${KDEFrameworksIntegration_SRCS} ${RSCS})
install(TARGETS KDEFrameworksIntegration DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(KDEFrameworksIntegration
    FalkonPrivate
    KF5::Wallet
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::Crash
    KF5::CoreAddons
    KF5::PurposeWidgets
)
