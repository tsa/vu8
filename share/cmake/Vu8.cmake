macro(vu8_plugin _name _sources)
    add_library(vu8_${_name} SHARED ${_sources})
    target_link_libraries(vu8_${_name} ${V8_DYNAMIC})
endmacro()
