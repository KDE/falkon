include(ECMMarkAsTest)

set(falkon_autotests_SRCS )

qt_add_resources(falkon_autotests_SRCS autotests.qrc)

macro(falkon_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp ${_testname}.h ${falkon_autotests_SRCS})
        target_link_libraries(${_testname} Qt::Test FalkonPrivate)
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
    webtabtest
    sqldatabasetest
    sitesettingstest
)

set(falkon_autotests_SRCS ${CMAKE_SOURCE_DIR}/tests/modeltest/modeltest.cpp)
include_directories(${CMAKE_SOURCE_DIR}/tests/modeltest)
falkon_tests(
    tabmodeltest
)

set(falkon_autotests_SRCS passwordbackendtest.cpp)
include_directories(${OPENSSL_INCLUDE_DIR})
falkon_tests(
    databasepasswordbackendtest
    databaseencryptedpasswordbackendtest
)

set(falkon_autotests_SRCS
    qml/qmltestitem.cpp
    qml/qmltesthelper.cpp
)

macro(falkon_qml_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} qml/${_testname}.cpp ${falkon_autotests_SRCS})
        target_link_libraries(${_testname} Qt::Test FalkonPrivate)
        add_test(NAME falkon-qml-${_testname} COMMAND ${_testname})
        ecm_mark_as_test(${_testname})
        set_tests_properties(falkon-qml-${_testname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_testname)
endmacro()

falkon_qml_tests(
    qmlbookmarksapitest
    qmltopsitesapitest
    qmlhistoryapitest
    qmlcookiesapitest
    qmlclipboardapitest
    qmltabsapitest
    qmlwindowsapitest
    qmluserscriptapitest
)
