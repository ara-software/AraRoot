set(libname AraWebPlotter)



set(EXCLUDE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/getDirFromConfig.cxx ${CMAKE_CURRENT_SOURCE_DIR}/AraTimeWebPlotter.cxx ${CMAKE_CURRENT_SOURCE_DIR}/AraWebPlotter.cxx)

#append the include directories to include AraEvent and AraCorrelator

set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS})



set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator)

include(AraRootCMakeLists)


#now include the libraries that we are going to make inside the AraWebPlotter

add_subdirectory(kvpLib)
add_subdirectory(configLib)

MESSAGE("----------Include directories are ${INCLUDE_DIRECTORIES}---------------")

target_link_libraries(AraWebPlotter ${LIBROOTFFTWWRAPPER_LIBRARIES}  AraEvent AraCorrelator AraKvp AraConfig AraDisplay ${ROOT_GUI_LIBRARIES} )


add_executable(AraWebPlotterBin AraWebPlotter.cxx)

target_link_libraries(AraWebPlotterBin AraWebPlotter AraEvent AraCorrelator ${LIBROOTFFTWWRAPPER_LIBRARIES} AraKvp AraConfig AraDisplay)
