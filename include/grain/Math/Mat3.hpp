#pragma once

#include <grain/Math/Scalar.hpp>
#include <grain/Math/Vec2.hpp>
#include <grain/Math/Vec3.hpp>

#include <numbers>
#include <cmath>
#include <cstddef>
#include <array>

namespace Grain {

template <ScalarType T>
class Mat3 {
public:
    constexpr Mat3()
        : data_{
            std::array<T, 3>{T{1}, T{0}, T{0}},
            std::array<T, 3>{T{0}, T{1}, T{0}},
            std::array<T, 3>{T{0}, T{0}, T{1}}
        }
    {
    }


    constexpr Mat3(
        T m00, T m01, T m02,
        T m10, T m11, T m12,
        T m20, T m21, T m22
    )
        : data_{
            std::array<T, 3>{m00, m01, m02},
            std::array<T, 3>{m10, m11, m12},
            std::array<T, 3>{m20, m21, m22}
        }
    {
    }

    [[nodiscard]] static constexpr auto identity() -> Mat3 {
        return {};
    }

    [[nodiscard]] static constexpr auto translation(T x, T y) -> Mat3 {
        return {
            T(1), T(0), x,
            T(0), T(1), y,
            T(0), T(0), T(1)
        };
    }

    [[nodiscard]] static constexpr auto scaling(T x, T y) -> Mat3 {
        return {
            x,    T(0), T(0),
            T(0), y,    T(0),
            T(0), T(0), T(1)
        };
    }

    [[nodiscard]] static constexpr auto scaling(T scale) -> Mat3 {
        return scaling(scale, scale);
    }

    [[nodiscard]] static auto rotation(T radians) -> Mat3 {
        const T c = std::cos(radians);
        const T s = std::sin(radians);

        return {
            c, -s, T(0),
            s,  c, T(0),
            T(0), T(0), T(1)
        };
    }

    static auto rotationDegrees(T degrees) -> Mat3 {
        const T radians = degrees * std::numbers::pi / 180.0;
        const T c = std::cos(radians);
        const T s = std::sin(radians);

        return {
            c, -s, T(0),
            s,  c, T(0),
            T(0), T(0), T(1)
        };
    }

    constexpr auto operator () (std::size_t row, std::size_t column) -> T& {
        return data_[row][column];
    }

    constexpr auto operator () (std::size_t row, std::size_t column) const -> T {
        return data_[row][column];
    }

    constexpr auto data() -> T* {
        return &data_[0][0];
    }

    constexpr auto data() const -> const T* {
        return &data_[0][0];
    }

    constexpr auto operator *= (const Mat3& other) -> Mat3& {
        *this = *this * other;
        return *this;
    }

    [[nodiscard]] constexpr auto operator * (const Mat3& other) const -> Mat3 {
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

    [[nodiscard]] constexpr auto transform(const Vec2<T>& v) const -> Vec2<T> {
        return {
            data_[0][0] * v.x +
            data_[0][1] * v.y +
            data_[0][2],

            data_[1][0] * v.x +
            data_[1][1] * v.y +
            data_[1][2]
        };
    }

    [[nodiscard]] constexpr auto transform(const Vec3<T>& v) const -> Vec3<T> {
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

    [[nodiscard]]constexpr auto determinant() const -> T {
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

    constexpr void transpose() {
        std::swap(data_[0][1], data_[1][0]);
        std::swap(data_[0][2], data_[2][0]);
        std::swap(data_[1][2], data_[2][1]);
    }

    [[nodiscard]] constexpr auto transposed() const -> Mat3 {
        Mat3 result = *this;
        result.transpose();
        return result;
    }

private:
    std::array<std::array<T, 3>, 3> data_{};
};

using Mat3f = Mat3<float>;
using Mat3d = Mat3<double>;

} // namespace Grain