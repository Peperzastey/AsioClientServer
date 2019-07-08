# project() - if EXCLUDE_FROM_ALL

set(TESTS_SRC
    util/LoggerTest.cpp
    ../include/acs/util/Logger.hpp
    )

add_executable(commonTests ${TESTS_SRC})
#TODO change OUTPUT to ${CMAKE_BINARY_DIR}/tests/commonTests
#OR create symlink (possible on Windows?)

target_include_directories(commonTests
    PRIVATE
    # WARN project() changes source dir
    #${PROJECT_SOURCE_DIR}/common/include  
    # can be useful when add_subdirectory() is used to connect several projects
    ../include
    #${AsioClientServer_SOURCE_DIR}/common/include
    #${CMAKE_SOURCE_DIR}/common/include
    )
target_link_libraries(commonTests PRIVATE gmock_main Threads::Threads)
add_test(NAME CommonTests COMMAND commonTests)
