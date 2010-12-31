set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost COMPONENTS program_options filesystem system)

set(vu8_sources vu8.cpp)
add_executable(vu8bin ${vu8_sources} ${V8_DYNAMIC})
target_link_libraries(vu8bin ${V8_DYNAMIC} ${V8_LINK} ${Boost_LIBRARIES} vu8 dl)

