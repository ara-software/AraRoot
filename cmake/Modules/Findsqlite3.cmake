# - Find SQLITE3
# Find the native SQLITE3 includes and library
#
# SQLITE3_INCLUDES - where to find fftw3.h
# SQLITE3_LIBRARIES - List of libraries when using FFTW.
# SQLITE3_FOUND - True if FFTW found.

if (SQLITE3_INCLUDES)
  # Already in cache, be silent
  set (SQLITE3_FIND_QUIETLY TRUE)
endif (SQLITE3_INCLUDES)

find_path (SQLITE3_INCLUDES sqlite3.h)

find_library (SQLITE3_LIBRARIES NAMES sqlite3)

# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (SQLITE3 DEFAULT_MSG SQLITE3_LIBRARIES
  SQLITE3_INCLUDES)

mark_as_advanced (SQLITE3_LIBRARIES SQLITE3_INCLUDES)

# create sqlite3 target
add_library(sqlite3 UNKNOWN IMPORTED)
set_property(TARGET sqlite3 PROPERTY IMPORTED_LOCATION "${SQLITE3_LIBRARIES}")
