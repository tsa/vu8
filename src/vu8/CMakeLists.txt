cmake_minimum_required(VERSION 2.4)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

project(vu8)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

enable_testing()

option(V8_DEBUG "compile v8 in debugging mode" off)
option(V8_STATIC "compile static libv8" off)
option(V8_DYNAMIC "compile dynamic libv8" on)

set(VU8_PLUGIN_PATH "lib" CACHE STRING "location of vu8 shared library plugins" )
set(VU8_PP_ITERATION_LIMIT 10 CACHE STRING 10)

set(V8_LIB_STATIC ${CMAKE_CURRENT_BINARY_DIR}/v8/libv8${V8_SUFFIX}.a)
set(V8_LIB_DYNAMIC ${CMAKE_CURRENT_BINARY_DIR}/v8/libv8${V8_SUFFIX}.so)
set(V8_LINK pthread)
set(V8_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/v8/include)

if(V8_DEBUG)
    set(V8_MODE debug)
    set(V8_SUFFIX _g)
else()
    set(V8_MODE release)
endif()

if(V8_STATIC)
    if(V8_DYNAMIC)
        set(V8_TARGET buildv8)
        set(V8_LIBS ${V8_LIB_DYNAMIC} ${V8_LIB_STATIC})
    else()
        set(V8_TARGET staticv8)
        set(V8_LIBS ${V8_LIB_STATIC})
    endif()
else()
    set(V8_TARGET dynamicv8)
    set(V8_LIBS ${V8_LIB_DYNAMIC})
endif()

set (VU8_INCLUDE_PATH ${CMAKE_CURRENT_BINARY_DIR}/include)
configure_file(config.hpp.cmake ${VU8_INCLUDE_PATH}/vu8/config.hpp)
include_directories(${V8_INCLUDE_DIR} .. ${VU8_INCLUDE_PATH})

add_custom_command(
    OUTPUT ${V8_LIBS}
    COMMAND make ${V8_TARGET} -C ${CMAKE_CURRENT_BINARY_DIR}
                              -f ${CMAKE_CURRENT_SOURCE_DIR}/mk/v8
                              mode=${V8_MODE})
add_custom_target(buildextv8 ALL DEPENDS ${V8_LIB_DYNAMIC})

add_library(vu8 STATIC Context.cpp)
add_dependencies(vu8 buildextv8)

file(GLOB vu8_headers *.hpp)
file(GLOB vu8_detail_headers *.hpp)
install(PROGRAMS ${V8_LIBS} DESTINATION ${VU8_PLUGIN_PATH})
install(FILES ${vu8_headers} DESTINATION include/vu8)
install(FILES ${vu8_detail_headers} DESTINATION include/vu8/detail)

set(Boost_USE_MULTITHREADED ON)
add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(test)
