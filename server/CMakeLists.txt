get_target_property(COMMON_INCLUDE common INTERFACE_INCLUDE_DIRECTORIES)

set(SERVER_SRC
    src/main.cpp
    src/conn/AsyncTcpServer.cpp
    include/acs/conn/AsyncTcpServer.hpp
    include/acs/conn/ConnectionManager.hpp
    include/acs/conn/ConnectionStateListener.hpp
    src/conn/TcpConnection.cpp
    include/acs/conn/TcpConnection.hpp
    include/acs/logic/ClientHandler.hpp
    src/echo/EchoClientHandler.cpp
    include/acs/echo/EchoClientHandler.hpp
    src/debug/TestClientHandler.cpp
    include/acs/debug/TestClientHandler.hpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    )

add_executable(server ${SERVER_SRC})

target_include_directories(server
    PRIVATE
        include
        ${COMMON_INCLUDE}
        ${Asio_INCLUDE_DIR}
        ${PROTO_GEN_DIR}
    )

target_link_libraries(server PRIVATE common protobuf::libprotobuf-lite Threads::Threads)

add_dependencies(server GenerateProto)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
