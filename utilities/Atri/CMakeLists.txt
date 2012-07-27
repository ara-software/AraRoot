Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})


include_directories( ${INCLUDE_DIRECTORIES} )

add_executable(makeAtriSensorHkTree makeAtriSensorHkTree.cxx)

target_link_libraries(makeAtriSensorHkTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeAtriEventHkTree makeAtriEventHkTree.cxx)

target_link_libraries(makeAtriEventHkTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeSimpleAtriEventTree makeSimpleAtriEventTree.cxx)

target_link_libraries(makeSimpleAtriEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(makeAtriEventTree makeAtriEventTree.cxx)

target_link_libraries(makeAtriEventTree AraEvent  ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

#install the binaries
install(TARGETS makeAtriSensorHkTree makeAtriEventHkTree makeSimpleAtriEventTree makeAtriEventTree DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

#install the scripts
install(FILES runAtriRunFileMaker.sh DESTINATION ${ARAROOT_INSTALL_PATH}/scripts)
