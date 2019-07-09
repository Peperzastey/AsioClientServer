# - Try to find the Asio library.
# This project does not use FindBoost.cmake because it uses the standalone version of Asio
# instead of the Boost one.
#
# You can provide a path to check in the ASIO_INCLUDE environment variable.
#
# The following variables are set after configuration is complete:
#  ``Asio_FOUND``
#  Found the Asio header-only library (header files).
#  ``Asio_VERSION``
#  Version of package found (in the form "major.minor.subminor").
#  ``Asio_INCLUDE_DIRS``
#  Include directories for Asio.
#  ``Asio_INCLUDE_DIR``
#  Include directory for Asio (cache variable).

find_path(Asio_INCLUDE_DIR
    asio.hpp
    HINTS ENV ASIO_INCLUDE
    #PATHS /usr/local/
    PATH_SUFFIXES include
    DOC "Asio include directory"
)
mark_as_advanced(Asio_INCLUDE_DIR)

if(Asio_INCLUDE_DIR)
    # not sure whether find_path always returns absolute paths
    get_filename_component(_ASIO_INC_ABS_PATH ${Asio_INCLUDE_DIR} ABSOLUTE CACHE)
    # behavior is well-defined only for full paths
    if(NOT EXISTS ${_ASIO_INC_ABS_PATH}/asio/ip)
        message (WARNING "asio/ip include subdirectory not found")
    elseif(NOT EXISTS ${_ASIO_INC_ABS_PATH}/asio/posix)
        message (WARNING "asio/posix include subdirectory not found")
    elseif(NOT EXISTS ${_ASIO_INC_ABS_PATH}/asio/ssl)
        message (WARNING "asio/ssl include subdirectory not found")
    endif()

    if(NOT EXISTS ${_ASIO_INC_ABS_PATH}/asio/version.hpp)
        message (WARNING "Asio version header file (asio/version.hpp) not found")
    else()
        # find Asio version
        #TODO? cache entries instead of normal variables
        file(STRINGS 
            ${_ASIO_INC_ABS_PATH}/asio/version.hpp
            _ASIO_VERSION_DEFINE_STRING
            REGEX "#define[ \t]+ASIO_VERSION[ \t]+[0-9]+"
        )
        if(_ASIO_VERSION_DEFINE_STRING MATCHES "#define[ \t]+ASIO_VERSION[ \t]+([0-9]+)")
            set(_ASIO_VERSION_DEFINE "${CMAKE_MATCH_1}")
        endif()
        unset(_ASIO_VERSION_DEFINE_STRING)

        math(EXPR _ASIO_MAJOR_VERSION "${_ASIO_VERSION_DEFINE} / 100000")
        math(EXPR _ASIO_MINOR_VERSION "${_ASIO_VERSION_DEFINE} / 100 % 1000")
        math(EXPR _ASIO_SUBMINOR_VERSION "${_ASIO_VERSION_DEFINE} % 100")

        set(Asio_VERSION "${_ASIO_MAJOR_VERSION}.${_ASIO_MINOR_VERSION}.${_ASIO_SUBMINOR_VERSION}")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Asio
    REQUIRED_VARS Asio_INCLUDE_DIR
    VERSION_VAR Asio_VERSION
)

#if(Asio_FOUND)
set(Asio_INCLUDE_DIRS ${Asio_INCLUDE_DIR})
#endif()

# Define upper case versions of output variables
foreach(Camel
    Asio_INCLUDE_DIRS
    Asio_INCLUDE_DIR
    Asio_VERSION
    )
    string(TOUPPER ${Camel} UPPER)
    set(${UPPER} ${${Camel}})
endforeach()

# Add asio::standalone imported target
add_library(asio::standalone INTERFACE IMPORTED GLOBAL) #TODO try without GLOBAL
set_target_properties(asio::standalone PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE"
    INTERFACE_INCLUDE_DIRECTORIES "${Asio_INCLUDE_DIR}"
)
#TODO target_link_libraries(asio::standalone INTERFACE Threads::Threads)
#OR SHOULD set_target_properties() INTERFACE_LINK_LIBRARIES ?
