add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/v8/libv8.so
           ${CMAKE_CURRENT_SOURCE_DIR}/v8/libv8.a
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && gmake buildv8)

include_directories(v8/include)
add_executable(v8run v8run.cpp v8/libv8.a)
