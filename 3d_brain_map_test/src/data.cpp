/**************************************************************************

Author:肖嘉威

Version:1.0.0

Date:2025/3/27 14:37

Description:

**************************************************************************/

#include "data.h"

#include <iostream>
#include <ostream>

#define SOURCE_LENGTH 4.0
#define SOURCE_AXIS_LENGTH (SOURCE_LENGTH/2.0)

void loadElectrode(
    std::vector<BrainCommon::Electrode> &electrodes,
    const int xLength, const int yLength, const int zLength
) {
    electrodes.clear();
    electrodes.push_back({
        "Fp1",
        static_cast<int>((-0.485328 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((-1.493835 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((1.693835 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        3.731913
    });

    electrodes.push_back({
        "Fp2",
        static_cast<int>((0.485501 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((-1.493884 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((1.693835 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        6.841336
    });

    electrodes.push_back({
        "C3",
        static_cast<int>((-0.785398 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((-0.000111 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((-1.000111 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        5.464287
    });

    electrodes.push_back({
        "C4",
        static_cast<int>((0.785398 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((-0.000111 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((-1.000111 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        7.650035
    });

    electrodes.push_back({
        "T3",
        static_cast<int>((-1.570796 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((0.000000 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((-1.09000 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        6.819226
    });

    electrodes.push_back({
        "T4",
        static_cast<int>((1.570796 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((0.000000 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((-1.090000 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        9.947087
    });

    electrodes.push_back({
        "Cz",
        static_cast<int>((0.000000 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((0.000200 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((0.000200 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        27.285629
    });

    electrodes.push_back({
        "O1",
        static_cast<int>((-0.485359 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((1.493930 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((1.493930 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        5.909139
    });

    electrodes.push_back({
        "O2",
        static_cast<int>((0.485359 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
        static_cast<int>((1.493930 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
        static_cast<int>((1.493930 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH),
        12.577266
    });

    std::cout << "electrode size: " << electrodes.size() << std::endl;
}


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_hierarchy_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <vector>
#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;

// 为顶点添加额外信息(如值)
struct Vertex_info {
    float value;

    Vertex_info() : value(0.0) {
    }

    Vertex_info(float v) : value(v) {
    }
};

// 顶点基类定义
typedef CGAL::Triangulation_vertex_base_with_info_3<Vertex_info, K> Vb;
typedef CGAL::Triangulation_hierarchy_vertex_base_3<Vb> Vbh;
typedef CGAL::Triangulation_data_structure_3<Vbh> Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds> Dt;
typedef CGAL::Triangulation_hierarchy_3<Dt> Triangulation;

class NearestNeighborInterpolator {
private:
    Triangulation tr;

public:
    // 插入带有值的点
    void insert_point(const Point_3 &p, float value) {
        // 创建顶点并设置信息
        auto vh = tr.insert(p);
        vh->info() = Vertex_info(value);
    }

    // 查询点的插值值
    float interpolate(const Point_3 &query) const {
        if (tr.number_of_vertices() == 0) {
            throw std::runtime_error("No points in the triangulation");
        }

        // 找到最近的顶点
        Triangulation::Vertex_handle vh = tr.nearest_vertex(query);
        return vh->info().value;
    }

    // 批量查询
    std::vector<float> interpolate(const std::vector<Point_3> &queries) const {
        std::vector<float> results;
        results.reserve(queries.size());
        for (const auto &q: queries) {
            results.push_back(interpolate(q));
        }
        return results;
    }
};

int test(std::vector<float> &values, const int xLength, const int yLength, const int zLength) {
    NearestNeighborInterpolator interpolator;

    // 添加一些样本点及其值
    interpolator.insert_point(
        Point_3(static_cast<int>((-0.485328 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((-1.493835 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((1.693835 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        3.731913
    );

    interpolator.insert_point(
        Point_3(static_cast<int>((0.485501 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((-1.493884 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((1.693835 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        6.841336
    );

    interpolator.insert_point(
        Point_3(static_cast<int>((-0.785398 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((-0.000111 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((-1.000111 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        5.464287
    );

    interpolator.insert_point(
        Point_3(static_cast<int>((0.785398 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((-0.000111 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((-1.000111 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        7.650035
    );

    interpolator.insert_point(
        Point_3(static_cast<int>((-1.570796 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((0.000000 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((-1.09000 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        6.819226
    );

    interpolator.insert_point(
        Point_3(static_cast<int>((1.570796 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((0.000000 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((-1.090000 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        9.947087
    );

    interpolator.insert_point(
        Point_3(static_cast<int>((0.000000 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((0.000200 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((0.000200 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        27.285629
    );

    interpolator.insert_point(
        Point_3(static_cast<int>((-0.485359 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((1.493930 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((1.493930 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        5.909139
    );

    interpolator.insert_point(
        Point_3(static_cast<int>((0.485359 + SOURCE_AXIS_LENGTH) * xLength / SOURCE_LENGTH),
                static_cast<int>((1.493930 + SOURCE_AXIS_LENGTH) * yLength / SOURCE_LENGTH),
                static_cast<int>((1.493930 + SOURCE_AXIS_LENGTH) * zLength / SOURCE_LENGTH)),
        12.577266
    );

    auto start = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    values.clear();
    values.resize(xLength * yLength * zLength);
    for (int y = 0; y < yLength; y++) {
        for (int z = 0; z < zLength; z++) {
            for (int x = 0; x < xLength; x++) {
                auto nearestVertex = interpolator.interpolate(Point_3(x, y, z));
                values[x + y * xLength + z * xLength * yLength] = nearestVertex;
            }
        }
    }
    auto end = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    std::cout << "calc time:" << (end - start).count() << "ms" << std::endl;
    return 0;
}
#undef SOURCE_LENGTH
#undef SOURCE_AXIS_LENGTH
