Set(libname AraDisplay)
# ROOT is included in the template.
Set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
Set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/G__${libname}.cxx)
Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
Set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES}  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter)

File(GLOB ${libname}Headers AraAtriCanvasMaker.h        AraCorrelationFactory.h AraDisplayConventions.h AraIcrrCanvasMaker.h
AraControlPanel.h       AraDisplay.h            AraFFTGraph.h           AraWaveformGraph.h
	  )

File(GLOB ${libname}Source AraAtriCanvasMaker.cxx        AraControlPanel.cxx       AraCorrelationFactory.cxx AraDisplay.cxx            AraFFTGraph.cxx           AraIcrrCanvasMaker.cxx      AraWaveformGraph.cxx
	  )

ROOT_GENERATE_DICTIONARY("${${libname}Headers}" 
                    "${LinkDef}" "${Dictionary}" 
                    "${DICTIONARY_INCLUDE_DIRECTORIES}" ""
                   )

include_directories( ${INCLUDE_DIRECTORIES} )

Add_Library(${libname} SHARED ${${libname}Source} ${Dictionary})

SET_TARGET_PROPERTIES(${libname} PROPERTIES SUFFIX .so)

target_link_libraries(AraDisplay AraEvent AraCorrelator ${ROOT_GUI_LIBRARY})

#Install the libraries and headers
install(TARGETS AraDisplay DESTINATION ${ARAROOT_INSTALL_PATH}/lib)
install(FILES ${${libname}Headers} DESTINATION ${ARAROOT_INSTALL_PATH}/include)

#Install the ROOT macros
SET(${libname}macros macros/runAraDisplay.C)
install(FILES ${${libname}macros} DESTINATION ${ARAROOT_INSTALL_PATH}/macros)
