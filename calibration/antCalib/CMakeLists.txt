Set(INCLUDE_DIRECTORIES  ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )

add_executable(generateDeltaTs generateDeltaTs.cxx)
target_link_libraries(generateDeltaTs AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


#install the binaries
install(TARGETS generateDeltaTs DESTINATION ${ARAROOT_INSTALL_PATH}/bin)


