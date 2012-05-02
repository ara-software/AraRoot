Set(libname AraKvp)

add_library(AraKvp SHARED keyValuePair.c)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
