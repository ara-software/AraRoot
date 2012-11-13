Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )
add_executable(makeSampleLineHistos makeSampleLineHistos.cxx)
target_link_libraries(makeSampleLineHistos AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

install(TARGETS makeSampleLineHistos DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

