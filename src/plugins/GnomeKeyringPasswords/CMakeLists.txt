set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNOME_KEYRING_CFLAGS}")

set( GnomeKeyringPasswords_SRCS
	gnomekeyringplugin.cpp
	gnomekeyringpasswordbackend.cpp
	)

set( GnomeKeyringPasswords_RSCS
	gnomekeyringpasswords.qrc
	)
qt5_add_resources(RSCS ${GnomeKeyringPasswords_RSCS})

add_library(GnomeKeyringPasswords MODULE ${GnomeKeyringPasswords_SRCS} ${RSCS})
install(TARGETS GnomeKeyringPasswords DESTINATION ${KDE_INSTALL_PLUGINDIR})
target_link_libraries(GnomeKeyringPasswords ${GNOME_KEYRING_LIBRARIES} FalkonPrivate)

