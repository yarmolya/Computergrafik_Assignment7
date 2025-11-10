# Windows stuff
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions("-D_WIN32 -DWIN32 -D_USE_MATH_DEFINES -DNOMINMAX -DWIN32_LEAN_AND_MEAN")
endif()


add_library(glew STATIC glew.cpp)
target_compile_definitions(glew PUBLIC "-DGLEW_STATIC")
target_compile_definitions(glew PUBLIC "-DGLEW_NO_GLU")
target_include_directories(glew PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(glew OpenGL::GL)

add_library(glew::glew ALIAS glew)
