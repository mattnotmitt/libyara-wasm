find_package(PkgConfig)
pkg_check_modules(PC_Yara QUIET Yara)

find_path(Yara_INCLUDE_DIR
  NAMES yara.h
  PATHS ${PC_Yara_INCLUDE_DIRS}
  PATH_SUFFIXES yara
)
find_library(Yara_LIBRARY
  NAMES yara
  PATHS ${PC_Yara_LIBRARY_DIRS}
)

set(Yara_VERSION ${PC_Yara_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Yara
  FOUND_VAR Yara_FOUND
  REQUIRED_VARS
    Yara_LIBRARY
    Yara_INCLUDE_DIR
  VERSION_VAR Yara_VERSION
)

if(Yara_FOUND)
  set(Yara_LIBRARIES ${Yara_LIBRARY})
  set(Yara_INCLUDE_DIRS ${Yara_INCLUDE_DIR})
  set(Yara_DEFINITIONS ${PC_Yara_CFLAGS_OTHER})
endif()

if(Yara_FOUND AND NOT TARGET Yara::Yara)
  add_library(Yara::Yara UNKNOWN IMPORTED)
  set_target_properties(Yara::Yara PROPERTIES
    IMPORTED_LOCATION "${Yara_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${PC_Yara_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${Yara_INCLUDE_DIR}"
  )
endif()