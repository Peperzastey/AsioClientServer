cmake_minimum_required (VERSION 3.1...3.14)
project (AsioClientServer
    VERSION 0.1
    DESCRIPTION "Tcp client-server program using Asio and Protobuf."
    LANGUAGES CXX
    )

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
# use standalone Asio (without Boost dependencies)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE")

# Asio
set(ASIO_INC_DIR /home/przemek/Dirs/libs/asio-1.12.2/include)

# thread library
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads) # REQUIRED ?

# Protobuf
# find the protobuf compiler and libraries
find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message ("Protobuf found")
else()
    message (FATAL_ERROR "Cannot find Protobuf")
endif()


# common
set(PROTO_FILES
    common/proto/framing.proto
    common/proto/chat.proto
    )
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})
message ("Proto sources to be generated:")
message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")
add_custom_target(GenerateProto
    DEPENDS ${PROTO_SRCS} ${PROTO_HDRS}
    )
# generate_proto_sources script: configure_file() / file(GENERATE)
# add_custom_target(GenerateProto
#     COMMAND ${CMAKE_COMMAND} -D PROTO_FILES=${PROTO_FILES} -P ${PROJECT_SOURCE_DIR}/generate_proto_sources.cmake
#     COMMENT "Generate sources from proto files"
#     )
#TODO change destination dir (how?)
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})

# common
set(COMMON_SRC
    common/src/util/Logger.cpp
    common/include/acs/util/Logger.hpp
    common/include/acs/util/Identity.hpp
    common/src/proto/Protocol.cpp
    common/include/acs/proto/Protocol.hpp
    #common/src/conn/AsyncWriter.cpp
    common/include/acs/conn/AsyncWriter.hpp
    common/include/acs/conn/AsyncReader.hpp
    common/include/acs/message/EchoHandler.hpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    )
add_library(common ${COMMON_SRC})
# link protobuf::libprotobuf ?
target_include_directories(common
    PUBLIC
    common/include
    ${ASIO_INC_DIR}
    ${PROTO_GEN_DIR}
    )

# client
set(CLIENT_SRC
    client/src/main.cpp
    client/src/conn/AsyncTcpClient.cpp
    client/include/acs/conn/AsyncTcpClient.hpp
    client/src/cmd/AsyncCommandLoop.cpp
    client/include/acs/cmd/AsyncCommandLoop.hpp
    client/src/cmd/CommandDispatcher.cpp
    client/include/acs/cmd/CommandDispatcher.hpp
    client/include/acs/cmd/CommandHandler.hpp
    client/src/echo/EchoCommandHandler.cpp
    client/include/acs/echo/EchoCommandHandler.hpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    )
add_executable(client ${CLIENT_SRC})
target_include_directories(client
    PRIVATE
    client/include
    common/include
    ${ASIO_INC_DIR}
    ${PROTO_GEN_DIR}
    )
target_link_libraries(client PRIVATE common protobuf::libprotobuf Threads::Threads)
add_dependencies(client GenerateProto)

# server
set(SERVER_SRC
    server/src/main.cpp
    server/src/conn/AsyncTcpServer.cpp
    server/include/acs/conn/AsyncTcpServer.hpp
    server/include/acs/conn/ConnectionManager.hpp
    server/include/acs/conn/ConnectionStateListener.hpp
    server/src/conn/TcpConnection.cpp
    server/include/acs/conn/TcpConnection.hpp
    server/include/acs/logic/ClientHandler.hpp
    server/src/echo/EchoClientHandler.cpp
    server/include/acs/echo/EchoClientHandler.hpp
    server/src/debug/TestClientHandler.cpp
    server/include/acs/debug/TestClientHandler.hpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    )
add_executable(server ${SERVER_SRC})
target_include_directories(server
    PRIVATE
    server/include
    common/include
    ${ASIO_INC_DIR}
    ${PROTO_GEN_DIR}
    )
target_link_libraries(server PRIVATE common protobuf::libprotobuf Threads::Threads)
add_dependencies(server GenerateProto)

# target_include_directories(csServer
#     PUBLIC
#     ${PROTOBUF_INCLUDE_DIRS}
#     ${CMAKE_CURRENT_BINARY_DIR}
# )

if(BUILD_TESTING)
    # TODO find GTest, GMock

    # # thread library
    # set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    # set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    # find_package(Threads) # REQUIRED ?

    # tests
    set(TESTS_SRC
    common/tests/util/LoggerTest.cpp
    common/include/acs/util/Logger.hpp
    )
    add_executable(tests ${TESTS_SRC})
    target_include_directories(tests
    PRIVATE
    common/include
    )
    target_link_libraries(tests PRIVATE common gmock_main Threads::Threads)
    add_test(NAME CommonTest COMMAND tests)

    # add_custom_target for all the tests? / that DEPENDS on all the test targets?
    add_custom_target(AllTests ALL)
        # DEPENDS tests  - file-level dependency
    add_dependencies(AllTests   # target-level dependencies
        tests #client #server
    )
    add_custom_command(TARGET AllTests
        COMMENT "Run tests"
        POST_BUILD
        COMMAND ctest #-C $<CONFIGURATION> --output-on-failure
        # WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

    # # common tests
    # add_library(tests_common
    #     common/tests/util/SimpleLoggerTest.cpp
    #     common/include/acs/util/SimpleLogger.hpp
    #     )
    # target_include_directories(tests_common
    #     PUBLIC
    #     common/include
    #     )
    # target_link_libraries(tests_common pthread)
    # 
# 
    # 
    # # client tests
    # # server tests
    # 
# 
    # 
    # # tests executable
    # add_executable(tests)
    # target_link_libraries(tests
    #     tests_common gmock_main)

    #TODO 3 test executables or 1 bulk?
    # # tests:common
    # set(testSources
    #     common/src/test/TimerTest.cpp
    #     common/src/test/util/SimpleLoggerTest.cpp
    # )
    # add_executable(tests)
    # target_sources()
    # target_link_libraries(tests gmock_main pthread)
else()
    message ("Building of tests is disabled")
endif()

#TODO target_* commands instead of global
#TODO subdirectories, building libraries - common, client, server
