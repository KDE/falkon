add_definitions(-DTRANSLATION_DOMAIN="falkon_testplugin")

set( TestPlugin_SRCS
	testplugin.cpp
	testplugin_sidebar.cpp
	)

add_library(TestPlugin MODULE ${TestPlugin_SRCS} ${RSCS})
install(TARGETS TestPlugin DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(TestPlugin FalkonPrivate)
