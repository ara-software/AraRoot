set(libname AraDisplay)
set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS})
set(DICTIONARY_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator)
MESSAGE("include directories ${INCLUDE_DIRECTORIES}")
include(AraRootCMakeLists)
target_link_libraries(AraDisplay AraEvent AraCorrelator ${ROOT_GUI_LIBRARY})

install(TARGETS AraDisplay DESTINATION ${ARAEVENT_INSTALL_PATH}/lib)
