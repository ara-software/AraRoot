cmake_minimum_required (VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(ARAEVENT_INSTALL_PATH $ENV{ARA_UTIL_INSTALL_DIR})

#Set the hint variables to point to where cmake should look first for a header / library file
#This is in case you have multiple instances of for example sqlite3 - one of which is known to be the correct version
set(SQLITE3_HINT_INCLUDES "~/repositories/sqlite3Test/include/")
set(SQLITE3_HINT_LIBRARIES "~/repositories/sqlite3Test/lib/")

project(AraRoot)
find_package(ROOT REQUIRED COMPONENTS MathMore Gui)
find_package(libRootFftwWrapper REQUIRED)
find_package(FFTW REQUIRED)
find_package(sqlite3 REQUIRED)
find_package(zlib REQUIRED)

#Build these sub-directories by searching for CMakeLists.txt files in there
add_subdirectory(AraEvent)
add_subdirectory(AraCorrelator)
add_subdirectory(AraDisplay)
add_subdirectory(AraWebPlotter)
add_subdirectory(utilities/Atri)
add_subdirectory(utilities/Icrr)

#add_subdirectory(analysis) #FIXME - Do we want to include this in CMake?
#add_subdirectory(AraVertexRec) #FIXME - Need to add build of L2 vertexing
