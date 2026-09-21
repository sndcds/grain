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

    constexpr Size() noexcept = default;

    constexpr Size(T width, T height) noexcept
        : width(width),
          height(height) {
    }

    constexpr Size(const Size&) noexcept = default;
    constexpr Size(Size&&) noexcept = default;

    constexpr Size& operator=(const Size&) noexcept = default;
    constexpr Size& operator=(Size&&) noexcept = default;

    ~Size() noexcept = default;


    template <typename U>
    constexpr Size(const Size<U>& other) noexcept
        : width(static_cast<T>(other.width)),
          height(static_cast<T>(other.height)) {
    }

    template <typename U>
    constexpr Size& operator=(const Size<U>& other) noexcept {
        width = static_cast<T>(other.width);
        height = static_cast<T>(other.height);
        return *this;
    }

    constexpr Size& operator=(const Rect<T>& rect) noexcept {
        width = rect.width;
        height = rect.height;
        return *this;
    }


    [[nodiscard]]
    constexpr T centerX() const noexcept {
        return width / static_cast<T>(2);
    }

    [[nodiscard]]
    constexpr T centerY() const noexcept {
        return height / static_cast<T>(2);
    }

    [[nodiscard]]
    constexpr Vec2<T> center() const noexcept {
        return {
            width / static_cast<T>(2),
            height / static_cast<T>(2)
        };
    }


    [[nodiscard]]
    int64_t roundedWidth() const noexcept {
        return static_cast<int64_t>(
            std::round(static_cast<double>(width)));
    }

    [[nodiscard]]
    int64_t roundedHeight() const noexcept {
        return static_cast<int64_t>(
            std::round(static_cast<double>(height)));
    }

    [[nodiscard]]
    constexpr T area() const noexcept {
        return width * height;
    }

    [[nodiscard]]
    double aspectRatio() const noexcept {
        constexpr double epsilon =
            std::numeric_limits<double>::epsilon();

        if (std::fabs(static_cast<double>(width)) > epsilon &&
            std::fabs(static_cast<double>(height)) > epsilon) {
            return static_cast<double>(height)
                / static_cast<double>(width);
        }

        return std::numeric_limits<double>::max();
    }

    [[nodiscard]]
    bool isLandscape() const noexcept {
        return aspectRatio() < 1.0;
    }

    [[nodiscard]]
    bool isPortrait() const noexcept {
        return aspectRatio() > 1.0;
    }

    [[nodiscard]]
    bool isSquare() const noexcept {
        return std::fabs(1.0 - aspectRatio())
            <= std::numeric_limits<double>::epsilon();
    }


    constexpr void set(T value) noexcept {
        width = value;
        height = value;
    }

    constexpr void set(
        T width,
        T height) noexcept {
        this->width = width;
        this->height = height;
    }

    constexpr void zero() noexcept {
        width = T{};
        height = T{};
    }


    constexpr void flip() noexcept {
        std::swap(width, height);
    }

    constexpr void scale(T value) noexcept {
        width *= value;
        height *= value;
    }


    constexpr bool operator==(
        const Size& other) const noexcept {
        return width == other.width
            && height == other.height;
    }

    constexpr bool operator!=(
        const Size& other) const noexcept {
        return !(*this == other);
    }


    friend std::ostream& operator<<(
        std::ostream& os,
        const Size& size) {
        os << size.width
           << ", "
           << size.height;

        return os;
    }

    friend std::ostream& operator<<(
        std::ostream& os,
        const Size* size) {
        if (size == nullptr) {
            os << "Size nullptr";
        }
        else {
            os << *size;
        }

        return os;
    }
};


// Standard types

using Sizei = Size<int32_t>;
using Sizel = Size<int64_t>;
using Sizef = Size<float>;
using Sized = Size<double>;

} // namespace Grain