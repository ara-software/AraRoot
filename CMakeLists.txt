cmake_minimum_required (VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(ARAROOT_INSTALL_PATH $ENV{ARA_UTIL_INSTALL_DIR})
#set(CMAKE_CXX_STANDARD 11) #you may also have to specify that you want C++11 standard

#Set the hint variables to point to where cmake should look first for a header / library file
#This is in case you have multiple instances of for example sqlite3 - one of which is known to be the correct version
#set(SQLITE3_HINT_INCLUDES "~/repositories/InstallDir/utilities/include/")
#set(SQLITE3_HINT_LIBRARIES "~/repositories/InstallDir/utilities/lib")
#you might also need to specify FFTW stuff
set(FFTW_LIBRARIES "$ENV{PLATFORM_DIR}/lib/libfftw3.so.3.4.4")
set(FFTW_INCLUDES "$ENV{PLATFORM_DIR}/include")

project(AraRoot)
find_package(ROOT REQUIRED COMPONENTS MathMore Gui)
find_package(libRootFftwWrapper REQUIRED)
#find_package(FFTW REQUIRED)
find_package(sqlite3 REQUIRED)
find_package(zlib REQUIRED)

#Build these sub-directories by searching for CMakeLists.txt files in there
add_subdirectory(AraEvent)
add_subdirectory(AraCorrelator)
add_subdirectory(AraDisplay)
add_subdirectory(AraWebPlotter)
add_subdirectory(utilities/Atri)
add_subdirectory(utilities/Icrr)
add_subdirectory(analysis)
add_subdirectory(AraVertex)
add_subdirectory(calibration/antCalib) 
add_subdirectory(calibration/ATRI) 
add_subdirectory(calibration/ATRI/voltageCalib) 

add_subdirectory(debugging/sample_lines)
add_subdirectory(debugging/data_lines)
add_subdirectory(debugging/operating_parameters/vadj)
add_subdirectory(debugging/operating_parameters/vdly)
add_subdirectory(debugging/thresholds)
