Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )
add_executable(exampleLoop exampleLoop.cxx)
target_link_libraries(exampleLoop AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(deltaTPulses deltaTPulses.cxx)
target_link_libraries(deltaTPulses AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(maxAmplitude maxAmplitude.cxx)
target_link_libraries(maxAmplitude AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

add_executable(avWaveformCalPulser avWaveformCalPulser.cxx)
target_link_libraries(avWaveformCalPulser AraEvent ${ROOT_LIBRARIES})

add_executable(avWaveformICLPulser avWaveformICLPulser.cxx)
target_link_libraries(avWaveformICLPulser AraEvent ${ROOT_LIBRARIES})


#install the binaries
install(TARGETS  avWaveformCalPulser avWaveformICLPulser deltaTPulses maxAmplitude exampleLoop DESTINATION ${ARAROOT_INSTALL_PATH}/bin)


##JPD --- This is an example for adding your own bins
# add_executable(exampleLoop exampleLoop.cxx)
# target_link_libraries(exampleLoop AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

#install the binaries
# install(TARGETS exampleLoop DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

#getAtriRunStatistics
add_executable(getAtriRunStatistics getAtriRunStatistics.cxx)
target_link_libraries(getAtriRunStatistics AraEvent)

install(TARGETS getAtriRunStatistics  DESTINATION ${ARAROOT_INSTALL_PATH}/bin)
install(FILES  runGetAtriRunStatistics.sh DESTINATION ${ARAROOT_INSTALL_PATH}/scripts)