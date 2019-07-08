cmake_minimum_required (VERSION 3.9...3.14)
project (AsioClientServer
    VERSION 0.1
    DESCRIPTION "Tcp client-server program using Asio and Protobuf."
    LANGUAGES CXX
)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

# thread library
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads) # REQUIRED ?

# find Asio headers
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
find_package(Asio REQUIRED)
if(Asio_FOUND)
    message ("Asio include dir: ${Asio_INCLUDE_DIR}")
endif()
# use standalone Asio (without Boost dependencies)
#TODO set in FindAsio.cmake / define macro in config file (beside optional debug macros)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE")
# set(Asio_INCLUDE_DIR /home/przemek/Dirs/libs/asio-1.12.2/include)

# find the protobuf compiler and libraries
find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message ("Protobuf include dir: ${Protobuf_INCLUDE_DIR}")   
endif()

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)

if(BUILD_TESTING)
    # TODO find GTest, GMock
#    find_package(GTest)

    # see subdirs:
    # common/tests
    # server/tests
    # client/tests

    #Single test executable:
    # file(WRITE dummy.cpp "")
    # add_executable(tests dummy.cpp)
    # target_link_libraries(tests
    #     PRIVATE
    #     gmock_main
    #     commonTestsLib
    #     serverTestsLib
    #     clientTestsLib
    #     Threads::Threads
    #     )

    if(NOT TARGET commonTests)
        add_custom_target(commonTests)  # empty
        #TODO ALL?
    endif()
    if(NOT TARGET serverTests)
        add_custom_target(serverTests)  # empty
        #TODO ALL?
    endif()
    if(NOT TARGET clientTests)
        add_custom_target(clientTests)  # empty
        #TODO ALL?
    endif()

    add_custom_target(AllTests ALL)
    add_dependencies(AllTests
        commonTests
        serverTests
        clientTests
    )
    add_custom_command(TARGET AllTests
        COMMENT "Run tests"
        POST_BUILD
        COMMAND ctest #-C $<CONFIGURATION> --output-on-failure
        # WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
    message ("Building of tests is disabled")
endif()
