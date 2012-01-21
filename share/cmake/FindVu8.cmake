# Searches for vu8
# Input variables
#      VU8_ROOT        -  prefix vu8 was installed into
# Output variables
#      VU8_FOUND       -  set to true if v8 is found
#      VU8_ROOT        -  prefix vu8 was installed into is determined if
#                         it is not already set
#      VU8_INCLUDE_DIRS   -  include directories needed to build against vu8
#      VU8_V8_INCLUDE_DIR -  include directory for vu8's copy of v8 if
#                            applicable
# Defines macros
#      vu8_plugin("module_name" "src1.cpp;src2.hpp")

if(NOT VU8_ROOT)
    if (EXISTS "${CMAKE_INSTALL_PREFIX}/include/vu8")
        set(VU8_ROOT "${CMAKE_INSTALL_PREFIX}")
    endif()
endif()

set(VU8_INCLUDE_DIRS "${VU8_ROOT}/include")

if(EXISTS "${VU8_ROOT}/vu8")
    set(VU8_FOUND true)
    # TODO: set VU8_LIB_DYNAMIC
endif()

macro(vu8_plugin _name _sources)
    add_library(vu8_${_name} SHARED ${_sources})
    target_link_libraries(vu8_${_name} ${V8_LIB_DYNAMIC})
    if(VU8_INTERNAL_BUILD AND NOT OWN_V8)
        add_dependencies(vu8_${_name} buildextv8)
    endif()
    install(TARGETS vu8_${_name} DESTINATION ${VU8_PLUGIN_PATH})
endmacro()
