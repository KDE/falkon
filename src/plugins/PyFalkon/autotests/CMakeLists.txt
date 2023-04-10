macro(pyfalkon_tests)
    foreach(_testname ${ARGN})
        add_test(NAME pyfalkon-${_testname} COMMAND pyfalkontestrunner ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.py)
        set_tests_properties(pyfalkon-${_testname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_testname)
endmacro()

add_executable(pyfalkontestrunner pyfalkontestrunner.cpp)
target_link_libraries(pyfalkontestrunner FalkonPrivate Qt${QT_MAJOR_VERSION}::Test)

pyfalkon_tests(
    basictest
)
