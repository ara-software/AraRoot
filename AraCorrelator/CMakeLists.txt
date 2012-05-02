set(libname AraCorrelator)
find_package(libRootFFTWWrapper)
find_package(ROOT)

set(EXCLUDE_SOURCES makeCorrelationMaps.cxx)
set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraEvent ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${SQLITE3_INCLUDES})
set(DICTIONARY_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraEvent)

include(AraRootCMakeLists)

# AraCorrelator uses AraEvent, and LibRootFFTWWrapper, and ROOT libraries
target_link_libraries(AraCorrelator AraEvent ${LIBROOTFFTWWRAPPER_LIBRARIES})

set_target_properties( AraCorrelator PROPERTIES VERSION 3.1.0 SOVERSION 3 )

add_executable(makeCorrelationMaps makeCorrelationMaps.cxx)

# makeCorrelationMaps uses AraCorrelator, AraEvent, and ROOT
target_link_libraries(makeCorrelationMaps AraCorrelator AraEvent)

install(TARGETS AraCorrelator DESTINATION ${ARAEVENT_INSTALL_PATH}/lib)
