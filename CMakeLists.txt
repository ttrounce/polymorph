cmake_minimum_required (VERSION 3.6)
project (polymorph)

set(CMAKE_CXX_STANDARD 17)

function(add_shader)
    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
        OUTPUT ${SHADER_BINARY_DIR}
        COMMENT "Creating ${SHADER_BINARY_DIR}"
    )

    foreach(source ${SHADER_SRC})
        get_filename_component(FILENAME ${source} NAME)
        add_custom_command(
            COMMAND ${glslangValidator} -V ${source} -o ${SHADER_BINARY_DIR}/${FILENAME}.spv
            OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
            DEPENDS ${source} ${SHADER_BINARY_DIR}
            COMMENT "Compiling ${FILENAME}"
        )
        list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
    endforeach()

    add_custom_target(polymorph_shaders ALL DEPENDS ${SPV_SHADERS})
    add_dependencies(polymorph_example polymorph_shaders)
endfunction(add_shader)

add_subdirectory(vendor)
add_subdirectory(polymorph)
add_subdirectory(example)
