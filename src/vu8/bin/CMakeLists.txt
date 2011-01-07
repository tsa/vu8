set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost COMPONENTS program_options filesystem system)

set(vu8_sources vu8.cpp)
add_executable(vu8bin ${vu8_sources})
add_dependencies(vu8bin buildextv8)

# cmake 2.4 is deficient, cmake 2.8 populates these based on COMPONENTS
if (NOT Boost_LIBRARIES)
    set(Boost_LIBRARIES boost_program_options-mt boost_filesystem-mt boost_system-mt)
endif()

target_link_libraries(vu8bin ${V8_LIB_DYNAMIC} ${V8_LINK} ${Boost_LIBRARIES} vu8 dl)

install(TARGETS vu8bin DESTINATION bin)
