if(NOT VU8_INCLUDE_DIR)
    set(VU8_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
endif()

include_directories(${VU8_INCLUDE_DIR})
if(EXISTS "${VU8_INCLUDE_DIR}/vu8/obj")
    # within VU8 build system
    include_directories("${VU8_INCLUDE_DIR}/vu8/obj/v8/include")
    include_directories("${VU8_INCLUDE_DIR}/vu8/obj/include")
endif()

# TODO: set VU8_ROOT
# TODO: set V8_LIB_DYNAMIC

macro(vu8_plugin _name _sources)
    add_library(vu8_${_name} SHARED ${_sources})
    target_link_libraries(vu8_${_name} ${V8_LIB_DYNAMIC})
    add_dependencies(vu8_${_name} buildextv8)
    install(TARGETS vu8_${_name} DESTINATION ${VU8_PLUGIN_PATH})
endmacro()
