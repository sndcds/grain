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

    // Construction
    constexpr Vec3() = default;

    constexpr Vec3(T x, T y, T z)
        : x(x), y(y), z(z) {}

    constexpr Vec3(const Vec3&) = default;
    constexpr Vec3(Vec3&&) = default;

    template <typename U>
        requires (!std::same_as<U, T>)
    explicit constexpr Vec3(const Vec3<U>& v)
        : x(static_cast<T>(v.x)),
          y(static_cast<T>(v.y)),
          z(static_cast<T>(v.z)) {}

    explicit Vec3(const char* csv, char delimiter = ',') {
        setByCSV(csv, delimiter);
    }

    explicit Vec3(const String& csv, char delimiter = ',') {
        setByCSV(csv, delimiter);
    }

    ~Vec3() = default;

    auto operator = (const Vec3&) -> Vec3& = default;
    auto operator = (Vec3&&) -> Vec3& = default;

    template <typename U>
        requires std::is_convertible_v<U, T>
    auto operator = (const Vec3<U>& v) -> Vec3& {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        return *this;
    }

    constexpr auto operator [] (std::size_t index) -> T& {
        if (index == 0) {
            return x;
        }

        if (index == 1) {
            return y;
        }

        return z;
    }

    constexpr auto operator [] (std::size_t index) const -> const T& {
        if (index == 0) {
            return x;
        }

        if (index == 1) {
            return y;
        }

        return z;
    }

    // Comparison
    [[nodiscard]] constexpr auto operator == (const Vec3&) const -> bool = default;

    [[nodiscard]] constexpr auto operator != (const Vec3& other) const -> bool {
        return !(*this == other);
    }

    // Arithmetic
    [[nodiscard]] constexpr auto operator - () const -> Vec3 {
        return {-x, -y, -z};
    }

    [[nodiscard]] constexpr auto operator + (const Vec3& other) const -> Vec3 {
        return {x + other.x, y + other.y, z + other.z};
    }

    [[nodiscard]] constexpr auto operator - (const Vec3& other) const -> Vec3 {
        return {x - other.x, y - other.y, z - other.z};
    }

    [[nodiscard]] constexpr auto operator * (const Vec3& other) const -> Vec3 {
        return {x * other.x, y * other.y, z * other.z};
    }

    [[nodiscard]] constexpr auto operator / (const Vec3& other) const -> Vec3 {
        return {x / other.x, y / other.y, z / other.z};
    }

    [[nodiscard]] constexpr auto operator * (T scalar) const -> Vec3 {
        return {x * scalar, y * scalar, z * scalar};
    }

    [[nodiscard]] constexpr auto operator / (T scalar) const -> Vec3 {
        return {x / scalar, y / scalar, z / scalar};
    }

    constexpr auto operator += (const Vec3& other) -> Vec3& {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr auto operator -= (const Vec3& other) -> Vec3& {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr auto operator *= (const Vec3& other) -> Vec3& {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    constexpr auto operator /= (const Vec3& other) -> Vec3& {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    constexpr auto operator *= (T scalar) -> Vec3& {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr auto operator /= (T scalar) -> Vec3& {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // Geometry
    [[nodiscard]] auto length() const -> double {
        return std::hypot(
            static_cast<double>(x),
            static_cast<double>(y),
            static_cast<double>(z)
        );
    }

    [[nodiscard]] constexpr auto squaredLength() const {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] auto distance(const Vec3& other) const -> double {
        return std::hypot(
            static_cast<double>(x - other.x),
            static_cast<double>(y - other.y),
            static_cast<double>(z - other.z)
        );
    }

    [[nodiscard]] constexpr auto squaredDistance(const Vec3& other) const {
        const auto dx = x - other.x;
        const auto dy = y - other.y;
        const auto dz = z - other.z;

        return dx * dx + dy * dy + dz * dz;
    }

    [[nodiscard]] constexpr auto dot(const Vec3& other) const -> T {
        return x * other.x + y * other.y + z * other.z;
    }

    /**
     * 3D cross product.
     *
     * Returns a vector perpendicular to this vector
     * and the other vector.
     */
    [[nodiscard]] constexpr auto cross(const Vec3& other) const -> Vec3 {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    /**
     * Returns the angle to another vector in degrees.
     */
    [[nodiscard]] auto angle(const Vec3& other) const -> double {
        const double denominator = length() * other.length();
        if (denominator == 0.0) {
            return 0.0;
        }

        double value = static_cast<double>(dot(other)) / denominator;
        value = std::clamp(value, -1.0, 1.0);

        return std::acos(value) * 180.0 / std::numbers::pi;
    }

    [[nodiscard]] constexpr auto flipped() const -> Vec3 {
        return {-x, -y, -z};
    }

    [[nodiscard]] constexpr auto reflectedPoint(const Vec3& pivot) const -> Vec3 {
        return pivot + (pivot - *this);
    }

    [[nodiscard]] auto normalized() const -> Vec3 {
        Vec3 result = *this;
        result.normalize();
        return result;
    }

    void normalize() {
        const double l = length();
        if (l == 0.0) {
            return;
        }

        const double scale = 1.0 / l;

        x = static_cast<T>(x * scale);
        y = static_cast<T>(y * scale);
        z = static_cast<T>(z * scale);
    }

    void setLength(T length) {
        const double current_length = this->length();
        if (current_length == 0.0) {
            return;
        }

        const double scale = static_cast<double>(length) / current_length;

        x = static_cast<T>(x * scale);
        y = static_cast<T>(y * scale);
        z = static_cast<T>(z * scale);
    }

    [[nodiscard]] auto checkEqual(const Vec3& other, T threshold) const -> bool {
        return distance(other) <= static_cast<double>(threshold);
    }

    // Modification
    constexpr void set(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    constexpr void zero() {
        x = T{};
        y = T{};
        z = T{};
    }

    constexpr void flip() {
        x = -x;
        y = -y;
        z = -z;
    }

    constexpr void translateX(T value) {
        x += value;
    }

    constexpr void translateY(T value) {
        y += value;
    }

    constexpr void translateZ(T value) {
        z += value;
    }

    constexpr void translate(T tx, T ty, T tz) {
        x += tx;
        y += ty;
        z += tz;
    }

    constexpr void scaleX(T value) {
        x *= value;
    }

    constexpr void scaleY(T value) {
        y *= value;
    }

    constexpr void scaleZ(T value) {
        z *= value;
    }

    constexpr void scale(T value) {
        x *= value;
        y *= value;
        z *= value;
    }

    constexpr void scale(T sx, T sy, T sz) {
        x *= sx;
        y *= sy;
        z *= sz;
    }

    void rotateXDegrees(double degrees) {
        rotateX(degrees * std::numbers::pi / 180.0);
    }

    void rotateX(double radians) {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double new_y =
            (c * static_cast<double>(y)) -
            (s * static_cast<double>(z));

        const double new_z =
            (s * static_cast<double>(y)) +
            (c * static_cast<double>(z));

        y = static_cast<T>(new_y);
        z = static_cast<T>(new_z);
    }

    void rotateYDegrees(double degrees) {
        rotateY(degrees * std::numbers::pi / 180.0);
    }

    void rotateY(double radians) {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double new_x =
            (c * static_cast<double>(x)) +
            (s * static_cast<double>(z));

        const double new_z =
            (-s * static_cast<double>(x)) +
            (c * static_cast<double>(z));

        x = static_cast<T>(new_x);
        z = static_cast<T>(new_z);
    }

    void rotateZDegrees(double degrees) {
        rotateZ(degrees * std::numbers::pi / 180.0);
    }

    void rotateZ(double radians) {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double new_x =
            (c * static_cast<double>(x)) -
            (s * static_cast<double>(y));

        const double new_y =
            (s * static_cast<double>(x)) +
            (c * static_cast<double>(y));

        x = static_cast<T>(new_x);
        y = static_cast<T>(new_y);
    }

    void setLerp(const Vec3& a, const Vec3& b, double t) {
        x = static_cast<T>(
            static_cast<double>(a.x) +
            (t * (
                static_cast<double>(b.x) -
                static_cast<double>(a.x)
            ))
        );

        y = static_cast<T>(
            static_cast<double>(a.y) +
            (t * (
                static_cast<double>(b.y) -
                static_cast<double>(a.y)
            ))
        );

        z = static_cast<T>(
            static_cast<double>(a.z) +
            (t * (
                static_cast<double>(b.z) -
                static_cast<double>(a.z)
            ))
        );
    }

    [[nodiscard]] static auto lerp(const Vec3& a, const Vec3& b, double t) -> Vec3 {
        Vec3 result;
        result.setLerp(a, b, t);
        return result;
    }

    // CSV
    auto setByCSV(const char* csv, char delimiter = ',') -> bool {
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

    auto setByCSV(const String& csv, char delimiter = ',') -> bool {
        return setByCSV(csv.utf8(), delimiter);
    }

    // Serialization
    void writeToFile(File& file);
    void readFromFile(File& file);
};

// Scalar multiplication from the left.
template <typename T>
[[nodiscard]] constexpr auto operator * (T scalar, const Vec3<T>& vector) -> Vec3<T> {
    return vector * scalar;
}

// Stream output.
template <typename T>
auto operator << (std::ostream& os, const Vec3<T>& vector) -> std::ostream& {
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