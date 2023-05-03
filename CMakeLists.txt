cmake_minimum_required (VERSION 3.25)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++17 -g -m64 -O3 -fsanitize=address")
set(CXX_STANDARD_REQUIRED ON)

set (glm_DIR Z:/GLM/lib/cmake/glm)
set (glfw3_DIR Z:/GLFW/lib/cmake/glfw3)

project (shadows)

if(MSVC)
  add_definitions(/DNOMINMAX)
endif()

if(UNIX)
    message(STATUS "Running Linux compiling. Options [GLVND] for modern OpenGL or [LEGACY] for historical OpenGl.")
    message(STATUS "Setting GLVND option.")
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package( glm CONFIG REQUIRED )
find_package( glfw3 CONFIG REQUIRED )
find_package( OpenGL REQUIRED )

include_directories( glad )

add_subdirectory( glad )
add_subdirectory( FP )
add_subdirectory( FP/shaders )
