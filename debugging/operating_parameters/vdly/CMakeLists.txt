Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )
add_executable(adc_to_voltage adc_to_voltage.cxx)
target_link_libraries(adc_to_voltage AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(adc_to_voltage_betterStats adc_to_voltage_betterStats.cxx)
target_link_libraries(adc_to_voltage_betterStats AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


install(TARGETS adc_to_voltage adc_to_voltage_betterStats DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

