//
//  Vec2.hpp
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
struct Vec2 {
public:
    T x{};
    T y{};

    // Construction
    constexpr Vec2() = default;
    constexpr Vec2(T x, T y)
        : x(x), y(y) {}

    constexpr Vec2(const Vec2&) = default;
    constexpr Vec2(Vec2&&) = default;

    template <typename U>
        requires (!std::same_as<U, T>)
    explicit constexpr Vec2(const Vec2<U>& v)
        : x(static_cast<T>(v.x)),
          y(static_cast<T>(v.y)) {}

    explicit Vec2(const char* csv, char delimiter = ',') {
        setByCSV(csv, delimiter);
    }

    explicit Vec2(const String& csv, char delimiter = ',') {
        setByCSV(csv, delimiter);
    }

    ~Vec2() = default;

    auto operator = (const Vec2&) -> Vec2& = default;
    auto operator = (Vec2&&) -> Vec2& = default;

    template <typename U>
        requires std::is_convertible_v<U, T>
    auto operator = (const Vec2<U>& v) -> Vec2& {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        return *this;
    }


    constexpr auto operator [] (std::size_t index) -> T& {
        return index == 0 ? x : y;
    }

    constexpr auto operator [] (std::size_t index) const -> const T& {
        return index == 0 ? x : y;
    }

    // Comparison
    [[nodiscard]] constexpr auto operator == (const Vec2&) const -> bool = default;

    [[nodiscard]] constexpr auto operator != (const Vec2& other) const -> bool {
        return !(*this == other);
    }

    // Arithmetic
    [[nodiscard]] constexpr auto operator - () const -> Vec2 {
        return {-x, -y};
    }

    [[nodiscard]] constexpr auto operator + (const Vec2& other) const -> Vec2 {
        return {x + other.x, y + other.y};
    }

    [[nodiscard]] constexpr auto operator - (const Vec2& other) const -> Vec2 {
        return {x - other.x, y - other.y};
    }

    [[nodiscard]] constexpr auto operator * (const Vec2& other) const -> Vec2 {
        return {x * other.x, y * other.y};
    }

    [[nodiscard]] constexpr auto operator / (const Vec2& other) const -> Vec2 {
        return {x / other.x, y / other.y};
    }

    [[nodiscard]] constexpr auto operator * (T scalar) const -> Vec2 {
        return {x * scalar, y * scalar};
    }

    [[nodiscard]] constexpr auto operator / (T scalar) const -> Vec2 {
        return {x / scalar, y / scalar};
    }

