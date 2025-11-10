set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(lodePNG STATIC lodepng.cpp)
target_include_directories(lodePNG PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(lodePNG::lodePNG ALIAS lodePNG)

