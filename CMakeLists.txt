cmake_minimum_required(VERSION 3.29)
project(new_opengl-study)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(INC_DIR libs/include)
set(LINK_DIR libs/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

include(build)

add_subdirectory(common)
add_subdirectory(3d_brain_map)
add_subdirectory(3d_brain_map_test)
add_subdirectory(037_assimp_demo)
add_subdirectory(038_depth_test)
add_subdirectory(039_stencil_buffer)
add_subdirectory(040_blend)
add_subdirectory(041_face_culling)
add_subdirectory(042_frame_buffers)
add_subdirectory(love)