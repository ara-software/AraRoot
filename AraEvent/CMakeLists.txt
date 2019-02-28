Set(libname AraEvent)
# ROOT is included in the template.
Set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
Set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/G__${libname}.cxx)
Set(DICTIONARY_INCLUDE_DIRECTORIES  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter ${DICTIONARY_INCLUDE_DIRECTORIES})
Set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${SQLITE3_INCLUDES} ${ZLIB_INCLUDE_DIRS} ${INCLUDE_DIRECTORIES})

#These are the headers
File(GLOB ${libname}Headers AraAntennaInfo.h            AraStationInfo.h            RawIcrrStationEvent.h       araIcrrDefines.h
AraEventCalibrator.h        AtriEventHkData.h           RawAraGenericHeader.h       RawIcrrStationHeader.h      araIcrrStructures.h
AtriSensorHkData.h          RawAraStationEvent.h        UsefulAraStationEvent.h     araSoft.h  			AraGeomTool.h
FullIcrrHkEvent.h           RawAtriSimpleStationEvent.h UsefulAtriStationEvent.h    AraRawIcrrRFChannel.h       IcrrHkData.h                
RawAtriStationBlock.h       UsefulIcrrStationEvent.h   	AraRootVersion.h            IcrrTriggerMonitor.h        RawAtriStationEvent.h       
araAtriStructures.h	    AraCalAntennaInfo.h         AraSunPos.h         AraQualCuts.h         AraEventConditioner.h
	  )

#Source for library
File(GLOB ${libname}Source AraAntennaInfo.cxx  AraCalAntennaInfo.cxx          AraRawIcrrRFChannel.cxx       FullIcrrHkEvent.cxx           RawAraStationEvent.cxx        RawIcrrStationEvent.cxx       UsefulIcrrStationEvent.cxx  AraEventCalibrator.cxx     AraStationInfo.cxx            IcrrHkData.cxx                 RawIcrrStationHeader.cxx
  AtriEventHkData.cxx    RawAtriSimpleStationEvent.cxx	   IcrrTriggerMonitor.cxx        RawAtriStationBlock.cxx       UsefulAraStationEvent.cxx     AraGeomTool.cxx               AtriSensorHkData.cxx          RawAraGenericHeader.cxx     RawAtriStationEvent.cxx       UsefulAtriStationEvent.cxx          AraSunPos.cxx           AraQualCuts.cxx           AraEventConditioner.cxx
	  )

#Generate the ROOT dictionary using the ROOT CMake function
ROOT_GENERATE_DICTIONARY("${${libname}Headers}" 
                    "${LinkDef}" "${Dictionary}" 
                    "${DICTIONARY_INCLUDE_DIRECTORIES}" ""
                   )
#Now create the library

include_directories( ${INCLUDE_DIRECTORIES} )
Add_Library(${libname} SHARED ${${libname}Source} ${Dictionary})
SET_TARGET_PROPERTIES(${libname} PROPERTIES SUFFIX .so)

#Set up the linking to pre-requisite libraries (sqlite etc...)
target_link_libraries(AraEvent ${LIBROOTFFTWWRAPPER_LIBRARIES} ${SQLITE3_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})

#Set the install paths
FILE(GLOB antennaFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/*.sqlite")
FILE(GLOB atriCalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ATRI/*.txt")
FILE(GLOB icrrTestBedCalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/TestBed/*.txt" "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/TestBed/*.dat")
FILE(GLOB icrrStation1CalibFiles "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/Station1/*.txt" "${CMAKE_CURRENT_SOURCE_DIR}/calib/ICRR/Station1/*.dat")

install(TARGETS AraEvent DESTINATION ${ARAROOT_INSTALL_PATH}/lib)
install(FILES ${${libname}Headers} DESTINATION ${ARAROOT_INSTALL_PATH}/include)

install(FILES ${antennaFiles} DESTINATION ${ARAROOT_INSTALL_PATH}/share/araCalib)
install(FILES ${atriCalibFiles} DESTINATION ${ARAROOT_INSTALL_PATH}/share/araCalib/ATRI)
install(FILES ${icrrTestBedCalibFiles} DESTINATION ${ARAROOT_INSTALL_PATH}/share/araCalib/ICRR/TestBed)
install(FILES ${icrrStation1CalibFiles} DESTINATION ${ARAROOT_INSTALL_PATH}/share/araCalib/ICRR/Station1)

