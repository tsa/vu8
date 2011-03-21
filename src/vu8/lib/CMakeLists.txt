set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../share/cmake")

set(VU8_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
find_package(Vu8)

if(NOT VU8_FOUND)
    message(FATAL_ERROR "could not find vu8")
endif()

vu8_plugin(console console.cpp)
vu8_plugin(file file.cpp)
vu8_plugin(os os.cpp)
