set(KDESupport_SRCS
    kdesupportplugin.cpp
    kwalletpasswordbackend.cpp
)

ecm_create_qm_loader(KDESupport_SRCS falkon_kdesupport_qt)

set(KDESupport_RSCS
    kdesupport.qrc
)
qt5_add_resources(RSCS ${KDESupport_RSCS})

add_library(KDESupport MODULE ${KDESupport_SRCS} ${RSCS})
install(TARGETS KDESupport DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(KDESupport FalkonPrivate KF5::Wallet)
