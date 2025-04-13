#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "daxa::daxa" for configuration "Release"
set_property(TARGET daxa::daxa APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(daxa::daxa PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/daxa.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "fmt::fmt"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/daxa.dll"
  )

list(APPEND _cmake_import_check_targets daxa::daxa )
list(APPEND _cmake_import_check_files_for_daxa::daxa "${_IMPORT_PREFIX}/lib/daxa.lib" "${_IMPORT_PREFIX}/bin/daxa.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
