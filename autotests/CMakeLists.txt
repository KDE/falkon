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
