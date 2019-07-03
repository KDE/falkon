if (HOGWEED_LIBRARY)
  set(HOGWEED_FIND_QUIETLY TRUE)
endif (HOGWEED_LIBRARY)

find_library(HOGWEED_LIBRARY hogweed
            PATHS $ENV{HOGWEEDDIR} ${LIB_INSTALL_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HOGWEED DEFAULT_MSG 
                                HOGWEED_LIBRARY)
mark_as_advanced(HOGWEED_LIBRARY)
