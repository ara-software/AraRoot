Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})


include_directories( ${INCLUDE_DIRECTORIES} )

add_executable(makeIcrrEventTree makeAraEventTree.cxx)

target_link_libraries(makeIcrrEventTree AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeIcrrHkTree makeAraHkTree.cxx)

target_link_libraries(makeIcrrHkTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeIcrrCalibratedEventTree makeAraCalibratedEventTree.cxx)

target_link_libraries(makeIcrrCalibratedEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

#Install the binaries
install(TARGETS makeIcrrEventTree makeIcrrHkTree makeIcrrCalibratedEventTree DESTINATION ${ARAEVENT_INSTALL_PATH}/bin)

#install the scripts
install(FILES runAraOneEventFileMaker.sh runAraOneRunFileMaker.sh runAraOneTimeFileMaker.sh runAraTestBedEventFileMaker.sh runAraTestBedRunFileMaker.sh runAraTestBedTimeFileMaker.sh  DESTINATION ${ARAEVENT_INSTALL_PATH}/scripts)

