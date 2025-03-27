project(039_stencil_buffer)

my_add_target(${PROJECT_NAME} EXECUTABLE)

target_link_libraries(${PROJECT_NAME}
        glfw3
        common
)