Set(INCLUDE_DIRECTORIES  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

File(GLOB ${libname}Headers AraVertex.h L2.h L2Structure.h iceProp.h
	  )

#message(STATUS "${libname}Headers ${${libname}Headers}")

include_directories( ${INCLUDE_DIRECTORIES} )
add_executable(exampleLoopL2Test exampleLoopL2.cxx)

#exampleLoopL2 uses AraEvent
target_link_libraries(exampleLoopL2Test  AraEvent)

install(FILES ${${libname}Headers} DESTINATION ${ARAROOT_INSTALL_PATH}/include)
install(TARGETS exampleLoopL2Test DESTINATION ${ARAROOT_INSTALL_PATH}/bin)
