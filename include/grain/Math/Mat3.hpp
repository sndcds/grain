#pragma once

#include <grain/Math/Scalar.hpp>
#include <grain/Math/Vec2.hpp>
#include <grain/Math/Vec3.hpp>

#include <numbers>
#include <cmath>
#include <cstddef>
#include <limits>

namespace Grain {

template <ScalarType T>
class Mat3 {
public:
    constexpr Mat3() noexcept
        : data_{
            {T(1), T(0), T(0)},
            {T(0), T(1), T(0)},
            {T(0), T(0), T(1)}
        }
    {
    }

    constexpr Mat3(
        T m00, T m01, T m02,
        T m10, T m11, T m12,
        T m20, T m21, T m22
    ) noexcept
        : data_{
            {m00, m01, m02},
            {m10, m11, m12},
            {m20, m21, m22}
        }
    {
    }

    [[nodiscard]]
    static constexpr Mat3 identity() noexcept {
        return {};
    }

    [[nodiscard]]
    static constexpr Mat3 translation(T x, T y) noexcept {
        return {
            T(1), T(0), x,
            T(0), T(1), y,
            T(0), T(0), T(1)
        };
    }

    [[nodiscard]]
    static constexpr Mat3 scaling(T x, T y) noexcept {
        return {
            x,    T(0), T(0),
            T(0), y,    T(0),
            T(0), T(0), T(1)
        };
    }

    [[nodiscard]]
    static constexpr Mat3 scaling(T scale) noexcept {
        return scaling(scale, scale);
    }

    [[nodiscard]]
    static Mat3 rotation(T radians) noexcept {
        const T c = std::cos(radians);
        const T s = std::sin(radians);

        return {
            c, -s, T(0),
            s,  c, T(0),
            T(0), T(0), T(1)
        };
    }

    static Mat3 rotationDegrees(T degrees) {
        const T radians = degrees * std::numbers::pi / 180.0;
        const T c = std::cos(radians);
        const T s = std::sin(radians);

        return {
            c, -s, T(0),
            s,  c, T(0),
            T(0), T(0), T(1)
        };
    }

    constexpr T& operator()(std::size_t row, std::size_t column) noexcept {
        return data_[row][column];
    }

    constexpr T operator()(std::size_t row, std::size_t column) const noexcept {
        return data_[row][column];
    }

    constexpr T* data() noexcept {
        return &data_[0][0];
    }

    constexpr const T* data() const noexcept {
        return &data_[0][0];
    }

    constexpr Mat3& operator*=(const Mat3& other) noexcept {
        *this = *this * other;
        return *this;
    }

    [[nodiscard]]
    constexpr Mat3 operator*(const Mat3& other) const noexcept {
        Mat3 result{};

        for (std::size_t row = 0; row < 3; ++row) {
            for (std::size_t column = 0; column < 3; ++column) {
                result(row, column) =
                    (*this)(row, 0) * other(0, column) +
                    (*this)(row, 1) * other(1, column) +
                    (*this)(row, 2) * other(2, column);
            }
        }

        return result;
    }

    [[nodiscard]]
    constexpr Vec2<T> transform(const Vec2<T>& v) const noexcept {
        return {
            data_[0][0] * v.x +
            data_[0][1] * v.y +
            data_[0][2],

            data_[1][0] * v.x +
            data_[1][1] * v.y +
            data_[1][2]
        };
    }

    [[nodiscard]]
    constexpr Vec3<T> transform(const Vec3<T>& v) const noexcept {
        return {
            data_[0][0] * v.x +
            data_[0][1] * v.y +
            data_[0][2] * v.z,

            data_[1][0] * v.x +
            data_[1][1] * v.y +
            data_[1][2] * v.z,

            data_[2][0] * v.x +
            data_[2][1] * v.y +
            data_[2][2] * v.z
        };
    }

    [[nodiscard]]
    constexpr T determinant() const noexcept {
        return
            data_[0][0] * (
                data_[1][1] * data_[2][2] -
                data_[1][2] * data_[2][1]
            ) -
            data_[0][1] * (
                data_[1][0] * data_[2][2] -
                data_[1][2] * data_[2][0]
            ) +
            data_[0][2] * (
                data_[1][0] * data_[2][1] -
                data_[1][1] * data_[2][0]
            );
    }

    constexpr void transpose() noexcept {
        std::swap(data_[0][1], data_[1][0]);
        std::swap(data_[0][2], data_[2][0]);
        std::swap(data_[1][2], data_[2][1]);
    }

    [[nodiscard]]
    constexpr Mat3 transposed() const noexcept {
        Mat3 result = *this;
        result.transpose();
        return result;
    }

private:
    T data_[3][3];
};

using Mat3f = Mat3<float>;
using Mat3d = Mat3<double>;

} // namespace Grain