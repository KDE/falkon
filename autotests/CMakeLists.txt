include(ECMMarkAsTest)

set(falkon_autotests_SRCS )

macro(falkon_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp ${falkon_autotests_SRCS})
        target_link_libraries(${_testname} Qt5::Test FalkonPrivate)
        add_test(NAME falkon-${_testname} COMMAND ${_testname})
        ecm_mark_as_test(${_testname})
        set_tests_properties(falkon-${_testname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_testname)
endmacro()

falkon_tests(
    qztoolstest
    cookiestest
    adblocktest
    updatertest
    locationbartest
)

set(falkon_autotests_SRCS passwordbackendtest.cpp)
falkon_tests(
    databasepasswordbackendtest
    databaseencryptedpasswordbackendtest
)
