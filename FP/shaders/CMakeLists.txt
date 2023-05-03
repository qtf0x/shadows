file(GLOB SHADER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
list(REMOVE_ITEM SHADER_SOURCES "CMakeLists.txt")

foreach(shader_source ${SHADER_SOURCES})
    add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${shader_source}

        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/${shader_source}
                ${CMAKE_CURRENT_BINARY_DIR}/${shader_source}
        
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${shader_source}
    )
    
    list(APPEND SHADERS ${CMAKE_CURRENT_BINARY_DIR}/${shader_source})
endforeach()

add_custom_target(FP_SHADERS ALL DEPENDS ${SHADERS})
