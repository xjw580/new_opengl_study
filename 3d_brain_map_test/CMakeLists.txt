project(3d_brain_map_test)

# 查找CGAL包
find_package(CGAL REQUIRED)
# 查找Boost
find_package(Boost REQUIRED)
# 查找GMP和MPFR
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)

# 包含头文件路径
include_directories(${CGAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS} ${MPFR_INCLUDE_DIRS})

my_add_target(${PROJECT_NAME} EXECUTABLE)

target_link_libraries(${PROJECT_NAME}
        glfw3
        common
        assimp-vc143-mt
        CGAL::CGAL ${Boost_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES}
)