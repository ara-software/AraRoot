Set(libname AraWebPlotter)
# ROOT is included in the template.
Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

File(GLOB ${libname}Headers AraEventPlotter.h     AraHistoHandler.h     AraHkPlotter.h        AraPlotUtils.h        AraTimeHistoHandler.h AraWebPlotterConfig.h
	  )

File(GLOB ${libname}Source AraEventPlotter.cxx     AraHkPlotter.cxx        AraTimeHistoHandler.cxx      AraHistoHandler.cxx     AraPlotUtils.cxx    AraWebPlotterConfig.cxx
	  )

Set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
Set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/G__${libname}.cxx)

Set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES}  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter)

ROOT_GENERATE_DICTIONARY("${${libname}Headers}" 
                    "${LinkDef}" "${Dictionary}" 
                    "${DICTIONARY_INCLUDE_DIRECTORIES}" ""
                   )

include_directories( ${INCLUDE_DIRECTORIES} )

Add_Library(${libname} SHARED ${${libname}Source} ${Dictionary})

SET_TARGET_PROPERTIES(${libname} PROPERTIES SUFFIX .so)

#Now we make sure that we have built the kvpLib and configLib libraries
add_subdirectory(kvpLib)
add_subdirectory(configLib)

target_link_libraries(AraWebPlotter ${LIBROOTFFTWWRAPPER_LIBRARIES}  AraEvent AraCorrelator AraKvp AraConfig AraDisplay ${ROOT_GUI_LIBRARIES})

add_executable(AraWebPlotterBin AraWebPlotter.cxx)

target_link_libraries(AraWebPlotterBin AraWebPlotter AraEvent AraCorrelator ${LIBROOTFFTWWRAPPER_LIBRARIES} AraKvp AraConfig AraDisplay)

add_executable(AraTimeWebPlotterBin AraTimeWebPlotter.cxx)

target_link_libraries(AraTimeWebPlotterBin AraWebPlotter AraEvent AraCorrelator ${LIBROOTFFTWWRAPPER_LIBRARIES} AraKvp AraConfig AraDisplay)

add_executable(getDirFromConfigBin getDirFromConfig.cxx)

target_link_libraries(getDirFromConfigBin AraWebPlotter AraEvent AraCorrelator ${LIBROOTFFTWWRAPPER_LIBRARIES} AraKvp AraConfig AraDisplay)




#install stuff
install(FILES ${${libname}Headers} DESTINATION ${ARAEVENT_INSTALL_PATH}/include)
install(TARGETS AraWebPlotter DESTINATION ${ARAEVENT_INSTALL_PATH}/lib)

#not sure this will work
install(TARGETS getDirFromConfigBin DESTINATION ${ARAEVENT_INSTALL_PATH}/bin)
install(TARGETS AraWebPlotterBin DESTINATION ${ARAEVENT_INSTALL_PATH}/bin)
install(TARGETS AraTimeWebPlotterBin DESTINATION ${ARAEVENT_INSTALL_PATH}/bin)


