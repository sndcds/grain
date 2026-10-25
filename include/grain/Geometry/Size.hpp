#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Math/Vec2.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <ostream>
#include <utility>

namespace Grain {

template <typename T>
struct Size {
    T width{};
    T height{};

    constexpr Size() = default;

    constexpr Size(T width, T height) : width(width), height(height) {
    }

    constexpr Size(const Size&) = default;
    constexpr Size(Size&&) = default;

    constexpr auto operator = (const Size&) -> Size& = default;
    constexpr auto operator = (Size&&) -> Size& = default;

    ~Size() = default;


    template <typename U>
    constexpr Size(const Size<U>& other)
        : width(static_cast<T>(other.width)),
          height(static_cast<T>(other.height)) {
    }

    template <typename U>
    constexpr auto operator = (const Size<U>& other) -> Size& {
        width = static_cast<T>(other.width);
        height = static_cast<T>(other.height);
        return *this;
    }

    constexpr auto operator = (const Rect<T>& rect) -> Size& {
        width = rect.width;
        height = rect.height;
        return *this;
    }


    [[nodiscard]] constexpr auto centerX() const -> T {
        return width / static_cast<T>(2);
    }

    [[nodiscard]] constexpr auto centerY() const -> T {
        return height / static_cast<T>(2);
    }

    [[nodiscard]] constexpr auto center() const -> Vec2<T> {
        return {width / static_cast<T>(2), height / static_cast<T>(2)};
    }


    [[nodiscard]] auto roundedWidth() const -> int64_t {
        return static_cast<int64_t>(std::round(static_cast<double>(width)));
    }

    [[nodiscard]] auto roundedHeight() const -> int64_t {
        return static_cast<int64_t>(std::round(static_cast<double>(height)));
    }

    [[nodiscard]] constexpr auto area() const -> T {
        return width * height;
    }

    [[nodiscard]] auto aspectRatio() const -> double {
        constexpr double epsilon =
            std::numeric_limits<double>::epsilon();

        if (std::fabs(static_cast<double>(width)) > epsilon &&
            std::fabs(static_cast<double>(height)) > epsilon) {
            return static_cast<double>(height)
                / static_cast<double>(width);
        }

        return std::numeric_limits<double>::max();
    }

    [[nodiscard]] auto isLandscape() const -> bool {
        return aspectRatio() < 1.0;
    }

    [[nodiscard]] auto isPortrait() const -> bool {
        return aspectRatio() > 1.0;
    }

    [[nodiscard]] auto isSquare() const -> bool {
        return std::fabs(1.0 - aspectRatio()) <= std::numeric_limits<double>::epsilon();
    }


    constexpr void set(T value) {
        width = value;
        height = value;
    }

    constexpr void set(T width, T height) {
        this->width = width;
        this->height = height;
    }

    constexpr void zero() {
        width = T{};
        height = T{};
    }


    constexpr void flip() {
        std::swap(width, height);
    }

    constexpr void scale(T value) {
        width *= value;
        height *= value;
    }


    constexpr auto operator == (
        const Size& other) const -> bool {
        return width == other.width
            && height == other.height;
    }

    constexpr auto operator != (
        const Size& other) const -> bool {
        return !(*this == other);
    }


    friend auto operator << (
        std::ostream& os,
        const Size& size) -> std::ostream& {
        os << size.width
           << ", "
           << size.height;

        return os;
    }

    friend auto operator << (
        std::ostream& os,
        const Size* size) -> std::ostream& {
        if (size == nullptr) {
            os << "Size nullptr";
        }
        else {
            os << *size;
        }

        return os;
    }
};


//------------------------------------------------------------------------------
//  Standard types.
//------------------------------------------------------------------------------

using Sizei = Size<int32_t>;
using Sizel = Size<int64_t>;
using Sizef = Size<float>;
using Sized = Size<double>;

} // namespace Grain