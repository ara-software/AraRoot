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

add_executable(eventRate eventRate.cxx)
target_link_libraries(eventRate AraEvent ${ROOT_LIBRARIES})


#install the binaries
install(TARGETS eventRate avWaveformCalPulser avWaveformICLPulser deltaTPulses maxAmplitude exampleLoop DESTINATION ${ARAROOT_INSTALL_PATH}/bin)


##JPD --- This is an example for adding your own bins
# add_executable(exampleLoop exampleLoop.cxx)
# target_link_libraries(exampleLoop AraEvent ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

#install the binaries
# install(TARGETS exampleLoop DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

