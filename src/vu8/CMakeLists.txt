cmake_minimum_required(VERSION 2.4)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

project(vu8)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

enable_testing()

option(VU8_INSTALL_CMAKE_MODULE "install cmake module for find_package(Vu8)" on)
option(VU8_FORCE_32 "force 64-bit system to produce 32-bit libraries" off)

set(VU8_PLUGIN_PATH "lib" CACHE STRING "location of vu8 shared library plugins")
set(VU8_PP_ITERATION_LIMIT 10 CACHE STRING
    "iteration limit for template meta-programming")

set(CMAKE_CXX_FLAGS "-Wall")

if(VU8_FORCE_32)
    set(VU8_ARCH ia32)
    add_definitions(-m32)
    set(CMAKE_EXE_LINKER_FLAGS "-m32")
    set(CMAKE_SHARED_LINKER_FLAGS "-m32")
    set(CMAKE_STATIC_LINKER_FLAGS "-m32")
else()
    execute_process(COMMAND uname -m OUTPUT_VARIABLE VU8_ARCH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (VU8_ARCH STREQUAL x86_64)
        set(VU8_ARCH x64)
    else()
        set(VU8_ARCH ia32)
    endif()
endif()

if(V8_DEBUG)
    set(V8_MODE debug)
    set(V8_SUFFIX _g)
else()
    set(V8_MODE release)
endif()

option(OWN_V8 "use v8 installed on system" on)
if(NOT OWN_V8)
    option(V8_DEBUG "compile v8 in debugging mode" off)
    option(V8_DYNAMIC "compile dynamic libv8" on)
    set(V8_PYTHON_BIN "python" CACHE STRING
        "python to use during v8 build process (must be python 2)")

    set(V8_VERSION "3.2.3.1" CACHE STRING "v8 version")
    # latest:
    # set(V8_VERSION "3.8.8" CACHE STRING "v8 version")

    set(V8_LIB_BUILD ${CMAKE_CURRENT_BINARY_DIR}/v8-${V8_VERSION})

    set(V8_LIB_DYNAMIC ${V8_LIB_BUILD}/libv8${V8_SUFFIX}.so)
    set(V8_LINK pthread)
    set(V8_ROOT ${CMAKE_CURRENT_BINARY_DIR}/v8-${V8_VERSION})

    set(V8_TARGET dynamicv8)

    add_custom_command(
        OUTPUT ${V8_LIB_DYNAMIC}
        COMMAND make ${V8_TARGET} -C ${CMAKE_CURRENT_BINARY_DIR}
                                  -f ${CMAKE_CURRENT_SOURCE_DIR}/mk/v8
                                  mode=${V8_MODE} CXX=${CMAKE_CXX_COMPILER}
                                  v8ver=${V8_VERSION} arch=${VU8_ARCH}
                                  python=${V8_PYTHON_BIN})
    add_custom_target(buildextv8 ALL DEPENDS ${V8_LIB_DYNAMIC})
else()
    # also search for v8 in install location
    find_library(V8_LIB_DYNAMIC NAMES v8${V8_SUFFIX}
                 PATHS "${CMAKE_INSTALL_PREFIX}/lib")

    get_filename_component(V8_LIB_DIR ${V8_LIB_DYNAMIC} PATH)
    get_filename_component(V8_ROOT ${V8_LIB_DIR} PATH)
endif()

include_directories("${V8_ROOT}/include")

set(config_hpp ${CMAKE_CURRENT_BINARY_DIR}/include/vu8/config.hpp)
configure_file(config.hpp.cmake ${config_hpp})

# use FindVu8 to resolve internal dependency
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../share/cmake")
set(VU8_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
include_directories(${VU8_ROOT} ${VU8_ROOT}/vu8/obj/include)

# to get the vu8_plugin macro
set(VU8_INTERNAL_BUILD on)
find_package(Vu8)

add_library(vu8 SHARED Context.cpp)
target_link_libraries(vu8 ${V8_LIB_DYNAMIC})
if(NOT OWN_V8)
    add_dependencies(vu8 buildextv8)
endif()

file(GLOB vu8_headers *.hpp)
file(GLOB vu8_detail_headers detail/*.hpp)
install(PROGRAMS ${V8_LIB_DYNAMIC} DESTINATION ${VU8_PLUGIN_PATH})
install(TARGETS vu8 DESTINATION lib)
install(FILES ${vu8_headers} ${config_hpp} DESTINATION include/vu8)
install(FILES ${vu8_detail_headers} DESTINATION include/vu8/detail)

if (VU8_INSTALL_CMAKE_MODULE)
    install(FILES ../../share/cmake/FindVu8.cmake
            DESTINATION ${CMAKE_ROOT}/Modules)
endif()

set(Boost_USE_MULTITHREADED ON)
add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(test)
