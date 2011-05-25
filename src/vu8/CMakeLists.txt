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

option(VU8_FORCE_32 "force 64-bit system to produce 32-bit libraries" off)

set(V8_VERSION "3.2.3.1" CACHE STRING "v8 version")
set(VU8_PLUGIN_PATH "lib" CACHE STRING "location of vu8 shared library plugins")
set(VU8_PP_ITERATION_LIMIT 10 CACHE STRING
    "iteration limit for template meta-programming")

set(V8_LIB_BUILD ${CMAKE_CURRENT_BINARY_DIR}/v8-${V8_VERSION})

set(V8_LIB_STATIC ${V8_LIB_BUILD}/libv8${V8_SUFFIX}.a)
set(V8_LIB_DYNAMIC ${V8_LIB_BUILD}/libv8${V8_SUFFIX}.so)
set(V8_LINK pthread)
set(V8_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/v8-${V8_VERSION}/include)

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

configure_file(config.hpp.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/include/vu8/config.hpp)

if(VU8_FORCE_32)
    set(VU8_ARCH ia32)
    add_definitions(-m32)
    set(CMAKE_EXE_LINKER_FLAGS "-m32")
    set(CMAKE_SHARED_LINKER_FLAGS "-m32")
    set(CMAKE_STATIC_LINKER_FLAGS "-m32")
else()
    execute_process(COMMAND uname -s OUTPUT_VARIABLE VU8_ARCH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (VU8_ARCH STREQUAL x86_64)
        set(VU8_ARCH x86_64)
    else()
        set(VU8_ARCH ia32)
    endif()
endif()


add_custom_command(
    OUTPUT ${V8_LIBS}
    COMMAND make ${V8_TARGET} -C ${CMAKE_CURRENT_BINARY_DIR}
                              -f ${CMAKE_CURRENT_SOURCE_DIR}/mk/v8
                              mode=${V8_MODE} CXX=${CMAKE_CXX_COMPILER}
                              v8ver=${V8_VERSION} arch=${VU8_ARCH})
add_custom_target(buildextv8 ALL DEPENDS ${V8_LIB_DYNAMIC})

# use FindVu8 to resolve internal dependency
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../share/cmake")
set(VU8_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(VU8_INTERNAL_BUILD on)
find_package(Vu8)
if(NOT VU8_FOUND)
    message(FATAL_ERROR "could not find vu8")
endif()
include_directories(${VU8_INCLUDE_DIRS})

add_library(vu8 SHARED Context.cpp)
add_dependencies(vu8 buildextv8)

file(GLOB vu8_headers *.hpp)
file(GLOB vu8_detail_headers *.hpp)
install(PROGRAMS ${V8_LIBS} DESTINATION ${VU8_PLUGIN_PATH})
install(TARGETS vu8 DESTINATION lib)
install(FILES ${vu8_headers} DESTINATION include/vu8)
install(FILES ${vu8_detail_headers} DESTINATION include/vu8/detail)

set(Boost_USE_MULTITHREADED ON)
add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(test)
