set(falkon_benchmarks_SRCS )

qt_add_resources(falkon_benchmarks_SRCS benchmarks.qrc)

macro(falkon_benchmarks)
    foreach(_benchmarkname ${ARGN})
        add_executable(${_benchmarkname} ${_benchmarkname}.cpp ${falkon_benchmarks_SRCS})
        target_link_libraries(${_benchmarkname} Qt::Test FalkonPrivate)
        #add_test(NAME falkon-${_benchmarkname} COMMAND ${_benchmarkname})
        #ecm_mark_as_test(${_benchmarkname})
        #set_tests_properties(falkon-${_benchmarkname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_benchmarkname)
endmacro()

falkon_benchmarks(
    #adblockmatchrule
    adblockparserule
)
