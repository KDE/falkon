# - Try to find nettle
# Once done this will define
#  NETTLE_FOUND - System has nettle
#  NETTLE_INCLUDE_DIRS - The nettle include directories
#  NETTLE_LIBRARIES - The libraries needed to use nettle
#  NETTLE_DEFINITIONS - Compiler switches required for using nettle

find_package(PkgConfig)
pkg_check_modules(PC_NETTLE QUIET nettle)
set(NETTLE_DEFINITIONS ${PC_NETTLE_CFLAGS_OTHER})

find_path(NETTLE_INCLUDE_DIR NAMES nettle/hkdf.h nettle/hmac.h nettle/sha2.h
            HINTS ${PC_NETTLE_INCLUDEDIR} ${PC_NETTLE_INCLUDE_DIRS}
            PATH_SUFFIXES nettle )

find_library(NETTLE_LIBRARY NAMES nettle libnettle
            HINTS ${PC_NETTLE_LIBDIR} ${PC_NETTLE_LIBRARY_DIRS} )

set(NETTLE_LIBRARIES ${NETTLE_LIBRARY} )
set(NETTLE_INCLUDE_DIRS ${NETTLE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NETTLE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(nettle  DEFAULT_MSG
                                  NETTLE_LIBRARY NETTLE_INCLUDE_DIR)
mark_as_advanced(NETTLE_INCLUDE_DIR NETTLE_LIBRARY )
