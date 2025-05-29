macro (my_copy_assimp_dll)

    # 设置输出目录
    set(EXECUTABLE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

    message(CMAKE_SOURCE_DIR:${CMAKE_SOURCE_DIR})
    message(EXECUTABLE_OUTPUT_DIR:${EXECUTABLE_OUTPUT_DIR})

    # 2. 定义 DLL 源路径和目标路径
    set(DLL_SOURCE "${CMAKE_SOURCE_DIR}/libs/lib/assimp-vc143-mt.dll")
    set(DLL_DEST "${EXECUTABLE_OUTPUT_DIR}/assimp-vc143-mt.dll")

    # 3. 添加自定义命令，在构建后拷贝 DLL
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_SOURCE}"
            "${DLL_DEST}"
            COMMENT "Copying assimp-vc143-mt.dll to executable directory: ${DLL_DEST}"
    )
endmacro()

macro (my_add_target name type)
    file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp src/*.c include/*.h)
    if ("${type}" MATCHES "EXECUTABLE")
        add_executable(${name} ${srcs})
    else()
        add_library(${name} ${type} ${srcs})
        target_include_directories(${name} PUBLIC include)
    endif()
endmacro()