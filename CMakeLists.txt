cmake_minimum_required (VERSION 3.5)
project (AsioTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS -DASIO_STANDALONE)

set(commonPrefix common/src/main)
set(clientPrefix client/src/main)
set(serverPrefix server/src/main)

include_directories(/home/przemek/Dirs/libs/asio-1.12.2/include)
include_directories(
    ${commonPrefix}
)

# protobuf
# find the protobuf compiler and libraries
find_package(Protobuf REQUIRED)
# check if protobuf was found
if(PROTOBUF_FOUND)
    message ("Protobuf found")
else()
    message(FATAL_ERROR "Cannot find Protobuf")
endif()

# # protobufTest
# # generate the .h and .cc files
# PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS protobufTest/AddressBook.proto)
# # print path to generated files
# message ("PROTO_SRCS = ${PROTO_SRCS}")
# message ("PROTO_HDRS = ${PROTO_HDRS}")
# # add a test executable
# add_executable(protobuf_example
#     protobufTest/main.cpp
#     ${PROTO_SRCS}
#     ${PROTO_HDRS}
# )
# target_include_directories(protobuf_example
#     PUBLIC
#     ${PROTOBUF_INCLUDE_DIRS}
#     ${CMAKE_CURRENT_BINARY_DIR}
# )
# # link the exe against the Protobuf libraries
# target_link_libraries(protobuf_example
#     PUBLIC
#     ${PROTOBUF_LIBRARIES}
# )

# tests:common
set(testSources
    common/src/test/TimerTest.cpp
    common/src/test/util/SimpleLoggerTest.cpp
)
add_executable(tests ${testSources})
target_link_libraries(tests gmock_main pthread)

# common
set(protoFiles
    common/src/main/conn/protocol/chat.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${protoFiles})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# client
set(clientSources
    ${clientPrefix}/main.cpp
    ${clientPrefix}/conn/tcp/SyncTcpClient.cpp
    ${PROTO_SRCS}
)
#target_sources()
add_executable(csClient ${clientSources})
target_include_directories(csClient PRIVATE ${clientPrefix})
target_link_libraries(csClient pthread ${PROTOBUF_LIBRARIES})

# server
set(serverSources
    ${serverPrefix}/main.cpp
    ${serverPrefix}/conn/tcp/AsyncTcpServer.cpp
    ${PROTO_SRCS} ${PROTO_HDRS}
)
#target_sources()
add_executable(csServer ${serverSources})
target_include_directories(csServer PRIVATE ${serverPrefix})
# target_include_directories(csServer
#     PUBLIC
#     ${PROTOBUF_INCLUDE_DIRS}
#     ${CMAKE_CURRENT_BINARY_DIR}
# )
target_link_libraries(csServer pthread ${PROTOBUF_LIBRARIES})

#TODO subdirectories, building libraries - common, client, server
