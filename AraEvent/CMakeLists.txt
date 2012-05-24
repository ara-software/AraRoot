Set(libname AraEvent)
# ROOT is included in the template.
Set(INCLUDE_DIRECTORIES ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})

File(GLOB ${libname}Headers AraAntennaInfo.h            AraStationInfo.h            RawIcrrStationEvent.h       araIcrrDefines.h
AraEventCalibrator.h        AtriEventHkData.h           RawAraGenericHeader.h       RawIcrrStationHeader.h      araIcrrStructures.h
AraFileUtility.h            AtriSensorHkData.h          RawAraStationEvent.h        UsefulAraStationEvent.h     araSoft.h
AraGeomTool.h               FullIcrrHkEvent.h           RawAtriSimpleStationEvent.h UsefulAtriStationEvent.h
AraRawIcrrRFChannel.h       IcrrHkData.h                RawAtriStationBlock.h       UsefulIcrrStationEvent.h
AraRootVersion.h            IcrrTriggerMonitor.h        RawAtriStationEvent.h       araAtriStructures.h
	  )

File(GLOB ${libname}Source AraAntennaInfo.cxx            AraRawIcrrRFChannel.cxx       FullIcrrHkEvent.cxx           RawAraStationEvent.cxx        RawIcrrStationEvent.cxx       UsefulIcrrStationEvent.cxx  AraEventCalibrator.cxx     AraStationInfo.cxx            IcrrHkData.cxx                RawAtriSimpleStationEvent.cxx RawIcrrStationHeader.cxx
AraFileUtility.cxx         AtriEventHkData.cxx           IcrrTriggerMonitor.cxx        RawAtriStationBlock.cxx       UsefulAraStationEvent.cxx
AraGeomTool.cxx            AtriSensorHkData.cxx          RawAraGenericHeader.cxx       RawAtriStationEvent.cxx       UsefulAtriStationEvent.cxx
	  )

#message(STATUS "${libname}Source ${${libname}Source}")
#message(STATUS "${libname}Headers ${${libname}Headers}")

Set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
Set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/G__${libname}.cxx)

Set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES}  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter)

ROOT_GENERATE_DICTIONARY("${${libname}Headers}" 
                    "${LinkDef}" "${Dictionary}" 
                    "${DICTIONARY_INCLUDE_DIRECTORIES}" ""
                   )

Set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})

include_directories( ${INCLUDE_DIRECTORIES} )

Add_Library(${libname} SHARED ${${libname}Source} ${Dictionary})

SET_TARGET_PROPERTIES(${libname} PROPERTIES SUFFIX .so)

target_link_libraries(AraEvent ${LIBROOTFFTWWRAPPER_LIBRARIES} ${SQLITE3_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

install(TARGETS AraEvent DESTINATION ${ARAEVENT_INSTALL_PATH}/lib)
install(FILES ${${libname}Headers} DESTINATION ${ARAEVENT_INSTALL_PATH}/include)

FILE(GLOB antennaFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/*.sqlite")
FILE(GLOB atriCalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ATRI/*.txt")
FILE(GLOB icrrTestBedCalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/TestBed/*.txt" "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/TestBed/*.dat")
FILE(GLOB icrrStation1CalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/Station1/*.txt" "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/Station1/*.dat")


install(FILES ${antennaFiles} DESTINATION ${ARAEVENT_INSTALL_PATH}/share/araCalib)
install(FILES ${atriCalibFiles} DESTINATION ${ARAEVENT_INSTALL_PATH}/share/araCalib/ATRI)
install(FILES ${icrrTestBedCalibFiles} DESTINATION ${ARAEVENT_INSTALL_PATH}/share/araCalib/ICRR/TestBed)
install(FILES ${icrrStation1CalibFiles} DESTINATION ${ARAEVENT_INSTALL_PATH}/share/araCalib/ICRR/Station1)

