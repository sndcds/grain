//
//  Vec2.hpp
//
//  Part of GrainLib
//  https://grain.one
//

#pragma once

#include "Grain.hpp"
#include "grain/String/String.hpp"
// #include "grain/String/CSVString.hpp"

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
class Vec2 {
public:
    T x_{};
    T y_{};

public:
    // Construction
    constexpr Vec2() noexcept = default;
    constexpr Vec2(T x, T y) noexcept
        : x_(x), y_(y) {}

    constexpr Vec2(const Vec2&) noexcept = default;
    constexpr Vec2(Vec2&&) noexcept = default;

    template <typename U>
        requires (!std::same_as<U, T>)
    explicit constexpr Vec2(const Vec2<U>& v) noexcept
        : x_(static_cast<T>(v.x_)),
          y_(static_cast<T>(v.y_)) {}

    explicit Vec2(const char* csv, char delimiter = ',') noexcept {
        setByCSV(csv, delimiter);
    }

    explicit Vec2(const String& csv, char delimiter = ',') noexcept {
        setByCSV(csv, delimiter);
    }

    ~Vec2() noexcept = default;

    Vec2& operator=(const Vec2&) noexcept = default;
    Vec2& operator=(Vec2&&) noexcept = default;

    template <typename U>
        requires std::is_convertible_v<U, T>
    Vec2& operator=(const Vec2<U>& v) noexcept {
        x_ = static_cast<T>(v.x_);
        y_ = static_cast<T>(v.y_);
        return *this;
    }

    // Access
    [[nodiscard]] constexpr T x() const noexcept { return x_; }
    [[nodiscard]] constexpr T y() const noexcept { return y_; }

    constexpr T& operator[](std::size_t index) noexcept {
        return index == 0 ? x_ : y_;
    }

    constexpr const T& operator[](std::size_t index) const noexcept {
        return index == 0 ? x_ : y_;
    }

    // Comparison
    [[nodiscard]] constexpr bool operator==(const Vec2&) const noexcept = default;

    [[nodiscard]] constexpr bool operator!=(const Vec2& other) const noexcept {
        return !(*this == other);
    }

    // Arithmetic
    [[nodiscard]] constexpr Vec2 operator-() const noexcept {
        return {-x_, -y_};
    }

