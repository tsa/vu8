set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../share/cmake")
include(Vu8)

vu8_plugin(console console.cpp)
vu8_plugin(file file.cpp)
vu8_plugin(os os.cpp)
