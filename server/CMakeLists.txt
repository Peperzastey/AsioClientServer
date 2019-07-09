#TODO add_library(server) in src dir, add_executable(server-exe) with OUTPUT_NAME x here
set(SERVER_SRCS
    src/main.cpp
    src/conn/AsyncTcpServer.cpp
    src/conn/TcpConnection.cpp
    src/echo/EchoClientHandler.cpp
    src/debug/TestClientHandler.cpp  
)
set(SERVER_HDRS
    include/acs/conn/AsyncTcpServer.hpp
    include/acs/conn/ConnectionManager.hpp
    include/acs/conn/ConnectionStateListener.hpp
    include/acs/conn/TcpConnection.hpp
    include/acs/logic/ClientHandler.hpp
    include/acs/echo/EchoClientHandler.hpp
    include/acs/debug/TestClientHandler.hpp
)

add_executable(server ${SERVER_SRCS} ${SERVER_HDRS})

target_include_directories(server
    PRIVATE
        include
)

target_link_libraries(server
    PRIVATE
        common
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
