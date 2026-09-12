//
//  Vec3.hpp
//
//  Part of GrainLib
//  https://grain.one
//

#pragma once

#include <grain/String/String.hpp>
// #include <grain/String/CSVString.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <ostream>
#include <type_traits>

namespace Grain {

class File;

template <typename T>
struct Vec3 {
public:
    T x{};
    T y{};
    T z{};

public:
    // Construction
    constexpr Vec3() noexcept = default;

    constexpr Vec3(T x, T y, T z) noexcept
        : x(x), y(y), z(z) {}

    constexpr Vec3(const Vec3&) noexcept = default;
    constexpr Vec3(Vec3&&) noexcept = default;

    template <typename U>
        requires (!std::same_as<U, T>)
    explicit constexpr Vec3(const Vec3<U>& v) noexcept
        : x(static_cast<T>(v.x)),
          y(static_cast<T>(v.y)),
          z(static_cast<T>(v.z)) {}

    explicit Vec3(const char* csv, char delimiter = ',') noexcept {
        setByCSV(csv, delimiter);
    }

    explicit Vec3(const String& csv, char delimiter = ',') noexcept {
        setByCSV(csv, delimiter);
    }

    ~Vec3() noexcept = default;

    Vec3& operator=(const Vec3&) noexcept = default;
    Vec3& operator=(Vec3&&) noexcept = default;

    template <typename U>
        requires std::is_convertible_v<U, T>
    Vec3& operator=(const Vec3<U>& v) noexcept {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        return *this;
    }

    constexpr T& operator[](std::size_t index) noexcept {
        if (index == 0) {
            return x;
        }

        if (index == 1) {
            return y;
        }

        return z;
    }

    constexpr const T& operator[](std::size_t index) const noexcept {
        if (index == 0) {
            return x;
        }

        if (index == 1) {
            return y;
        }

        return z;
    }

    // Comparison
    [[nodiscard]] constexpr bool operator==(
        const Vec3&
    ) const noexcept = default;

    [[nodiscard]] constexpr bool operator!=(
        const Vec3& other
    ) const noexcept {
        return !(*this == other);
    }

    // Arithmetic
    [[nodiscard]] constexpr Vec3 operator-() const noexcept {
        return {-x, -y, -z};
    }

    [[nodiscard]] constexpr Vec3 operator+(
        const Vec3& other
    ) const noexcept {
        return {
            x + other.x,
            y + other.y,
            z + other.z
        };
    }

    [[nodiscard]] constexpr Vec3 operator-(
        const Vec3& other
    ) const noexcept {
        return {
            x - other.x,
            y - other.y,
            z - other.z
        };
    }

    [[nodiscard]] constexpr Vec3 operator*(
        const Vec3& other
    ) const noexcept {
        return {
            x * other.x,
            y * other.y,
            z * other.z
        };
    }

    [[nodiscard]] constexpr Vec3 operator/(
        const Vec3& other
    ) const noexcept {
        return {
            x / other.x,
            y / other.y,
            z / other.z
        };
    }

    [[nodiscard]] constexpr Vec3 operator*(
        T scalar
    ) const noexcept {
        return {
            x * scalar,
            y * scalar,
            z * scalar
        };
    }

    [[nodiscard]] constexpr Vec3 operator/(
        T scalar
    ) const noexcept {
        return {
            x / scalar,
            y / scalar,
            z / scalar
        };
    }

