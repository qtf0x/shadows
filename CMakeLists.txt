cmake_minimum_required (VERSION 3.25)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17 -m64 -O3")
set(CXX_STANDARD_REQUIRED ON)

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
