#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Math/Vec2.hpp>

#include <utility>

namespace Grain {

template <typename T>
class RemapRect {
public:
    constexpr RemapRect() noexcept = default;

    explicit RemapRect(
        const Rect<T>& source,
        const Rect<T>& destination,
        bool flipY = false
    ) noexcept
    {
        set(source, destination, flipY);
    }

    //--------------------------------------------------------------------------
    // Setup
    //--------------------------------------------------------------------------

    void set(
        const Rect<T>& source,
        const Rect<T>& destination,
        bool flipY = false
    ) noexcept
    {
        source_ = source;
        destination_ = destination;

        scaleX_ =
            source.width != T{}
                ? static_cast<double>(destination.width) /
                  static_cast<double>(source.width)
                : 1.0;

        if (flipY) {
            scaleY_ =
                source.height != T{}
                    ? -static_cast<double>(destination.height) /
                      static_cast<double>(source.height)
                    : 1.0;

            destination_.y += destination_.height;
        }
        else {
            scaleY_ =
                source.height != T{}
                    ? static_cast<double>(destination.height) /
                      static_cast<double>(source.height)
                    : 1.0;
        }
    }

    [[nodiscard]]
    RemapRect inverse() const noexcept {
        return RemapRect(
            destination_,
            source_
        );
    }


    //--------------------------------------------------------------------------
    // Source / destination
    //--------------------------------------------------------------------------

    [[nodiscard]]
    const Rect<T>& source() const noexcept {
        return source_;
    }

    [[nodiscard]]
    const Rect<T>& destination() const noexcept {
        return destination_;
    }

    //--------------------------------------------------------------------------
    // Mapping
    //--------------------------------------------------------------------------

    [[nodiscard]]
    T mapX(T x) const noexcept {
        return static_cast<T>(
            (static_cast<double>(x) -
             static_cast<double>(source_.x)) *
            scaleX_ +
            static_cast<double>(destination_.x)
        );
    }

    [[nodiscard]]
    T mapY(T y) const noexcept {
        return static_cast<T>(
            (static_cast<double>(y) -
             static_cast<double>(source_.y)) *
            scaleY_ +
            static_cast<double>(destination_.y)
        );
    }

    [[nodiscard]]
    T inverseMapX(T x) const noexcept {
        return static_cast<T>(
            (static_cast<double>(x) -
             static_cast<double>(destination_.x)) /
            scaleX_ +
            static_cast<double>(source_.x)
        );
    }

    [[nodiscard]]
    T inverseMapY(T y) const noexcept {
        return static_cast<T>(
            (static_cast<double>(y) -
             static_cast<double>(destination_.y)) /
            scaleY_ +
            static_cast<double>(source_.y)
        );
    }

    //--------------------------------------------------------------------------
    // Vec2
    //--------------------------------------------------------------------------

    [[nodiscard]]
    Vec2<T> map(const Vec2<T>& point) const noexcept {
        return {
            mapX(point.x),
            mapY(point.y)
        };
    }

    [[nodiscard]]
    Vec2<T> inverseMap(const Vec2<T>& point) const noexcept {
        return {
            inverseMapX(point.x),
            inverseMapY(point.y)
        };
    }

    void map(Vec2<T>& point) const noexcept {
        point = map(static_cast<const Vec2<T>&>(point));
    }

    void mapInPlace(Vec2<T>& point) const noexcept {
        point.x =
            static_cast<T>(
                (static_cast<double>(point.x) -
                 static_cast<double>(source_.x)) *
                scaleX_ +
                static_cast<double>(destination_.x)
            );

        point.y =
            static_cast<T>(
                (static_cast<double>(point.y) -
                 static_cast<double>(source_.y)) *
                scaleY_ +
                static_cast<double>(destination_.y)
            );
    }

    void inverseMap(Vec2<T>& point) const noexcept {
        point = inverseMap(
            static_cast<const Vec2<T>&>(point)
        );
    }

    void map(
        const Vec2<T>& point,
        Vec2<T>& result
    ) const noexcept
    {
        result = map(point);
    }

    void inverseMap(
        const Vec2<T>& point,
        Vec2<T>& result
    ) const noexcept
    {
        result = inverseMap(point);
    }

    //--------------------------------------------------------------------------
    // Rect
    //--------------------------------------------------------------------------

    [[nodiscard]]
    Rect<T> map(const Rect<T>& rect) const noexcept {
        const Vec2<T> min{
            rect.x,
            rect.y
        };

        const Vec2<T> max{
            rect.x + rect.width,
            rect.y + rect.height
        };

        const auto mappedMin = map(min);
        const auto mappedMax = map(max);

        return {
            mappedMin.x,
            mappedMin.y,
            mappedMax.x - mappedMin.x,
            mappedMax.y - mappedMin.y
        };
    }

    [[nodiscard]]
    Rect<T> inverseMap(const Rect<T>& rect) const noexcept {
        const Vec2<T> min{
            rect.x,
            rect.y
        };

        const Vec2<T> max{
            rect.x + rect.width,
            rect.y + rect.height
        };

        const auto mappedMin = inverseMap(min);
        const auto mappedMax = inverseMap(max);

        return {
            mappedMin.x,
            mappedMin.y,
            mappedMax.x - mappedMin.x,
            mappedMax.y - mappedMin.y
        };
    }

    void map(Rect<T>& rect) const noexcept {
        rect = map(static_cast<const Rect<T>&>(rect));
    }

    void inverseMap(Rect<T>& rect) const noexcept {
        rect = inverseMap(
            static_cast<const Rect<T>&>(rect)
        );
    }

    //--------------------------------------------------------------------------
    // Scale
    //--------------------------------------------------------------------------

    [[nodiscard]]
    double scaleX() const noexcept {
        return scaleX_;
    }

    [[nodiscard]]
    double scaleY() const noexcept {
        return scaleY_;
    }

private:
    Rect<T> source_{};
    Rect<T> destination_{};

    double scaleX_ = 1.0;
    double scaleY_ = 1.0;
};


//------------------------------------------------------------------------------
// Standard types
//------------------------------------------------------------------------------

using RemapRectf = RemapRect<float>;
using RemapRectd = RemapRect<double>;

} // namespace Grain