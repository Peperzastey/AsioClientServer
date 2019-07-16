#TODO add_library(client) in src dir, add_executable(client-exe) with OUTPUT_NAME x here
set(CLIENT_SRCS
    src/main.cpp
    src/conn/AsyncTcpClient.cpp
    src/cmd/AsyncCommandLoop.cpp
    src/cmd/CommandDispatcher.cpp
    src/echo/EchoCommandHandler.cpp  
)
set(CLIENT_HDRS
    include/acs/conn/AsyncTcpClient.hpp
    include/acs/cmd/AsyncCommandLoop.hpp
    include/acs/cmd/CommandDispatcher.hpp
    include/acs/cmd/CommandHandler.hpp
    include/acs/echo/EchoCommandHandler.hpp
    include/acs/message/EchoMessage.hpp
)

add_executable(client ${CLIENT_SRCS} ${CLIENT_HDRS})

target_include_directories(client
    PRIVATE
        include
)

target_link_libraries(client
    PRIVATE
        common
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
