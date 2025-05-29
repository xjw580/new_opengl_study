project(043_love_demo)

my_add_target(${PROJECT_NAME} EXECUTABLE)

my_copy_assimp_dll()

target_link_libraries(${PROJECT_NAME}
        glfw3
        glad
        stb_image
        assimp-vc143-mt
        imgui
        legacy_stdio_definitions
        pdh
)