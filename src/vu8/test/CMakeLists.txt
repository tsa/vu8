macro(vu8_test _name _file)
    add_test(${_name} ../bin/vu8bin ${CMAKE_CURRENT_SOURCE_DIR}/${_file})
endmacro()

vu8_test(LoadModule loadmodule.js)
vu8_test(File file.js)

