Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})


include_directories( ${INCLUDE_DIRECTORIES} )

add_executable(makeAtriSensorHkTree makeAraSensorHkTree.cxx)

target_link_libraries(makeAtriSensorHkTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeAtriEventHkTree makeAraEventHkTree.cxx)

target_link_libraries(makeAtriEventHkTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeSimpleAtriEventTree makeSimpleAraEventTree.cxx)

target_link_libraries(makeSimpleAtriEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeAtriEventTree makeAraEventTree.cxx)

target_link_libraries(makeAtriEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

install(TARGETS makeAtriSensorHkTree makeAtriEventHkTree makeSimpleAtriEventTree makeAtriEventTree DESTINATION ${ARAEVENT_INSTALL_PATH}/bin)

install(FILES runAraRunFileMaker.sh DESTINATION ${ARAEVENT_INSTALL_PATH}/scripts)
