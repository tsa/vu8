cmake_minimum_required(VERSION 2.8)

project(vu8)

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

add_custom_command(
    OUTPUT ${V8_STATIC} ${V8_DYNAMIC}
    COMMAND gmake -C ${CMAKE_CURRENT_BINARY_DIR}
                  -f ${CMAKE_CURRENT_SOURCE_DIR}/mk/v8
                  buildv8 mode=${V8_MODE})

include_directories(${V8_INCLUDE_DIR} ..)
set(vu8_sources vu8.cpp Context.cpp)
add_executable(vu8 ${vu8_sources} ${V8_STATIC})
target_link_libraries(vu8 ${V8_STATIC} ${V8_LINK})
