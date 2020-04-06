Set(libname AraVertex)
Set(INCLUDE_DIRECTORIES  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

File(GLOB ${libname}Headers AraVertex.h iceProp.h AraRecoHandler.h
	  )
File(GLOB ${libname}Source AraVertex.cxx AraRecoHandler.cxx
	  )
#message(STATUS "${libname}Headers ${${libname}Headers}")

Set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
Set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/G__${libname}.cxx)

Set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES}  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraVertex)

ROOT_GENERATE_DICTIONARY("${${libname}Headers}" 
                    "${LinkDef}" "${Dictionary}" 
                    "${DICTIONARY_INCLUDE_DIRECTORIES}" ""
                   )

include_directories( ${INCLUDE_DIRECTORIES} )

Add_Library(${libname} SHARED ${${libname}Source} ${Dictionary})

SET_TARGET_PROPERTIES(${libname} PROPERTIES SUFFIX .so)

if( ${ROOT_VERSION} VERSION_GREATER "5.99/99")
  set(DICTNAME G__${libname})
  add_custom_target(${DICTNAME}.pcm DEPENDS ${DICTNAME})
endif()

target_link_libraries(AraVertex AraEvent)


add_executable(exampleLoopL2Test exampleLoopL2.cxx)
target_link_libraries(exampleLoopL2Test  AraEvent ${LIBROOTFFTWWRAPPER})

install(FILES ${${libname}Headers} DESTINATION ${ARAROOT_INSTALL_PATH}/include)
install(TARGETS AraVertex DESTINATION ${ARAROOT_INSTALL_PATH}/lib)
install(TARGETS exampleLoopL2Test DESTINATION ${ARAROOT_INSTALL_PATH}/bin)

if( ${ROOT_VERSION} VERSION_GREATER "5.99/99")
  install (FILES ${PROJECT_BINARY_DIR}/${libname}/${DICTNAME}_rdict.pcm DESTINATION ${ARAROOT_INSTALL_PATH}/lib)
endif()
