#pragma once

#include <grain/Math/Vec2.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <type_traits>

namespace Grain {

template <typename T>
struct Rect {
    T x{};
    T y{};
    T width{};
    T height{};

    // -------------------------------------------------------------------------
    // Construction
    // -------------------------------------------------------------------------

    constexpr Rect() noexcept = default;

    constexpr Rect(
        T x,
        T y,
        T width,
        T height
    ) noexcept
        : x(x),
          y(y),
          width(width),
          height(height) {}

    explicit constexpr Rect(T size) noexcept
        : width(size),
          height(size) {}

    constexpr Rect(
        T width,
        T height,
        std::nullptr_t
    ) noexcept
        : width(width),
          height(height) {}

    explicit constexpr Rect(
        const Vec2<T>& position,
        const Vec2<T>& size
    ) noexcept
        : x(position.x),
          y(position.y),
          width(size.x),
          height(size.y) {}

    explicit constexpr Rect(
        const Vec2<T>& center,
        T radius
    ) noexcept
        : x(center.x - radius),
          y(center.y - radius),
          width(radius * T{2}),
          height(radius * T{2}) {}

    constexpr Rect(
        const Vec2<T>& center,
        T width,
        T height
    ) noexcept
        : x(center.x - width / T{2}),
          y(center.y - height / T{2}),
          width(width),
          height(height) {}

    constexpr Rect(
        const Rect& other,
        T inset_size
    ) noexcept
        : x(other.x),
          y(other.y),
          width(other.width),
          height(other.height) {
        inset(inset_size);
    }

    template <typename U>
        requires std::is_convertible_v<U, T>
    explicit constexpr Rect(
        const Rect<U>& other
    ) noexcept
        : x(static_cast<T>(other.x)),
          y(static_cast<T>(other.y)),
          width(static_cast<T>(other.width)),
          height(static_cast<T>(other.height)) {}

    constexpr Rect(const Rect&) noexcept = default;
    constexpr Rect(Rect&&) noexcept = default;

    constexpr Rect& operator=(const Rect&) noexcept = default;
    constexpr Rect& operator=(Rect&&) noexcept = default;

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr Rect& operator=(
        const Rect<U>& other
    ) noexcept {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
        width = static_cast<T>(other.width);
        height = static_cast<T>(other.height);
        return *this;
    }

    // -------------------------------------------------------------------------
    // Comparison
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr bool operator==(
        const Rect&
    ) const noexcept = default;

    [[nodiscard]] constexpr bool operator!=(
        const Rect& other
    ) const noexcept {
        return !(*this == other);
    }

    // -------------------------------------------------------------------------
    // Position and size
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr Vec2<T> position() const noexcept {
        return {x, y};
    }

    [[nodiscard]] constexpr Vec2<T> size() const noexcept {
        return {width, height};
    }

    constexpr void setPosition(
        T new_x,
        T new_y
    ) noexcept {
        x = new_x;
        y = new_y;
    }

    constexpr void setPosition(
        const Vec2<T>& position
    ) noexcept {
        x = position.x;
        y = position.y;
    }

    constexpr void setSize(
        T new_width,
        T new_height
    ) noexcept {
        width = new_width;
        height = new_height;
    }

    constexpr void setSize(
        const Vec2<T>& new_size
    ) noexcept {
        width = new_size.x;
        height = new_size.y;
    }

    constexpr void set(
        T new_x,
        T new_y,
        T new_width,
        T new_height
    ) noexcept {
        x = new_x;
        y = new_y;
        width = new_width;
        height = new_height;
    }

    constexpr void zero() noexcept {
        x = T{};
        y = T{};
        width = T{};
        height = T{};
    }

    // -------------------------------------------------------------------------
    // Bounds
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr T right() const noexcept {
        return x + width;
    }

    [[nodiscard]] constexpr T bottom() const noexcept {
        return y + height;
    }

    [[nodiscard]] constexpr T centerX() const noexcept {
        return x + width / T{2};
    }

