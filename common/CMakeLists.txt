add_subdirectory(proto)

#TODO? move add_library to src dir
set(COMMON_SRCS
    src/util/Logger.cpp
    src/proto/Protocol.cpp
    #src/conn/AsyncWriter.cpp
)
set(COMMON_HDRS
    include/acs/util/Logger.hpp
    include/acs/util/Identity.hpp
    include/acs/proto/Protocol.hpp
    include/acs/conn/AsyncWriter.hpp
    include/acs/conn/AsyncReader.hpp
    include/acs/message/EchoHandler.hpp
)

add_library(common ${COMMON_SRCS} ${COMMON_HDRS})

target_include_directories(common
    PUBLIC
        include
        #$<TARGET_PROPERTY:proto,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(common
    PUBLIC #TODO PRIVATE (after module refactor)
        proto
        asio::standalone
        protobuf::libprotobuf-lite
        Threads::Threads
)
#TODO 
#TODO needed? add_dependencies(common GenerateProto)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
