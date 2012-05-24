cmake_minimum_required (VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(ARAEVENT_INSTALL_PATH $ENV{ARA_UTIL_INSTALL_DIR})
#set(CMAKE_SHARED_LINKER_FLAGS " -dynamiclib")




project(AraRoot)
find_package(ROOT REQUIRED COMPONENTS MathMore Gui)
find_package(libRootFftwWrapper REQUIRED)
find_package(FFTW REQUIRED)
find_package(sqlite3 REQUIRED)
find_package(zlib REQUIRED)


add_subdirectory(AraEvent) #FIXME
add_subdirectory(AraCorrelator) #FIXME
add_subdirectory(AraDisplay) #FIXME
add_subdirectory(AraWebPlotter) #FIXME --Got to change to a library specific list of header files for all libraries as we run into cmake issues with this generic "everything that has .h at the end" business

add_subdirectory(utilities/Atri) #FIXME
add_subdirectory(utilities/Icrr) #FIXME