    [[nodiscard]] constexpr T centerY() const noexcept {
        return y + height / T{2};
    }

    [[nodiscard]] constexpr Vec2<T> center() const noexcept {
        return {
            centerX(),
            centerY()
        };
    }

    [[nodiscard]] constexpr Vec2<T> topLeft() const noexcept {
        return {x, y};
    }

    [[nodiscard]] constexpr Vec2<T> topRight() const noexcept {
        return {right(), y};
    }

    [[nodiscard]] constexpr Vec2<T> bottomRight() const noexcept {
        return {right(), bottom()};
    }

    [[nodiscard]] constexpr Vec2<T> bottomLeft() const noexcept {
        return {x, bottom()};
    }

    // -------------------------------------------------------------------------
    // Geometry
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr T shortSide() const noexcept {
        return std::min(width, height);
    }

    [[nodiscard]] constexpr T longSide() const noexcept {
        return std::max(width, height);
    }

    [[nodiscard]] constexpr double area() const noexcept {
        return static_cast<double>(width) *
               static_cast<double>(height);
    }

    [[nodiscard]] constexpr double aspectRatio() const noexcept {
        if (width == T{}) {
            return 1.0;
        }

        return static_cast<double>(height) /
               static_cast<double>(width);
    }

    [[nodiscard]] constexpr double absRatio() const noexcept {
        const double absolute_width =
            std::abs(static_cast<double>(width));

        const double absolute_height =
            std::abs(static_cast<double>(height));

        if (absolute_width == 0.0 ||
            absolute_height == 0.0) {
            return 1.0;
        }

        return std::max(
            absolute_width / absolute_height,
            absolute_height / absolute_width
        );
    }

    [[nodiscard]] constexpr bool isSquare() const noexcept {
        return width == height;
    }

    [[nodiscard]] constexpr bool isEmpty() const noexcept {
        return width == T{} || height == T{};
    }

    [[nodiscard]] constexpr bool isValid() const noexcept {
        return width > T{} && height > T{};
    }

    [[nodiscard]] constexpr bool isHorizontal() const noexcept {
        return std::abs(static_cast<double>(width)) >=
               std::abs(static_cast<double>(height));
    }

    [[nodiscard]] constexpr bool isVertical() const noexcept {
        return std::abs(static_cast<double>(height)) >
               std::abs(static_cast<double>(width));
    }

    [[nodiscard]] T roundedWidth() const noexcept {
        return static_cast<T>(
            std::round(static_cast<double>(width))
        );
    }

    [[nodiscard]] T roundedHeight() const noexcept {
        return static_cast<T>(
            std::round(static_cast<double>(height))
        );
    }

    // -------------------------------------------------------------------------
    // Modification
    // -------------------------------------------------------------------------

    constexpr void translate(
        T tx,
        T ty
    ) noexcept {
        x += tx;
        y += ty;
    }

    constexpr void translate(
        const Vec2<T>& offset
    ) noexcept {
        x += offset.x;
        y += offset.y;
    }

    constexpr void scale(
        T value
    ) noexcept {
        x *= value;
        y *= value;
        width *= value;
        height *= value;
    }

    constexpr void scale(
        T sx,
        T sy
    ) noexcept {
        x *= sx;
        y *= sy;
        width *= sx;
        height *= sy;
    }

    constexpr void scaleSize(
        T value
    ) noexcept {
        width *= value;
        height *= value;
    }

    constexpr void scaleSize(
        T sx,
        T sy
    ) noexcept {
        width *= sx;
        height *= sy;
    }

    constexpr void inset(
        T value
    ) noexcept {
        x += value;
        y += value;
        width -= value * T{2};
        height -= value * T{2};
    }

    constexpr void expand(
        T value
    ) noexcept {
        x -= value;
        y -= value;
        width += value * T{2};
        height += value * T{2};
    }

    constexpr void normalize() noexcept {
        if (width < T{}) {
            x += width;
            width = -width;
        }

        if (height < T{}) {
            y += height;
            height = -height;
        }
    }

