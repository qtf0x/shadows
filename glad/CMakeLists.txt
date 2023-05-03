set(target glad)

set(glad_SOURCES
        src/glad.c)

add_library(${target} STATIC ${glad_SOURCES})

target_include_directories(${target} PUBLIC include)

if( UNIX AND NOT APPLE )
    target_link_libraries(${target} PUBLIC ${CMAKE_DL_LIBS})
endif()
