if (GMP_INCLUDE_DIR AND GMP_LIBRARY)
  set(GMP_FIND_QUIETLY TRUE)
endif (GMP_INCLUDE_DIR AND GMP_LIBRARY)

find_path(GMP_INCLUDE_DIR NAMES gmp.h 
            PATHS $ENV{GMPDIR} ${INCLUDE_INSTALL_DIR}
)

find_library(GMP_LIBRARY gmp 
            PATHS $ENV{GMPDIR} ${LIB_INSTALL_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG
                                  GMP_INCLUDE_DIR GMP_LIBRARY)
mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY)
