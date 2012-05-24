Set(libname AraCorrelator)
Set(INCLUDE_DIRECTORIES  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

File(GLOB ${libname}Headers AraEventCorrelator.h
	  )

File(GLOB ${libname}Source AraEventCorrelator.cxx  makeCorrelationMaps.cxx
	  )

#message(STATUS "${libname}Headers ${${libname}Headers}")
#message(STATUS "${libname}Source ${${libname}Source}")


Set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
Set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/G__${libname}.cxx)

Set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES}  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator)

ROOT_GENERATE_DICTIONARY("${${libname}Headers}" 
                    "${LinkDef}" "${Dictionary}" 
                    "${DICTIONARY_INCLUDE_DIRECTORIES}" ""
                   )


include_directories( ${INCLUDE_DIRECTORIES} )

Add_Library(${libname} SHARED ${${libname}Source} ${Dictionary})

SET_TARGET_PROPERTIES(${libname} PROPERTIES SUFFIX .so)

target_link_libraries(AraCorrelator AraEvent)

add_executable(makeCorrelationMaps makeCorrelationMaps.cxx)

# makeCorrelationMaps uses AraCorrelator, AraEvent, and ROOT
target_link_libraries(makeCorrelationMaps AraCorrelator AraEvent)

install(FILES ${${libname}Headers} DESTINATION ${ARAEVENT_INSTALL_PATH}/include)
install(TARGETS AraCorrelator DESTINATION ${ARAEVENT_INSTALL_PATH}/lib)
#not sure this will work
install(TARGETS makeCorrelationMaps DESTINATION ${ARAEVENT_INSTALL_PATH}/bin)
