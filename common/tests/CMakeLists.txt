set(TESTS_SRC
    util/LoggerTest.cpp
    #../include/acs/util/Logger.hpp
)

add_executable(commonTests ${TESTS_SRC})
#TODO change OUTPUT to ${CMAKE_BINARY_DIR}/tests/commonTests
#OR create symlink (possible on Windows?)

target_include_directories(commonTests
    PRIVATE
        #../include
        $<TARGET_PROPERTY:common,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(commonTests
    PRIVATE
        gmock_main
        Threads::Threads)

add_test(NAME CommonTests COMMAND commonTests)
