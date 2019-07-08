# - Try to find the Asio library.
# This project does not use FindBoost because it uses the standalone version Asio.
#
# You can provide a path to check in the ASIO_INCLUDE environment variable.
#
# The following variables are set after configuration is complete:
#  Asio_FOUND
#  Asio_INCLUDE_DIR

#TODO add ASIO::ASIO target
#set(ASIO_FOUND FALSE)

find_path(Asio_INCLUDE_DIR
    asio.hpp
    HINTS ENV ASIO_INCLUDE
    #PATHS /usr/local/include/
    # all these subdirectories must exist
    PATH_SUFFIXES asio asio/ip asio/posix asio/ssl #TODO test it
    DOC "Asio include directory"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Asio DEFAULT_MSG
    Asio_INCLUDE_DIR
)
mark_as_advanced(Asio_INCLUDE_DIR)

# Define upper case versions of output variables
foreach(Camel
    Asio_INCLUDE_DIR
    )
    string(TOUPPER ${Camel} UPPER)
    set(${UPPER} ${${Camel}})
endforeach()
