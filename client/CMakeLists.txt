get_target_property(COMMON_INCLUDE common INTERFACE_INCLUDE_DIRECTORIES)

set(CLIENT_SRCS
    src/main.cpp
    src/conn/AsyncTcpClient.cpp
    src/cmd/AsyncCommandLoop.cpp
    src/cmd/CommandDispatcher.cpp
    src/echo/EchoCommandHandler.cpp   
    ${PROTO_SRCS}
    )
set(CLIENT_HDRS
    include/acs/conn/AsyncTcpClient.hpp
    include/acs/cmd/AsyncCommandLoop.hpp
    include/acs/cmd/CommandDispatcher.hpp
    include/acs/cmd/CommandHandler.hpp
    include/acs/echo/EchoCommandHandler.hpp
    )

add_executable(client ${CLIENT_SRCS} ${CLIENT_HDRS})

target_include_directories(client
    PRIVATE
        include
        #TODO common/include
        ${COMMON_INCLUDE}
        ${Asio_INCLUDE_DIRS}
        ${PROTO_GEN_DIR}
    )

target_link_libraries(client PRIVATE common protobuf::libprotobuf-lite Threads::Threads)

add_dependencies(client GenerateProto)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
