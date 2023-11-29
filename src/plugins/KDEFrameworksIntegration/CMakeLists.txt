set(KDEFrameworksIntegration_SRCS
    kdeframeworksintegrationplugin.cpp
    kwalletpasswordbackend.cpp
    kioschemehandler.cpp
    downloadkjob.cpp
    kdeframeworksintegrationplugin.h
    kwalletpasswordbackend.h
    kioschemehandler.h
    downloadkjob.h
)

ecm_create_qm_loader(KDEFrameworksIntegration_SRCS falkon_kdeframeworksintegration_qt)

set(KDEFrameworksIntegration_RSCS
    kdeframeworksintegration.qrc
)
qt_add_resources(RSCS ${KDEFrameworksIntegration_RSCS})

add_library(KDEFrameworksIntegration MODULE ${KDEFrameworksIntegration_SRCS} ${RSCS})
install(TARGETS KDEFrameworksIntegration DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(KDEFrameworksIntegration
    FalkonPrivate
    KF6::Wallet
    KF6::KIOCore
    KF6::KIOWidgets
    KF6::Crash
    KF6::CoreAddons
    KF6::PurposeWidgets
    KF6::JobWidgets
)
