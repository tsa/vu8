cmake_minimum_required(VERSION 2.8)
set(V8_STATIC ${CMAKE_CURRENT_SOURCE_DIR}/v8/libv8.a)
set(V8_DYNAMIC ${CMAKE_CURRENT_SOURCE_DIR}/v8/libv8.so)

add_custom_command(
    OUTPUT ${V8_STATIC} ${V8_DYNAMIC}
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && gmake buildv8)

include_directories(v8/include)
add_executable(v8run v8run.cpp ${V8_STATIC})
target_link_libraries(v8run ${V8_STATIC})
