#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "IPI::ipi" for configuration ""
set_property(TARGET IPI::ipi APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(IPI::ipi PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libipi.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS IPI::ipi )
list(APPEND _IMPORT_CHECK_FILES_FOR_IPI::ipi "${_IMPORT_PREFIX}/lib/libipi.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
