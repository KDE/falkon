include(ECMMarkAsTest)

set(falkon_autotests_SRCS )

qt5_add_resources(falkon_autotests_SRCS autotests.qrc)

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
    webviewtest
)

set(falkon_autotests_SRCS ${CMAKE_SOURCE_DIR}/tests/modeltest/modeltest.cpp)
include_directories(${CMAKE_SOURCE_DIR}/tests/modeltest)
falkon_tests(
    tabmodeltest
)

set(falkon_autotests_SRCS passwordbackendtest.cpp)
falkon_tests(
    databasepasswordbackendtest
    databaseencryptedpasswordbackendtest
)
