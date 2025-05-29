project(038_depth_test)

my_add_target(${PROJECT_NAME} EXECUTABLE)

target_link_libraries(${PROJECT_NAME}
        glfw3
        glad
        stb_image
)