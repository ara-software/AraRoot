Set(libname AraKvp)

add_library(AraKvp SHARED keyValuePair.c)


SET_TARGET_PROPERTIES(AraKvp PROPERTIES SUFFIX .so)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

install(TARGETS AraKvp DESTINATION ${ARAROOT_INSTALL_PATH}/lib)
