Set(libname AraConfig)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")

add_library(AraConfig SHARED configFileSpec.cxx configStore.cxx configLoad.cxx configError.cxx)

target_link_libraries(AraConfig AraKvp)

