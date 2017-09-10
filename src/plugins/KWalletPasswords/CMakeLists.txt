set( KWalletPasswords_SRCS
	kwalletplugin.cpp
	kwalletpasswordbackend.cpp
	)

set( KWalletPasswords_RSCS
	kwalletpasswords.qrc
	)
qt5_add_resources(RSCS ${KWalletPasswords_RSCS})

add_library(KWalletPasswords MODULE ${KWalletPasswords_SRCS} ${RSCS})
install(TARGETS KWalletPasswords DESTINATION ${KDE_INSTALL_PLUGINDIR})
target_link_libraries(KWalletPasswords KF5::KWallet)

