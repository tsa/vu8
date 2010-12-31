cmake_minimum_required(VERSION 2.8)

project(vu8)
enable_testing()

option(V8_DEBUG "compile v8 in debugging mode" off)
if(V8_DEBUG)
    set(V8_MODE debug)
    set(V8_SUFFIX _g)
else()
    set(V8_MODE release)
endif()

set(V8_STATIC ${CMAKE_CURRENT_BINARY_DIR}/v8/libv8${V8_SUFFIX}.a)
set(V8_DYNAMIC ${CMAKE_CURRENT_BINARY_DIR}/v8/libv8${V8_SUFFIX}.so)
set(V8_LINK pthread)
set(V8_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/v8/include)

include_directories(${V8_INCLUDE_DIR} ..)

add_custom_command(
    OUTPUT ${V8_STATIC} ${V8_DYNAMIC}
    COMMAND gmake -C ${CMAKE_CURRENT_BINARY_DIR}
                  -f ${CMAKE_CURRENT_SOURCE_DIR}/mk/v8
                  buildv8 mode=${V8_MODE})

add_library(vu8 STATIC Context.cpp)

set(Boost_USE_MULTITHREADED ON)
add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(test)
