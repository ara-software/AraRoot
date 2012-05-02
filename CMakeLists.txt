cmake_minimum_required (VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(ARAEVENT_INSTALL_PATH $ENV{ARA_UTIL_INSTALL_DIR})



project(AraRoot)
find_package(ROOT REQUIRED COMPONENTS MathMore Gui)
find_package(libRootFftwWrapper REQUIRED)
find_package(FFTW REQUIRED)
find_package(sqlite3 REQUIRED)
find_package(zlib REQUIRED)


add_subdirectory(AraEvent)
add_subdirectory(AraCorrelator)
add_subdirectory(AraDisplay)
add_subdirectory(AraWebPlotter)

#add_subdirectory(utilities) #FIXME
