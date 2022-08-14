link_libraries (polymorph_vendor)

file (GLOB_RECURSE SOURCES src/*.cpp)

add_library (polymorph_engine ${SOURCES})
target_include_directories (polymorph_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(polymorph_engine PROPERTIES OUTPUT_NAME polymorph)