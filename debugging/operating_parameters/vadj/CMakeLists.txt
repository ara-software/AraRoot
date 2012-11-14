Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )
add_executable(estimate_sampling_speed_oneRun estimate_sampling_speed_oneRun.cxx)
target_link_libraries(estimate_sampling_speed_oneRun AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(estimate_sampling_speed_runList estimate_sampling_speed_runList.cxx)
target_link_libraries(estimate_sampling_speed_runList AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

install(TARGETS estimate_sampling_speed_oneRun estimate_sampling_speed_runList DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

