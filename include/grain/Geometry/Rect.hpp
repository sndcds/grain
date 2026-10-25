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


    //--------------------------------------------------------------------------
    //  Construction.
    //--------------------------------------------------------------------------

    constexpr Rect() = default;

    constexpr Rect(T x, T y, T width, T height)
        : x(x), y(y), width(width), height(height) {
    }

    explicit constexpr Rect(T size) : width(size), height(size) {
    }

    constexpr Rect(T width, T height, std::nullptr_t)
        : width(width), height(height) {
    }

    explicit constexpr Rect(const Vec2<T>& position, const Vec2<T>& size)
        : x(position.x), y(position.y), width(size.x), height(size.y) {
    }

    explicit constexpr Rect(const Vec2<T>& center, T radius)
        : x(center.x - radius),
          y(center.y - radius),
          width(radius * T{2}),
          height(radius * T{2}) {
    }

    constexpr Rect(const Vec2<T>& center, T width, T height)
        : x(center.x - (width / T{2})),
          y(center.y - (height / T{2})),
          width(width),
          height(height) {
    }

    constexpr Rect(const Rect& other, T inset_size)
        : x(other.x),
          y(other.y),
          width(other.width),
          height(other.height) {
        inset(inset_size);
    }

    template <typename U>
        requires std::is_convertible_v<U, T>
    explicit constexpr Rect(const Rect<U>& other)
        : x(static_cast<T>(other.x)),
          y(static_cast<T>(other.y)),
          width(static_cast<T>(other.width)),
          height(static_cast<T>(other.height)) {
    }

    constexpr Rect(const Rect&) = default;
    constexpr Rect(Rect&&) = default;

    constexpr auto operator = (const Rect&) -> Rect& = default;
    constexpr auto operator = (Rect&&) -> Rect& = default;

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr auto operator = (const Rect<U>& other) -> Rect& {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
        width = static_cast<T>(other.width);
        height = static_cast<T>(other.height);
        return *this;
    }


    //--------------------------------------------------------------------------
    //  Comparison.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto operator == (const Rect&) const -> bool = default;

    [[nodiscard]] constexpr auto operator != (const Rect& other) const -> bool {
        return !(*this == other);
    }


    //--------------------------------------------------------------------------
    //  Position and size.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto position() const -> Vec2<T> {
        return {x, y};
    }

    [[nodiscard]] constexpr auto size() const -> Vec2<T> {
        return {width, height};
    }

    constexpr void setPosition(T new_x, T new_y) {
        x = new_x;
        y = new_y;
    }

    constexpr void setPosition(const Vec2<T>& position) {
        x = position.x;
        y = position.y;
    }

    constexpr void setSize(T new_width, T new_height) {
        width = new_width;
        height = new_height;
    }

    constexpr void setSize(const Vec2<T>& new_size) {
        width = new_size.x;
        height = new_size.y;
    }

    constexpr void set(T new_x, T new_y, T new_width, T new_height) {
        x = new_x;
        y = new_y;
        width = new_width;
        height = new_height;
    }

    constexpr void zero() {
        x = T{};
        y = T{};
        width = T{};
        height = T{};
    }


    //--------------------------------------------------------------------------
    //  Bounds.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto right() const -> T {
        return x + width;
    }

    [[nodiscard]] constexpr auto bottom() const -> T {
        return y + height;
    }

    [[nodiscard]] constexpr auto centerX() const -> T {
        return x + (width / T{2});
    }

    [[nodiscard]] constexpr auto centerY() const -> T {
        return y + (height / T{2});
    }

    [[nodiscard]] constexpr auto center() const -> Vec2<T> {
        return {centerX(), centerY()};
    }

    [[nodiscard]] constexpr auto topLeft() const -> Vec2<T> {
        return {x, y};
    }

    [[nodiscard]] constexpr auto topRight() const -> Vec2<T> {
        return {right(), y};
    }

    [[nodiscard]] constexpr auto bottomRight() const -> Vec2<T> {
        return {right(), bottom()};
    }

    [[nodiscard]] constexpr auto bottomLeft() const -> Vec2<T> {
        return {x, bottom()};
    }


    //--------------------------------------------------------------------------
    //  Geometry.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto shortSide() const -> T {
        return std::min(width, height);
    }

    [[nodiscard]] constexpr auto longSide() const -> T {
        return std::max(width, height);
    }

    [[nodiscard]] constexpr auto area() const -> double {
        return static_cast<double>(width) *
               static_cast<double>(height);
    }

    [[nodiscard]] constexpr auto aspectRatio() const -> double {
        if (width == T{}) {
            return 1.0;
        }

        return static_cast<double>(height) /
               static_cast<double>(width);
    }

    [[nodiscard]] constexpr auto absRatio() const -> double {
        const double absolute_width =
            std::abs(static_cast<double>(width));

        const double absolute_height =
            std::abs(static_cast<double>(height));

        if (absolute_width == 0.0 || absolute_height == 0.0) {
            return 1.0;
        }

        return std::max(
            absolute_width / absolute_height,
            absolute_height / absolute_width
        );
    }

    [[nodiscard]] constexpr auto isSquare() const -> bool {
        return width == height;
    }

    [[nodiscard]] constexpr auto isEmpty() const -> bool {
        return width == T{} || height == T{};
    }

    [[nodiscard]] constexpr auto isValid() const -> bool {
        return width > T{} && height > T{};
    }

    [[nodiscard]] constexpr auto isHorizontal() const -> bool {
        return std::abs(static_cast<double>(width)) >=
               std::abs(static_cast<double>(height));
    }

    [[nodiscard]] constexpr auto isVertical() const -> bool {
        return std::abs(static_cast<double>(height)) >
               std::abs(static_cast<double>(width));
    }

    [[nodiscard]] auto roundedWidth() const -> T {
        return static_cast<T>(
            std::round(static_cast<double>(width))
        );
    }

    [[nodiscard]] auto roundedHeight() const -> T {
        return static_cast<T>(
            std::round(static_cast<double>(height))
        );
    }


    //--------------------------------------------------------------------------
    //  Modification.
    //--------------------------------------------------------------------------

    constexpr void translate(T tx, T ty) {
        x += tx;
        y += ty;
    }

    constexpr void translate(const Vec2<T>& offset) {
        x += offset.x;
        y += offset.y;
    }

    constexpr void scale(T value) {
        x *= value;
        y *= value;
        width *= value;
        height *= value;
    }

    constexpr void scale(T sx, T sy) {
        x *= sx;
        y *= sy;
        width *= sx;
        height *= sy;
    }

    constexpr void scaleSize(T value) {
        width *= value;
        height *= value;
    }

    constexpr void scaleSize(T sx, T sy) {
        width *= sx;
        height *= sy;
    }

    constexpr void inset(T value) {
        x += value;
        y += value;
        width -= value * T{2};
        height -= value * T{2};
    }

    constexpr void expand(T value) {
        x -= value;
        y -= value;
        width += value * T{2};
        height += value * T{2};
    }

    constexpr void normalize() {
        if (width < T{}) {
            x += width;
            width = -width;
        }

        if (height < T{}) {
            y += height;
            height = -height;
        }
    }

    constexpr void clampSize() {
        width = std::max(width, T{});
        height = std::max(height, T{});
    }

    void makePositiveSize() {
        if (width < 0) {
            x += width;
            width = -width;
        }

        if (height < 0) {
            y += height;
            height = -height;
        }
    }


    //--------------------------------------------------------------------------
    //  Containment.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto contains(const Vec2<T>& point) const -> bool {
        return point.x >= x &&
               point.x < right() &&
               point.y >= y &&
               point.y < bottom();
    }

    [[nodiscard]] auto contains(const Vec2<T>& point, T radius) const -> bool {
        const T dx =
            point.x < x ? x - point.x :
            point.x > right() ? point.x - right() :
            T{0};

        const T dy =
            point.y < y ? y - point.y :
            point.y > bottom() ? point.y - bottom() :
            T{0};

        return dx * dx + dy * dy <= radius * radius;
    }

    [[nodiscard]] constexpr auto contains(const Rect& other) const -> bool {
        return
            other.x >= x &&
            other.right() <= right() &&
            other.y >= y &&
            other.bottom() <= bottom();
    }

    [[nodiscard]] constexpr auto containsX(T value) const -> bool {
        return
            value >= x &&
            value < right();
    }

    [[nodiscard]] constexpr auto containsY(T value) const -> bool {
        return value >= y &&
               value < bottom();
    }


    //--------------------------------------------------------------------------
    //  Intersection.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto intersects(const Rect& other) const -> bool {
        return
            x < other.right() &&
            right() > other.x &&
            y < other.bottom() &&
            bottom() > other.y;
    }

    [[nodiscard]] constexpr auto intersection(const Rect& other) const -> Rect {
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

    constexpr void intersect(const Rect& other) {
        *this = intersection(other);
    }


    //--------------------------------------------------------------------------
    //  Union.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto united(const Rect& other) const -> Rect {
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

    constexpr void unite(const Rect& other) {
        *this = united(other);
    }


    //--------------------------------------------------------------------------
    //  Interpolation.
    //--------------------------------------------------------------------------

    [[nodiscard]]
    constexpr auto lerp(const Rect& dst, double t) const -> Rect {
        return {
            static_cast<T>(x + (dst.x - x) * t),
            static_cast<T>(y + (dst.y - y) * t),
            static_cast<T>(width + (dst.width - width) * t),
            static_cast<T>(height + (dst.height - height) * t)
        };
    }


    //--------------------------------------------------------------------------
    //  Operators.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto operator + (const Vec2<T>& offset) const -> Rect {
        return {
            x + offset.x,
            y + offset.y,
            width,
            height
        };
    }

    [[nodiscard]] constexpr auto operator - (const Vec2<T>& offset) const -> Rect {
        return {
            x - offset.x,
            y - offset.y,
            width,
            height
        };
    }

    [[nodiscard]] constexpr auto operator * (const Vec2<T>& scale) const -> Rect {
        return {
            x * scale.x,
            y * scale.y,
            width,
            height
        };
    }

    constexpr auto operator += (const Vec2<T>& offset) -> Rect& {
        x += offset.x;
        y += offset.y;
        return *this;
    }

    constexpr auto operator -= (const Vec2<T>& offset) -> Rect& {
        x -= offset.x;
        y -= offset.y;
        return *this;
    }

    constexpr auto operator *= (const Vec2<T>& scale) -> Rect& {
        x *= scale.x;
        y *= scale.y;
        return *this;
    }

    constexpr auto operator += (const Rect& other) -> Rect& {
        unite(other);
        return *this;
    }


    //--------------------------------------------------------------------------
    //  Output.
    //--------------------------------------------------------------------------

    friend auto operator << (std::ostream& stream, const Rect& rect ) -> std::ostream& {
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