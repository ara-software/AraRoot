Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})


include_directories( ${INCLUDE_DIRECTORIES} )

add_executable(makeIcrrEventTree makeIcrrEventTree.cxx)

target_link_libraries(makeIcrrEventTree AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeIcrrHkTree makeIcrrHkTree.cxx)

target_link_libraries(makeIcrrHkTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeIcrrCalibratedEventTree makeIcrrCalibratedEventTree.cxx)

target_link_libraries(makeIcrrCalibratedEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeIcrrEventTreeFromAraSim makeIcrrEventTreeFromAraSim.cxx)

target_link_libraries(makeIcrrEventTreeFromAraSim AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


#Install the binaries
install(TARGETS makeIcrrEventTree makeIcrrHkTree makeIcrrCalibratedEventTree makeIcrrEventTreeFromAraSim DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

#install the scripts
install(FILES runAraOneEventFileMaker.sh runAraOneRunFileMaker.sh runAraOneTimeFileMaker.sh runTestBedEventFileMaker.sh runTestBedRunFileMaker.sh runTestBedTimeFileMaker.sh  DESTINATION ${ARAROOT_INSTALL_PATH}/scripts)

