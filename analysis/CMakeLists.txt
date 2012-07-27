Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )
add_executable(exampleLoop exampleLoop.cxx)
target_link_libraries(exampleLoop AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

#install the binaries
install(TARGETS exampleLoop DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

##JPD --- This is an example for adding your own bins
# add_executable(exampleLoop exampleLoop.cxx)
# target_link_libraries(exampleLoop AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

#install the binaries
# install(TARGETS exampleLoop DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

