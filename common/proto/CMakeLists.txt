set(PROTO_FILES
    framing.proto
    echo.proto
    chat.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES}) # result vars visible in parent dir
message ("Proto sources to be generated:")
message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")
#TODO? change destination dir (file copy ?) - to include/acs/proto and proto/src in binary dir
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(GenerateProto
    DEPENDS ${PROTO_SRCS} ${PROTO_HDRS}
)

# Note
# The protobuf_generate_cpp and protobuf_generate_python functions and add_executable() or add_library() calls only work properly within the same directory.

#TODO try OBJECT library
add_library(proto ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(proto INTERFACE ${PROTO_GEN_DIR})

message (DEBUG "proto gen files dir: ${PROTO_GEN_DIR}")
#target_sources(proto INTERFACE ${PROTO_SRCS} ${PROTO_HDRS})
#TODO? target_link_libraries(proto INTERFACE Threads::Threads)

add_dependencies(proto GenerateProto)