    [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const noexcept {
        return {x_ + other.x_, y_ + other.y_};
    }

    [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const noexcept {
        return {x_ - other.x_, y_ - other.y_};
    }

    [[nodiscard]] constexpr Vec2 operator*(const Vec2& other) const noexcept {
        return {x_ * other.x_, y_ * other.y_};
    }

    [[nodiscard]] constexpr Vec2 operator/(const Vec2& other) const noexcept {
        return {x_ / other.x_, y_ / other.y_};
    }

    [[nodiscard]] constexpr Vec2 operator*(T scalar) const noexcept {
        return {x_ * scalar, y_ * scalar};
    }

    [[nodiscard]] constexpr Vec2 operator/(T scalar) const noexcept {
        return {x_ / scalar, y_ / scalar};
    }

    constexpr Vec2& operator+=(const Vec2& other) noexcept {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& other) noexcept {
        x_ -= other.x_;
        y_ -= other.y_;
        return *this;
    }

    constexpr Vec2& operator*=(const Vec2& other) noexcept {
        x_ *= other.x_;
        y_ *= other.y_;
        return *this;
    }

    constexpr Vec2& operator/=(const Vec2& other) noexcept {
        x_ /= other.x_;
        y_ /= other.y_;
        return *this;
    }

    constexpr Vec2& operator*=(T scalar) noexcept {
        x_ *= scalar;
        y_ *= scalar;
        return *this;
    }

    constexpr Vec2& operator/=(T scalar) noexcept {
        x_ /= scalar;
        y_ /= scalar;
        return *this;
    }

    // Geometry
    [[nodiscard]] double length() const noexcept {
        return std::hypot(
            static_cast<double>(x_),
            static_cast<double>(y_)
        );
    }

    [[nodiscard]] constexpr auto squaredLength() const noexcept {
        return x_ * x_ + y_ * y_;
    }

    [[nodiscard]] double distance(const Vec2& other) const noexcept {
        return std::hypot(
            static_cast<double>(x_ - other.x_),
            static_cast<double>(y_ - other.y_)
        );
    }

    [[nodiscard]] constexpr auto squaredDistance(
        const Vec2& other
    ) const noexcept {
        const auto dx = x_ - other.x_;
        const auto dy = y_ - other.y_;

        return dx * dx + dy * dy;
    }

    [[nodiscard]] constexpr T dot(const Vec2& other) const noexcept {
        return x_ * other.x_ + y_ * other.y_;
    }

    /**
     * 2D cross product.
     *
     * Returns the scalar z-component of the 3D cross product.
     */
    [[nodiscard]] constexpr T cross(const Vec2& other) const noexcept {
        return x_ * other.y_ - y_ * other.x_;
    }

    /**
     * Returns the angle to another vector in degrees.
     */
    [[nodiscard]] double angle(const Vec2& other) const noexcept {
        const double denominator = length() * other.length();

        if (denominator == 0.0) {
            return 0.0;
        }

        double value =
            static_cast<double>(dot(other)) / denominator;

        value = std::clamp(value, -1.0, 1.0);

        return std::acos(value) * 180.0 / std::numbers::pi;
    }

    [[nodiscard]] constexpr Vec2 flipped() const noexcept {
        return {-x_, -y_};
    }

    [[nodiscard]] constexpr Vec2 reflectedPoint(
        const Vec2& pivot
    ) const noexcept {
        return pivot + (pivot - *this);
    }

    [[nodiscard]] Vec2 normalized() const noexcept {
        Vec2 result = *this;
        result.normalize();
        return result;
    }

    void normalize() noexcept {
        const double l = length();

        if (l == 0.0) {
            return;
        }

        const double scale = 1.0 / l;

        x_ = static_cast<T>(x_ * scale);
        y_ = static_cast<T>(y_ * scale);
    }

    void setLength(T length) noexcept {
        const double currentLength = this->length();

        if (currentLength == 0.0) {
            return;
        }

        const double scale =
            static_cast<double>(length) / currentLength;

        x_ = static_cast<T>(x_ * scale);
        y_ = static_cast<T>(y_ * scale);
    }

    [[nodiscard]] bool checkEqual(
        const Vec2& other,
        T threshold
    ) const noexcept {
        return distance(other) <= static_cast<double>(threshold);
    }

    // Modification
    constexpr void set(T x, T y) noexcept {
        x_ = x;
        y_ = y;
    }

    constexpr void zero() noexcept {
        x_ = T{};
        y_ = T{};
    }

    constexpr void flip() noexcept {
        x_ = -x_;
        y_ = -y_;
    }

    constexpr void translateX(T value) noexcept {
        x_ += value;
    }

    constexpr void translateY(T value) noexcept {
        y_ += value;
    }

    constexpr void translate(T tx, T ty) noexcept {
        x_ += tx;
        y_ += ty;
    }

    constexpr void scaleX(T value) noexcept {
        x_ *= value;
    }

    constexpr void scaleY(T value) noexcept {
        y_ *= value;
    }

    constexpr void scale(T value) noexcept {
        x_ *= value;
        y_ *= value;
    }

    constexpr void scale(T sx, T sy) noexcept {
        x_ *= sx;
        y_ *= sy;
    }

    void rotate(double angle) noexcept {
        rotateRad(angle * std::numbers::pi / 180.0);
    }

    void rotateRad(double radians) noexcept {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        const double x =
            c * static_cast<double>(x_) -
            s * static_cast<double>(y_);

        const double y =
            s * static_cast<double>(x_) +
            c * static_cast<double>(y_);

        x_ = static_cast<T>(x);
        y_ = static_cast<T>(y);
    }

    [[nodiscard]] Vec2 rotated(double angle) const noexcept {
        Vec2 result = *this;
        result.rotate(angle);
        return result;
    }

    [[nodiscard]] Vec2 rotatedRad(double radians) const noexcept {
        Vec2 result = *this;
        result.rotateRad(radians);
        return result;
    }

    void setLerp(
        const Vec2& a,
        const Vec2& b,
        double t
    ) noexcept {
        x_ = static_cast<T>(
            static_cast<double>(a.x_) +
            t * (static_cast<double>(b.x_) -
                 static_cast<double>(a.x_))
        );

        y_ = static_cast<T>(
            static_cast<double>(a.y_) +
            t * (static_cast<double>(b.y_) -
                 static_cast<double>(a.y_))
        );
    }

    [[nodiscard]] static Vec2 lerp(
        const Vec2& a,
        const Vec2& b,
        double t
    ) noexcept {
        Vec2 result;
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

        T values[2]{};
        const int32_t result = parser.values(2, values);

        if (result == 2) {
            x_ = values[0];
            y_ = values[1];
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
[[nodiscard]] constexpr Vec2<T> operator*(
    T scalar,
    const Vec2<T>& vector
) noexcept {
    return vector * scalar;
}

// Stream output.
template <typename T>
std::ostream& operator<<(
    std::ostream& os,
    const Vec2<T>& vector
) {
    if constexpr (
        std::is_same_v<T, int8_t> ||
        std::is_same_v<T, uint8_t>
    ) {
        return os
            << static_cast<int32_t>(vector.x_)
            << ", "
            << static_cast<int32_t>(vector.y_);
    }

    return os << vector.x_ << ", " << vector.y_;
}

// Standard types.
using Vec2i = Vec2<int32_t>;
using Vec2l = Vec2<int64_t>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

} // namespace Grain