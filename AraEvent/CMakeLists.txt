Set(libname AraEvent)
# ROOT is included in the template.
Set(INCLUDE_DIRECTORIES ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})

include(AraRootCMakeLists)

target_link_libraries(AraEvent ${LIBROOTFFTWWRAPPER_LIBRARIES} ${SQLITE3_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})


install(TARGETS AraEvent DESTINATION ${ARAEVENT_INSTALL_PATH}/lib)

FILE(GLOB antennaFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/*.sqlite")
FILE(GLOB atriCalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ATRI/*.txt")
FILE(GLOB icrrTestBedCalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/TestBed/*")
FILE(GLOB icrrStation1CalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/Station1/*")


install(FILES ${antennaFiles} DESTINATION ${ARAEVENT_INSTALL_PATH}/share/araCalib)
install(FILES ${atriCalibFiles} DESTINATION ${ARAEVENT_INSTALL_PATH}/share/araCalib/ATRI)
install(FILES ${icrrTestBedCalibFiles} DESTINATION ${ARAEVENT_INSTALL_PATH}/share/araCalib/ICRR/TestBed)
install(FILES ${icrrStation1CalibFiles} DESTINATION ${ARAEVENT_INSTALL_PATH}/share/araCalib/ICRR/Station1)

