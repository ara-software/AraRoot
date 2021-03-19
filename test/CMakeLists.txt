Set(INCLUDE_DIRECTORIES 
	${CMAKE_SOURCE_DIR}/AraEvent 
	${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} 
	${ROOT_INCLUDE_DIRS} 
	)

include_directories( ${INCLUDE_DIRECTORIES} )

add_executable(basicTest basicTest.cxx)
target_link_libraries(basicTest 
	AraEvent 
	${ROOT_LIBRARIES} 
	${ZLIB_LIBRARIES})

add_test(NAME BasicTest COMMAND basicTest)

