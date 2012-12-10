Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )
add_executable(calibrateATRI firstCalibTry.cxx)
target_link_libraries(calibrateATRI AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


add_executable(calibrateATRISecondTry secondCalibTry.cxx)
target_link_libraries(calibrateATRISecondTry AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(calibrateATRISecondTrySingleChannel secondCalibTrySingleChannel.cxx)
target_link_libraries(calibrateATRISecondTrySingleChannel AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

#install the binaries
install(TARGETS calibrateATRI calibrateATRISecondTry calibrateATRISecondTrySingleChannel DESTINATION ${ARAROOT_INSTALL_PATH}/bin)


