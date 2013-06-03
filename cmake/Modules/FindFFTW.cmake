# - Find FFTW
# Find the native FFTW includes and library
#
# FFTW_INCLUDES - where to find fftw3.h
# FFTW_LIBRARIES - List of libraries when using FFTW.
# FFTW_FOUND - True if FFTW found.

if (FFTW_INCLUDES)
  # Already in cache, be silent
  set (FFTW_FIND_QUIETLY TRUE)
endif (FFTW_INCLUDES)

find_path (FFTW_INCLUDES fftw3.h HINTS ${FFTW3_HINT_INCLUDES})

find_library (FFTW_LIBRARIES NAMES fftw3 HINTS ${FFTW3_HINT_LIBRARIES})

# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (FFTW DEFAULT_MSG FFTW_LIBRARIES
  FFTW_INCLUDES)

# Create the FFTW target if it doesn't exist.
if (NOT TARGET FFTW)
  add_library(FFTW UNKNOWN IMPORTED)
  set_property(TARGET FFTW PROPERTY IMPORTED_LOCATION ${FFTW_LIBRARIES})
endif (NOT TARGET FFTW)

mark_as_advanced (FFTW_LIBRARIES FFTW_INCLUDES)