project(love)

# 查找CGAL包
find_package(CGAL REQUIRED)
# 查找Boost
find_package(Boost REQUIRED)
# 查找GMP和MPFR
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)

# 包含头文件路径
include_directories(${CGAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS} ${MPFR_INCLUDE_DIRS})

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(${PROJECT_NAME} ${srcs})


target_link_libraries(${PROJECT_NAME}
        legacy_stdio_definitions
        glfw3
#        opengl32
        common
        assimp-vc143-mt
        CGAL::CGAL ${Boost_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES}
)

if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_FLAGS /NODEFAULTLIB:MSVCRT.lib) # 禁用默认的msvc运行时库
endif()