# Find all headers except the LinkDef file and copy
# them to the include subdirectory of the build directory
File(GLOB ${libname}Headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
    )
List(REMOVE_ITEM ${libname}Headers 
                 "${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h" 
    )
List(REMOVE_ITEM ${libname}Headers
                 "${CMAKE_CURRENT_SOURCE_DIR}/G__${libname}.h"
		 ${EXCLUDE_HEADERS}
    )
List(REMOVE_ITEM ${libname}Headers 
                 "${CMAKE_CURRENT_SOURCE_DIR}/*~" 
)
# Find all source files
File(GLOB ${libname}Source "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx" 
  )
List(REMOVE_ITEM ${libname}Source 
                 "${CMAKE_CURRENT_SOURCE_DIR}/*~" 
)



if (EXCLUDE_SOURCES)
  List(REMOVE_ITEM ${libname}Source
    ${EXCLUDE_SOURCES}
    )
endif()

# Define the LinkDef and the output file which are needed
# to create the dictionary.
# Definde the needed include directories and create the Dictionary
Set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
Set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/G__${libname}.cxx)

#jpd testing including everything
#Set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR})

Set(DICTIONARY_INCLUDE_DIRECTORIES ${DICTIONARY_INCLUDE_DIRECTORIES}  ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter)



ROOT_GENERATE_DICTIONARY("${${libname}Headers}" 
                    "${LinkDef}" "${Dictionary}" 
                    "${DICTIONARY_INCLUDE_DIRECTORIES}" ""
                   )

#jpd testing including everything
#Set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

Set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR}/AraEvent ${CMAKE_SOURCE_DIR}/AraCorrelator ${CMAKE_SOURCE_DIR}/AraDisplay ${CMAKE_SOURCE_DIR}/AraWebPlotter ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})


include_directories( ${INCLUDE_DIRECTORIES} )

# Here all objects ar put together into one share library
Add_Library(${libname} SHARED ${${libname}Source} ${Dictionary})

#This line forces cmake to create .so files on MAC systems
SET_TARGET_PROPERTIES(${libname} PROPERTIES SUFFIX .so)




# Linkages are added OUTSIDE of this file

# Define all the header files which should be installed when
# doing a "make install"

Install(FILES ${${libname}Headers} DESTINATION ${ARAEVENT_INSTALL_PATH}/include)
