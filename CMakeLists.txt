cmake_minimum_required(VERSION 3.30)
project(new_opengl_study)

set(CMAKE_CXX_STANDARD 20)

set(INC_DIR libs/include)
set(LINK_DIR libs/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_executable(new_opengl_study src/glad.c src/model_loading.cpp src/stb_image.cpp)

#target_link_options(${PROJECT_NAME} PRIVATE /NODEFAULTLIB:MSVCRT)


target_link_libraries(${PROJECT_NAME}
        glfw3
        assimp-vc143-mt
)

