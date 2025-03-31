#pragma once

// 3D模糊函数类型
using blurFunc3D = std::function<void(std::vector<double>&, std::vector<double>&, int, int, int)>;

// 整数半径的3D模糊函数生成器
inline blurFunc3D bluri3D(const double radius) {
    const double w = 2 * radius + 1;

    return [=](std::vector<double> &T, std::vector<double> &S, int start, int stop, int step)-> void {
        if ((stop -= step) < start) return;
        double sum = radius * S[start];
        const double s = step * radius;
        const auto stop_d = static_cast<double>(stop);
        const auto start_d = static_cast<double>(start);
        for (int i = start, j = start + static_cast<int>(s); i < j; i += step) {
            sum += S[std::min(stop, i)];
        }
        for (int i = start, j = stop; i <= j; i += step) {
            sum += S[static_cast<int>(std::min(stop_d, i + s))];
            T[i] = sum / w;
            sum -= S[static_cast<int>(std::max(start_d, i - s))];
        }
    };
}

// 浮点半径的3D模糊函数生成器
inline blurFunc3D blurf3D(double radius) {
    double radius0 = std::floor(radius);
    if (radius0 == radius) return bluri3D(radius);
    const double t = radius - radius0;
    const double w = 2 * radius + 1;

    return [=](std::vector<double> &T, std::vector<double> &S, int start, int stop, int step)-> void {
        if (!((stop -= step) >= start)) return;
        double sum = radius0 * S[start];
        const double s0 = step * radius0;
        const double s1 = s0 + step;
        const auto stop_d = static_cast<double>(stop);
        const auto start_d = static_cast<double>(start);
        for (int i = start, j = start + static_cast<int>(s0); i < j; i += step) {
            sum += S[std::min(stop, i)];
        }
        for (int i = start, j = stop; i <= j; i += step) {
            sum += S[static_cast<int>(std::min(stop_d, i + s0))];
            T[i] = (sum + t * (S[static_cast<int>(std::max(start_d, i - s1))] + S[static_cast<int>(std::min(
                                   stop_d, i + s1))])) / w;
            sum -= S[static_cast<int>(std::max(start_d, i - s0))];
        }
    };
}

// X方向模糊
inline void blurx3D(const blurFunc3D &blur_func, std::vector<double> &T, std::vector<double> &S,
                    const int width, const int height, const int depth) {
    const int slice = width * height;
    for (int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            int start = z * slice + y * width;
            int stop = start + width;
            blur_func(T, S, start, stop, 1);
        }
    }
}

// Y方向模糊
inline void blury3D(const blurFunc3D &blur_func, std::vector<double> &T, std::vector<double> &S,
                    const int width, const int height, const int depth) {
    const int slice = width * height;
    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            int start = z * slice + x;
            int stop = start + slice;
            blur_func(T, S, start, stop, width);
        }
    }
}

// Z方向模糊
inline void blurz3D(const blurFunc3D &blur_func, std::vector<double> &T, std::vector<double> &S,
                    const int width, const int height, const int depth) {
    const int slice = width * height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int start = y * width + x;
            int stop = start + slice * depth;
            blur_func(T, S, start, stop, slice);
        }
    }
}

// 完整的3D模糊函数
inline void blur3D(std::vector<double> &values, const int width, const int height, const int depth, double radius, int iterations = 1) {
    // 创建临时数组
    std::vector<double> tempValues(values.size());

    // 创建模糊函数
    const auto blurx = blurf3D(radius);
    const auto blury = blurf3D(radius);
    const auto blurz = blurf3D(radius);

    // 应用多次迭代
    for (int iter = 0; iter < iterations; iter++) {
        // X方向模糊
        blurx3D(blurx, tempValues, values, width, height, depth);

        // Y方向模糊
        blury3D(blury, values, tempValues, width, height, depth);

        // Z方向模糊
        blurz3D(blurz, tempValues, values, width, height, depth);

        // 确保最终结果在values中
        if (iter < iterations - 1) {
            values = tempValues;
        } else {
            // 最后一次迭代后，将结果从tempValues复制到values
            std::copy(tempValues.begin(), tempValues.end(), values.begin());
        }
    }
}

// 此函数用于从vertices提取并应用模糊到v值
inline void blurVertices(std::vector<GLfloat>& vertices, int width, int height, int depth, double radius, int iterations = 1) {
    // 提取v值
    std::vector<double> values(width * height * depth);
    for (int i = 0; i < width * height * depth; i++) {
        values[i] = static_cast<double>(vertices[i * 4 + 3]);
    }

    // 应用3D模糊
    blur3D(values, width, height, depth, radius, iterations);

    // 更新vertices中的v值
    for (int i = 0; i < width * height * depth; i++) {
        vertices[i * 4 + 3] = static_cast<GLfloat>(values[i]);
    }
}
