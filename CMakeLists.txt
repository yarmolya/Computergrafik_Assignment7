cmake_minimum_required(VERSION 3.14)

project(ShadowViewer
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # create compile_commands.json, useful for IDE auto-complete systems

# set output paths
set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

add_subdirectory(lib/glew)
add_subdirectory(lib/glfw)
add_subdirectory(lib/lodePNG)

# add source directory to include path


# build source directory
add_subdirectory(src)
