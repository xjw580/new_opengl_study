project(love)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(${PROJECT_NAME} ${srcs})

target_link_libraries(${PROJECT_NAME}
        legacy_stdio_definitions
        glfw3
        common
        assimp-vc143-mt
        pdh
)