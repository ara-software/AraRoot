Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/utilities/Atri ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} )

include_directories( ${INCLUDE_DIRECTORIES} )


add_executable(makeAtriSensorHkTree makeAtriSensorHkTree.cxx)

target_link_libraries(makeAtriSensorHkTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})



add_executable(makeAtriEventHkTree makeAtriEventHkTree.cxx)

target_link_libraries(makeAtriEventHkTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})



add_executable(makeSimpleAtriEventTree makeSimpleAtriEventTree.cxx)

target_link_libraries(makeSimpleAtriEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


add_executable(makeAtriEventTree makeAtriEventTree.cxx)

target_link_libraries(makeAtriEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})



add_executable(makeAtriEventTreeForcedStationId makeAtriEventTreeForceStationId.cxx)

target_link_libraries(makeAtriEventTreeForcedStationId AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})



add_executable(makeAtriEventTreeStation1 makeAtriEventTreeStation1.cxx)
target_link_libraries(makeAtriEventTreeStation1 AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


add_executable(makeAtriEventTreeStation3 makeAtriEventTreeStation3.cxx)
target_link_libraries(makeAtriEventTreeStation3 AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


add_executable(makeAtriCalibratedEventTree makeAtriCalibratedEventTree.cxx)
target_link_libraries(makeAtriCalibratedEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


add_executable(quickL1EventFilter quickL1EventFilter.cxx fileWriterUtil.c)
target_link_libraries(quickL1EventFilter AraEvent)

add_executable(quickOneInTenFilter quickOneInTenFilter.cxx fileWriterUtil.c)
target_link_libraries(quickOneInTenFilter AraEvent)

add_executable(quickL1CalpulserFilter quickL1CalpulserFilter.cxx fileWriterUtil.c)
target_link_libraries(quickL1CalpulserFilter AraEvent)


#install the binaries
install(TARGETS makeAtriSensorHkTree makeAtriEventHkTree makeSimpleAtriEventTree  makeAtriEventTree makeAtriCalibratedEventTree makeAtriEventTreeForcedStationId makeAtriEventTreeStation1 makeAtriEventTreeStation3  quickL1EventFilter quickOneInTenFilter quickL1CalpulserFilter DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

#install the scripts
install(FILES runAtriRunFileMaker.sh runAtriRunFileMakerForcedStationId.sh runQuickL1Filter.sh runQuickOneInTenFilter.sh DESTINATION ${ARAROOT_INSTALL_PATH}/scripts)
