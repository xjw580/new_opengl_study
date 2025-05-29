project(imgui)

my_add_target(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME}
        legacy_stdio_definitions
        glfw3
        common
)