set(target vmarias_FP)
set( FP_SOURCES
	src/ArcballCam.cpp
	src/Engine.cpp
	src/main.cpp
	src/ShaderProgram.cpp
	)

add_executable( ${target} ${FP_SOURCES} )

target_compile_definitions(${target}
		PRIVATE
		${DEFAULT_COMPILE_DEFINITIONS}
		GLFW_INCLUDE_NONE
		)

target_link_libraries( ${target}
		PRIVATE
		glad
		glfw
		${OPENGL_gl_LIBRARY}
		)

include_directories(include)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
