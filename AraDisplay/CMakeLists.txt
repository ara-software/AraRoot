Set(libname AraDisplay)
# ROOT is included in the template.
Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

File(GLOB ${libname}Headers AraCanvasMaker.h        AraCorrelationFactory.h AraDisplayConventions.h AraTBCanvasMaker.h
AraControlPanel.h       AraDisplay.h            AraFFTGraph.h           AraWaveformGraph.h
	  )

File(GLOB ${libname}Source AraCanvasMaker.cxx        AraControlPanel.cxx       AraCorrelationFactory.cxx AraDisplay.cxx            AraFFTGraph.cxx           AraTBCanvasMaker.cxx      AraWaveformGraph.cxx
	  )

#message(STATUS "${libname}Headers ${${libname}Headers}")
#message(STATUS "${libname}Source ${${libname}Source}")


Set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
Set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/G__${libname}.cxx)

Set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES}  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter)

ROOT_GENERATE_DICTIONARY("${${libname}Headers}" 
                    "${LinkDef}" "${Dictionary}" 
                    "${DICTIONARY_INCLUDE_DIRECTORIES}" ""
                   )

#Set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )

Add_Library(${libname} SHARED ${${libname}Source} ${Dictionary})

SET_TARGET_PROPERTIES(${libname} PROPERTIES SUFFIX .so)

target_link_libraries(AraDisplay AraEvent AraCorrelator ${ROOT_GUI_LIBRARY})

install(TARGETS AraDisplay DESTINATION ${ARAEVENT_INSTALL_PATH}/lib)
install(FILES ${${libname}Headers} DESTINATION ${ARAEVENT_INSTALL_PATH}/include)
