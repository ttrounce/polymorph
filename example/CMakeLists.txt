link_libraries (polymorph_engine)

file (GLOB_RECURSE SOURCES src/*.h src/*.cpp)
add_executable(polymorph_example ${SOURCES})
target_link_libraries (polymorph_example polymorph_engine)