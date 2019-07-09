#TODO? include_directories

#TODO PARENT_SCOPE for set ?
set(PROTO_FILES
    proto/framing.proto
    proto/chat.proto
    )
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES}) # result vars visible in parent dir
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
#TODO change destination dir (how?) -- common/proto/
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})
#set(PROTO_GEN_DIR ${CMAKE/PROJECT_BINARY_DIR})

# common
set(COMMON_SRC
    src/util/Logger.cpp
    include/acs/util/Logger.hpp
    include/acs/util/Identity.hpp
    src/proto/Protocol.cpp
    include/acs/proto/Protocol.hpp
    #src/conn/AsyncWriter.cpp
    include/acs/conn/AsyncWriter.hpp
    include/acs/conn/AsyncReader.hpp
    include/acs/message/EchoHandler.hpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    )
add_library(common ${COMMON_SRC})
#TODO link protobuf::libprotobuf-lite (after dependency refactor)
target_include_directories(common
    PUBLIC
        include
        ${PROTO_GEN_DIR} #TEMP until dependency refactor
    PRIVATE
        ${Asio_INCLUDE_DIRS}
        # ${PROTO_GEN_DIR}
        #${Protobuf_INCLUDE_DIRS}
    )
#TODO add_dependencies(common GenerateProto)

if(BUILD_TESTING)
    add_subdirectory(tests)
    # EXCLUDE_FROM_ALL
    # but: excluded from IDE project files
endif()
