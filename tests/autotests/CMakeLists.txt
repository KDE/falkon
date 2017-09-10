set(autotests_EXTRA_LIBS )

if (KDE_INTEGRATION AND KF5Wallet_FOUND)
    add_definitions(-DHAVE_KDE_PASSWORDS_PLUGIN)
    set(autotests_EXTRA_LIBS ${autotests_EXTRA_LIBS} ${CMAKE_BINARY_DIR}/bin/plugins/libKWalletPasswords.so)
endif()

if (GNOME_INTEGRATION AND GNOME_KEYRING_FOUND)
    add_definitions(-DHAVE_GNOME_PASSWORDS_PLUGIN)
    set(autotests_EXTRA_LIBS ${autotests_EXTRA_LIBS} ${CMAKE_BINARY_DIR}/bin/plugins/libGnomeKeyringPasswords.so)
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
target_link_libraries(autotests FalkonPrivate Qt5::Test)

