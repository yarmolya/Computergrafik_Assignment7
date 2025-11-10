# OpenGL & GLEW library
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

# Make sure the textures, shaders and meshes are available
set(SHADER_PATH   ${CMAKE_SOURCE_DIR}/shaders      CACHE PATH "location of shader programs")
set(TEXTURE_PATH  ${CMAKE_SOURCE_DIR}/textures     CACHE PATH "location of texture images")
set(MESH_PATH     ${CMAKE_SOURCE_DIR}/meshes       CACHE PATH "location of example meshes")
add_definitions("-DSHADER_PATH=\"${SHADER_PATH}\"")
add_definitions("-DMESH_PATH=\"${MESH_PATH}\"")
add_definitions("-DTEXTURE_PATH=\"${TEXTURE_PATH}\"")

add_executable(ShadowViewer glfw_window.cpp glmath.cpp main.cpp shader.cpp ShadowViewer.cpp Mesh.cpp sphere.cpp dumpOpenGLBuffer.cpp CubeMapVisualization.cpp Quad.cpp)

target_link_libraries(ShadowViewer glfw lodePNG::lodePNG glew::glew OpenGL::GL)
target_include_directories(ShadowViewer SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(ShadowViewer PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_USE_MATH_DEFINES>)
target_compile_definitions(ShadowViewer PRIVATE $<$<CXX_COMPILER_ID:MSVC>:WIN32_LEAN_AND_MEAN>)
target_compile_definitions(ShadowViewer PRIVATE $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>)