    constexpr auto operator += (const Vec2& other) -> Vec2& {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr auto operator -= (const Vec2& other) -> Vec2& {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr auto operator *= (const Vec2& other) -> Vec2& {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    constexpr auto operator /= (const Vec2& other) -> Vec2& {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    constexpr auto operator *= (T scalar) -> Vec2& {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr auto operator /= (T scalar) -> Vec2& {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Geometry
    [[nodiscard]] auto length() const -> double {
        return std::hypot(
            static_cast<double>(x),
            static_cast<double>(y)
        );
    }

    [[nodiscard]] constexpr auto squaredLength() const {
        return (x * x) + (y * y);
    }

    [[nodiscard]] auto distance(const Vec2& other) const -> double {
        return std::hypot(
            static_cast<double>(x - other.x),
            static_cast<double>(y - other.y)
        );
    }

    [[nodiscard]] constexpr auto squaredDistance(const Vec2& other) const {
        const auto dx = x - other.x;
        const auto dy = y - other.y;

        return (dx * dx) + (dy * dy);
    }

    [[nodiscard]] constexpr auto dot(const Vec2& other) const -> T {
        return (x * other.x) + (y * other.y);
    }

    /**
     * 2D cross product.
     *
     * Returns the scalar z-component of the 3D cross product.
     */
    [[nodiscard]] constexpr auto cross(const Vec2& other) const -> T {
        return (x * other.y) - (y * other.x);
    }

    /**
     * Returns the angle to another vector in degrees.
     */
    [[nodiscard]] auto angle(const Vec2& other) const -> double {
        const double denominator = length() * other.length();

        if (denominator == 0.0) {
            return 0.0;
        }

        double value = static_cast<double>(dot(other)) / denominator;
        value = std::clamp(value, -1.0, 1.0);

        return std::acos(value) * 180.0 / std::numbers::pi;
    }

    [[nodiscard]] constexpr auto flipped() const -> Vec2 {
        return {-x, -y};
    }

    [[nodiscard]] constexpr auto reflectedPoint(const Vec2& pivot) const -> Vec2 {
        return pivot + (pivot - *this);
    }

    [[nodiscard]] auto normalized() const -> Vec2 {
        Vec2 result = *this;
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
    }

    void setLength(T length) {
        const double current_length = this->length();

        if (current_length == 0.0) {
            return;
        }

        const double scale =
            static_cast<double>(length) / current_length;

        x = static_cast<T>(x * scale);
        y = static_cast<T>(y * scale);
    }

    [[nodiscard]] auto checkEqual(const Vec2& other, T threshold) const -> bool {
        return distance(other) <= static_cast<double>(threshold);
    }

    // Modification
    constexpr void set(T newX, T newY) {
        this->x = newX;
        this->y = newY;
    }

    constexpr void zero() {
        x = T{};
        y = T{};
    }

    constexpr void flip() {
        x = -x;
        y = -y;
    }

    constexpr void translateX(T value) {
        x += value;
    }

    constexpr void translateY(T value) {
        y += value;
    }

    constexpr void translate(Vec2 t) {
        x += t.x;
        y += t.y;
    }

    constexpr void translate(T tx, T ty) {
        x += tx;
        y += ty;
    }

    constexpr void scaleX(T value) {
        x *= value;
    }

    constexpr void scaleY(T value) {
        y *= value;
    }

    constexpr void scale(T value) {
        x *= value;
        y *= value;
    }

    constexpr void scale(T sx, T sy) {
        x *= sx;
        y *= sy;
    }

    constexpr void scale(const Vec2<T>& pivot, T value) {
        x = pivot.x + ((x - pivot.x) * value);
        y = pivot.y + ((y - pivot.y) * value);
    }

    constexpr void scale(const Vec2<T>& pivot, T sx, T sy) {
        x = pivot.x + ((x - pivot.x) * sx);
        y = pivot.y + ((y - pivot.y) * sy);
    }

    void rotate(double radians) {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double new_x = (c * static_cast<double>(x)) - (s * static_cast<double>(y));
        const double new_y = (s * static_cast<double>(x)) + (c * static_cast<double>(y));

        x = static_cast<T>(new_x);
        y = static_cast<T>(new_y);
    }

    void rotateDegrees(double degrees) {
        rotate(degrees * std::numbers::pi / 180.0);
    }

    void rotate(const Vec2& pivot, double radians) {
        x -= pivot.x;
        y -= pivot.y;

        rotate(radians);

        x += pivot.x;
        y += pivot.y;
    }

    void rotateDegrees(const Vec2& pivot, double degrees) {
        rotate(pivot, degrees * std::numbers::pi / 180.0);
    }

    void setLerp(const Vec2& a, const Vec2& b, double t) {
        x = static_cast<T>(static_cast<double>(a.x) + (t * (static_cast<double>(b.x) - static_cast<double>(a.x))));
        y = static_cast<T>(static_cast<double>(a.y) + (t * (static_cast<double>(b.y) - static_cast<double>(a.y))));
    }

    [[nodiscard]] static auto lerp(const Vec2& a, const Vec2& b, double t) -> Vec2 {
        Vec2 result;
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

        T values[2]{};
        const int32_t result = parser.values(2, values);

        if (result == 2) {
            x = values[0];
            y = values[1];
            return true;
        }
*/
        return false;
    }

    bool setByCSV(const String& csv, char delimiter = ',') {
        return setByCSV(csv.utf8(), delimiter);
    }

    // Serialization
    void writeToFile(File& file);
    void readFromFile(File& file);
};

// Scalar multiplication from the left.
template <typename T>
[[nodiscard]] constexpr auto operator * (T scalar, const Vec2<T>& vector) -> Vec2<T> {
    return vector * scalar;
}

// Stream output.
template <typename T>
auto operator << (std::ostream& os, const Vec2<T>& vector) -> std::ostream& {
    if constexpr (
        std::is_same_v<T, int8_t> ||
        std::is_same_v<T, uint8_t>
    ) {
        return os
            << static_cast<int32_t>(vector.x)
            << ", "
            << static_cast<int32_t>(vector.y);
    }

    return os << vector.x << ", " << vector.y;
}

// Standard types.
using Vec2i = Vec2<int32_t>;
using Vec2l = Vec2<int64_t>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

} // namespace Grain