    constexpr Vec3& operator+=(
        const Vec3& other
    ) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr Vec3& operator-=(
        const Vec3& other
    ) noexcept {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr Vec3& operator*=(
        const Vec3& other
    ) noexcept {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    constexpr Vec3& operator/=(
        const Vec3& other
    ) noexcept {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    constexpr Vec3& operator*=(T scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr Vec3& operator/=(T scalar) noexcept {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // Geometry
    [[nodiscard]] double length() const noexcept {
        return std::hypot(
            static_cast<double>(x),
            static_cast<double>(y),
            static_cast<double>(z)
        );
    }

    [[nodiscard]] constexpr auto squaredLength() const noexcept {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] double distance(
        const Vec3& other
    ) const noexcept {
        return std::hypot(
            static_cast<double>(x - other.x),
            static_cast<double>(y - other.y),
            static_cast<double>(z - other.z)
        );
    }

    [[nodiscard]] constexpr auto squaredDistance(
        const Vec3& other
    ) const noexcept {
        const auto dx = x - other.x;
        const auto dy = y - other.y;
        const auto dz = z - other.z;

        return dx * dx + dy * dy + dz * dz;
    }

    [[nodiscard]] constexpr T dot(
        const Vec3& other
    ) const noexcept {
        return
            x * other.x +
            y * other.y +
            z * other.z;
    }

    /**
     * 3D cross product.
     *
     * Returns a vector perpendicular to this vector
     * and the other vector.
     */
    [[nodiscard]] constexpr Vec3 cross(
        const Vec3& other
    ) const noexcept {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    /**
     * Returns the angle to another vector in degrees.
     */
    [[nodiscard]] double angle(
        const Vec3& other
    ) const noexcept {
        const double denominator =
            length() * other.length();

        if (denominator == 0.0) {
            return 0.0;
        }

        double value =
            static_cast<double>(dot(other)) /
            denominator;

        value = std::clamp(value, -1.0, 1.0);

        return std::acos(value) *
               180.0 /
               std::numbers::pi;
    }

    [[nodiscard]] constexpr Vec3 flipped() const noexcept {
        return {-x, -y, -z};
    }

    [[nodiscard]] constexpr Vec3 reflectedPoint(
        const Vec3& pivot
    ) const noexcept {
        return pivot + (pivot - *this);
    }

    [[nodiscard]] Vec3 normalized() const noexcept {
        Vec3 result = *this;
        result.normalize();
        return result;
    }

    void normalize() noexcept {
        const double l = length();

        if (l == 0.0) {
            return;
        }

        const double scale = 1.0 / l;

        x = static_cast<T>(x * scale);
        y = static_cast<T>(y * scale);
        z = static_cast<T>(z * scale);
    }

    void setLength(T length) noexcept {
        const double current_length = this->length();

        if (current_length == 0.0) {
            return;
        }

        const double scale =
            static_cast<double>(length) /
            current_length;

        x = static_cast<T>(x * scale);
        y = static_cast<T>(y * scale);
        z = static_cast<T>(z * scale);
    }

    [[nodiscard]] bool checkEqual(
        const Vec3& other,
        T threshold
    ) const noexcept {
        return distance(other) <=
               static_cast<double>(threshold);
    }

    // Modification
    constexpr void set(
        T x,
        T y,
        T z
    ) noexcept {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    constexpr void zero() noexcept {
        x = T{};
        y = T{};
        z = T{};
    }

    constexpr void flip() noexcept {
        x = -x;
        y = -y;
        z = -z;
    }

    constexpr void translateX(T value) noexcept {
        x += value;
    }

    constexpr void translateY(T value) noexcept {
        y += value;
    }

    constexpr void translateZ(T value) noexcept {
        z += value;
    }

    constexpr void translate(
        T tx,
        T ty,
        T tz
    ) noexcept {
        x += tx;
        y += ty;
        z += tz;
    }

    constexpr void scaleX(T value) noexcept {
        x *= value;
    }

    constexpr void scaleY(T value) noexcept {
        y *= value;
    }

    constexpr void scaleZ(T value) noexcept {
        z *= value;
    }

    constexpr void scale(T value) noexcept {
        x *= value;
        y *= value;
        z *= value;
    }

    constexpr void scale(
        T sx,
        T sy,
        T sz
    ) noexcept {
        x *= sx;
        y *= sy;
        z *= sz;
    }

    void rotateXDegrees(double degrees) noexcept {
        rotateX(degrees * std::numbers::pi / 180.0);
    }

    void rotateX(double radians) noexcept {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double new_y =
            c * static_cast<double>(y) -
            s * static_cast<double>(z);

        const double new_z =
            s * static_cast<double>(y) +
            c * static_cast<double>(z);

        y = static_cast<T>(new_y);
        z = static_cast<T>(new_z);
    }

    void rotateYDegrees(double degrees) noexcept {
        rotateY(degrees * std::numbers::pi / 180.0);
    }

    void rotateY(double radians) noexcept {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double new_x =
            c * static_cast<double>(x) +
            s * static_cast<double>(z);

        const double new_z =
            -s * static_cast<double>(x) +
            c * static_cast<double>(z);

        x = static_cast<T>(new_x);
        z = static_cast<T>(new_z);
    }

    void rotateZDegrees(double degrees) noexcept {
        rotateZ(degrees * std::numbers::pi / 180.0);
    }

    void rotateZ(double radians) noexcept {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double new_x =
            c * static_cast<double>(x) -
            s * static_cast<double>(y);

        const double new_y =
            s * static_cast<double>(x) +
            c * static_cast<double>(y);

        x = static_cast<T>(new_x);
        y = static_cast<T>(new_y);
    }

    void setLerp(
        const Vec3& a,
        const Vec3& b,
        double t
    ) noexcept {
        x = static_cast<T>(
            static_cast<double>(a.x) +
            t * (
                static_cast<double>(b.x) -
                static_cast<double>(a.x)
            )
        );

        y = static_cast<T>(
            static_cast<double>(a.y) +
            t * (
                static_cast<double>(b.y) -
                static_cast<double>(a.y)
            )
        );

        z = static_cast<T>(
            static_cast<double>(a.z) +
            t * (
                static_cast<double>(b.z) -
                static_cast<double>(a.z)
            )
        );
    }

    [[nodiscard]] static Vec3 lerp(
        const Vec3& a,
        const Vec3& b,
        double t
    ) noexcept {
        Vec3 result;
        result.setLerp(a, b, t);
        return result;
    }

    // CSV
    bool setByCSV(
        const char* csv,
        char delimiter = ','
    ) noexcept {
        if (csv == nullptr) {
            return false;
        }

/*
        CSVLineParser parser(csv);
        parser.setDelimiter(delimiter);

        T values[3]{};
        const int32_t result = parser.values(3, values);

        if (result == 3) {
            x = values[0];
            y = values[1];
            z = values[2];
            return true;
        }
*/

        return false;
    }

    bool setByCSV(
        const String& csv,
        char delimiter = ','
    ) noexcept {
        return setByCSV(csv.utf8(), delimiter);
    }

    // Serialization
    void writeToFile(File& file);
    void readFromFile(File& file);
};

// Scalar multiplication from the left.
template <typename T>
[[nodiscard]] constexpr Vec3<T> operator*(
    T scalar,
    const Vec3<T>& vector
) noexcept {
    return vector * scalar;
}

// Stream output.
template <typename T>
std::ostream& operator<<(
    std::ostream& os,
    const Vec3<T>& vector
) {
    if constexpr (
        std::is_same_v<T, int8_t> ||
        std::is_same_v<T, uint8_t>
    ) {
        return os
            << static_cast<int32_t>(vector.x)
            << ", "
            << static_cast<int32_t>(vector.y)
            << ", "
            << static_cast<int32_t>(vector.z);
    }

    return os
        << vector.x
        << ", "
        << vector.y
        << ", "
        << vector.z;
}

// Standard types.
using Vec3i = Vec3<int32_t>;
using Vec3l = Vec3<int64_t>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

} // namespace Grain