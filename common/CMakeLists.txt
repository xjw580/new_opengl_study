project(common VERSION 0.1 LANGUAGES CXX)

my_add_target(${PROJECT_NAME} STATIC)

#    添加头文件搜索路径，暴露给其他模块使用
target_include_directories(common PUBLIC src)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw3
)