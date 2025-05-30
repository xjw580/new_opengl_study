project(imgui)

my_add_target(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME}
        legacy_stdio_definitions #为旧版本（legacy）的 C 标准库符号（如 _imp___iob）提供兼容支持
        glfw3
)