Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )

add_executable(adcSampleBlock adcSampleBlock.cxx)
target_link_libraries(adcSampleBlock AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
install(TARGETS adcSampleBlock DESTINATION ${ARAROOT_INSTALL_PATH}/bin)


add_executable(getADCmVConversion getADCmVConversion.cxx)
target_link_libraries(getADCmVConversion AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
install(TARGETS getADCmVConversion DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

add_executable(getADCmVConversionTest getADCmVConversionTest.cxx)
target_link_libraries(getADCmVConversionTest AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
install(TARGETS getADCmVConversionTest DESTINATION ${ARAROOT_INSTALL_PATH}/bin)


Set(executable3 adcSample)
add_executable(${executable3} ${executable3}.cxx)
target_link_libraries(${executable3} AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
install(TARGETS ${executable3} DESTINATION ${ARAROOT_INSTALL_PATH}/bin)


Set(executable4 getADCmVConversion2)
add_executable(${executable4} ${executable4}.cxx)
target_link_libraries(${executable4} AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
install(TARGETS ${executable4} DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

Set(executable5 adcSampleBlockNoPed)
add_executable(${executable5} ${executable5}.cxx)
target_link_libraries(${executable5} AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
install(TARGETS ${executable5} DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

Set(executable5 calibrationFitter)
add_executable(${executable5} ${executable5}.cxx)
target_link_libraries(${executable5} AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
install(TARGETS ${executable5} DESTINATION ${ARAROOT_INSTALL_PATH}/bin)
