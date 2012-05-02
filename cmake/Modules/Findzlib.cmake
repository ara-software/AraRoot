# - Find ZLIB
# Find the native ZLIB includes and library
#
# ZLIB_INCLUDES - where to find zlib.h
# ZLIB_LIBRARIES - List of libraries when using ZLIB.
# ZLIB_FOUND - True if ZLIB found.

if (ZLIB_INCLUDES)
  # Already in cache, be silent
  set (ZLIB_FIND_QUIETLY TRUE)
endif (ZLIB_INCLUDES)

find_path (ZLIB_INCLUDES zlib.h)

find_library (ZLIB_LIBRARIES NAMES z)

# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (ZLIB DEFAULT_MSG ZLIB_LIBRARIES
  ZLIB_INCLUDES)

mark_as_advanced (ZLIB_LIBRARIES ZLIB_INCLUDES)

# create zlib target
add_library(zlib UNKNOWN IMPORTED)
set_property(TARGET zlib PROPERTY IMPORTED_LOCATION "${ZLIB_LIBRARIES}")