    constexpr void clampSize() noexcept {
        width = std::max(width, T{});
        height = std::max(height, T{});
    }

    // -------------------------------------------------------------------------
    // Containment
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr bool contains(
        const Vec2<T>& point
    ) const noexcept {
        return point.x >= x &&
               point.x < right() &&
               point.y >= y &&
               point.y < bottom();
    }

    [[nodiscard]] constexpr bool contains(
        const Rect& other
    ) const noexcept {
        return other.x >= x &&
               other.right() <= right() &&
               other.y >= y &&
               other.bottom() <= bottom();
    }

    [[nodiscard]] constexpr bool containsX(
        T value
    ) const noexcept {
        return value >= x &&
               value < right();
    }

    [[nodiscard]] constexpr bool containsY(
        T value
    ) const noexcept {
        return value >= y &&
               value < bottom();
    }

    // -------------------------------------------------------------------------
    // Intersection
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr bool intersects(
        const Rect& other
    ) const noexcept {
        return x < other.right() &&
               right() > other.x &&
               y < other.bottom() &&
               bottom() > other.y;
    }

    [[nodiscard]] constexpr Rect intersection(
        const Rect& other
    ) const noexcept {
        const T left = std::max(x, other.x);
        const T top = std::max(y, other.y);
        const T right_edge = std::min(right(), other.right());
        const T bottom_edge = std::min(bottom(), other.bottom());

        if (left >= right_edge ||
            top >= bottom_edge) {
            return {};
        }

        return {
            left,
            top,
            right_edge - left,
            bottom_edge - top
        };
    }

    constexpr void intersect(
        const Rect& other
    ) noexcept {
        *this = intersection(other);
    }

    // -------------------------------------------------------------------------
    // Union
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr Rect united(
        const Rect& other
    ) const noexcept {
        const T left = std::min(x, other.x);
        const T top = std::min(y, other.y);
        const T right_edge = std::max(right(), other.right());
        const T bottom_edge = std::max(bottom(), other.bottom());

        return {
            left,
            top,
            right_edge - left,
            bottom_edge - top
        };
    }

    constexpr void unite(
        const Rect& other
    ) noexcept {
        *this = united(other);
    }

    // -------------------------------------------------------------------------
    // Operators
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr Rect operator+(
        const Vec2<T>& offset
    ) const noexcept {
        return {
            x + offset.x,
            y + offset.y,
            width,
            height
        };
    }

    [[nodiscard]] constexpr Rect operator-(
        const Vec2<T>& offset
    ) const noexcept {
        return {
            x - offset.x,
            y - offset.y,
            width,
            height
        };
    }

    [[nodiscard]] constexpr Rect operator*(
        const Vec2<T>& scale
    ) const noexcept {
        return {
            x * scale.x,
            y * scale.y,
            width,
            height
        };
    }

    constexpr Rect& operator+=(
        const Vec2<T>& offset
    ) noexcept {
        x += offset.x;
        y += offset.y;
        return *this;
    }

    constexpr Rect& operator-=(
        const Vec2<T>& offset
    ) noexcept {
        x -= offset.x;
        y -= offset.y;
        return *this;
    }

    constexpr Rect& operator*=(
        const Vec2<T>& scale
    ) noexcept {
        x *= scale.x;
        y *= scale.y;
        return *this;
    }

    constexpr Rect& operator+=(
        const Rect& other
    ) noexcept {
        unite(other);
        return *this;
    }

    // -------------------------------------------------------------------------
    // Output
    // -------------------------------------------------------------------------

    friend std::ostream& operator<<(
        std::ostream& stream,
        const Rect& rect
    ) {
        return stream
            << rect.x << ", "
            << rect.y << " .. "
            << rect.width << " x "
            << rect.height;
    }
};

using Recti = Rect<std::int32_t>;
using Rectl = Rect<std::int64_t>;
using Rectf = Rect<float>;
using Rectd = Rect<double>;

} // namespace Grain