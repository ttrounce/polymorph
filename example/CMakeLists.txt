link_libraries (polymorph_engine)

file (GLOB_RECURSE SOURCES src/*.h src/*.cpp)
add_executable(polymorph_example ${SOURCES})
target_link_libraries (polymorph_example polymorph_engine)

set_target_properties(polymorph_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

file(GLOB SHADER_SRC
    shader/*.frag
    shader/*.vert
)

find_program(glslangValidator NAMES glslangValidator HINTS Vulkan::glslc)

set(SHADER_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/shader)

add_shader()