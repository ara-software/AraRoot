Set(INCLUDE_DIRECTORIES 
	${CMAKE_SOURCE_DIR}/AraEvent 
	${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} 
	${ROOT_INCLUDE_DIRS} 
	)

include_directories( ${INCLUDE_DIRECTORIES} )

Set(TEST_DATA_DIR
  ${CMAKE_SOURCE_DIR}/test/test_data 
  )


add_executable(basicTest basicTest.cxx)
target_link_libraries(basicTest 
	AraEvent 
	${ROOT_LIBRARIES} 
	${ZLIB_LIBRARIES})

add_test(NAME BasicTest COMMAND basicTest ${TEST_DATA_DIR}/test_A2_run2000.root)

