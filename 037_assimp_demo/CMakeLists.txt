project(037_assimp_demo)

my_add_target(${PROJECT_NAME} EXECUTABLE)

target_link_libraries(${PROJECT_NAME}
        glfw3
        glad
        stb_image
        assimp-vc143-mt
)

my_copy_assimp_dll()