Set(INCLUDE_DIRECTORIES 
	${CMAKE_SOURCE_DIR}/AraEvent 
	${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} 
	${ROOT_INCLUDE_DIRS} 
	)

include_directories( ${INCLUDE_DIRECTORIES} )

Set(TEST_DATA_DIR
  ${CMAKE_SOURCE_DIR}/test/test_data 
  )


add_executable(FileAndEventCal fileAndEventCal.cxx)
target_link_libraries(FileAndEventCal 
	AraEvent 
	${ROOT_LIBRARIES} 
	${ZLIB_LIBRARIES})

add_test(NAME File_and_EventCal_Test COMMAND FileAndEventCal ${TEST_DATA_DIR}/test_A2_run2000.root)

