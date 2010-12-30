cmake_minimum_required(VERSION 2.8)

project(v8)

option(V8_DEBUG "compile v8 in debugging mode" off)
if(V8_DEBUG)
    set(V8_MODE debug)
    set(V8_SUFFIX _g)
else()
    set(V8_MODE release)
endif()

set(V8_STATIC ${CMAKE_CURRENT_SOURCE_DIR}/v8/libv8${V8_SUFFIX}.a)
set(V8_DYNAMIC ${CMAKE_CURRENT_SOURCE_DIR}/v8/libv8${V8_SUFFIX}.so)
set(V8_LINK pthread)

add_custom_command(
    OUTPUT ${V8_STATIC} ${V8_DYNAMIC}
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && gmake buildv8 mode=${V8_MODE})

include_directories(v8/include)
add_executable(v8run v8run.cpp ${V8_STATIC})
target_link_libraries(v8run ${V8_STATIC} ${V8_LINK})
