set(autotests_EXTRA_LIBS )

if (KF5Wallet_FOUND)
    add_definitions(-DHAVE_KDE_PASSWORDS_PLUGIN)
    add_library(KWalletPasswordsPluginLib UNKNOWN IMPORTED)
    set_property(TARGET KWalletPasswordsPluginLib PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/bin/plugins/KWalletPasswords.so)
    set(autotests_EXTRA_LIBS ${autotests_EXTRA_LIBS} KWalletPasswordsPluginLib)
endif()

if (GNOME_KEYRING_FOUND)
    add_definitions(-DHAVE_GNOME_PASSWORDS_PLUGIN)
    add_library(GnomeKeyringPasswordsPluginLib UNKNOWN IMPORTED)
    set_property(TARGET GnomeKeyringPasswordsPluginLib PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/bin/plugins/GnomeKeyringPasswords.so)
    set(autotests_EXTRA_LIBS ${autotests_EXTRA_LIBS} GnomeKeyringPasswordsPluginLib)
endif()

set( autotests_SRCS
	qztoolstest.cpp
	main.cpp
	cookiestest.cpp
	adblocktest.cpp
	updatertest.cpp
	passwordbackendtest.cpp
	)

add_executable(autotests ${autotests_SRCS})
target_link_libraries(autotests FalkonPrivate Qt5::Test ${autotests_EXTRA_LIBS})
