#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Math/Vec2.hpp>

#include <utility>

namespace Grain {

template <typename T>
class RemapRect {
public:
    constexpr RemapRect() = default;

    explicit RemapRect(
        const Rect<T>& source,
        const Rect<T>& destination,
        bool flipY = false
    ) {
        set(source, destination, flipY);
    }


    //--------------------------------------------------------------------------
    //  Setup.
    //--------------------------------------------------------------------------

    void set(const Rect<T>& source, const Rect<T>& destination, bool flipY = false) {
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

    [[nodiscard]] auto inverse() const -> RemapRect {
        return RemapRect(destination_, source_);
    }


    //--------------------------------------------------------------------------
    //  Source / destination.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto source() const -> const Rect<T>& {
        return source_;
    }

    [[nodiscard]] auto destination() const -> const Rect<T>& {
        return destination_;
    }


    //--------------------------------------------------------------------------
    //  Mapping.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto mapX(T x) const -> T {
        return static_cast<T>(
            ((static_cast<double>(x) - static_cast<double>(source_.x)) * scaleX_) +
            static_cast<double>(destination_.x)
        );
    }

    [[nodiscard]] auto mapY(T y) const -> T {
        return static_cast<T>(
            ((static_cast<double>(y) - static_cast<double>(source_.y)) * scaleY_) +
            static_cast<double>(destination_.y)
        );
    }

    [[nodiscard]] auto inverseMapX(T x) const -> T {
        return static_cast<T>(
            ((static_cast<double>(x) - static_cast<double>(destination_.x)) / scaleX_) +
            static_cast<double>(source_.x)
        );
    }

    [[nodiscard]] auto inverseMapY(T y) const -> T {
        return static_cast<T>(
            ((static_cast<double>(y) - static_cast<double>(destination_.y)) / scaleY_) +
            static_cast<double>(source_.y)
        );
    }


    //--------------------------------------------------------------------------
    //  Vec2.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto map(const Vec2<T>& point) const -> Vec2<T> {
        return {mapX(point.x), mapY(point.y)};
    }

    [[nodiscard]] auto inverseMap(const Vec2<T>& point) const -> Vec2<T> {
        return {inverseMapX(point.x), inverseMapY(point.y)};
    }

    void map(Vec2<T>& point) const {
        point = map(static_cast<const Vec2<T>&>(point));
    }

    void mapInPlace(Vec2<T>& point) const {
        point.x =
            static_cast<T>(
                ((static_cast<double>(point.x) - static_cast<double>(source_.x)) * scaleX_) +
                static_cast<double>(destination_.x)
            );

        point.y =
            static_cast<T>(
                ((static_cast<double>(point.y) - static_cast<double>(source_.y)) * scaleY_) +
                static_cast<double>(destination_.y)
            );
    }

    void inverseMap(Vec2<T>& point) const {
        point = inverseMap(static_cast<const Vec2<T>&>(point));
    }

    void map(const Vec2<T>& point, Vec2<T>& result) const {
        result = map(point);
    }

    void inverseMap(const Vec2<T>& point, Vec2<T>& result) const {
        result = inverseMap(point);
    }

    //--------------------------------------------------------------------------
    //  Rect.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto map(const Rect<T>& rect) const -> Rect<T> {
        const Vec2<T> min{rect.x, rect.y};
        const Vec2<T> max{rect.x + rect.width, rect.y + rect.height};

        const auto mappedMin = map(min);
        const auto mappedMax = map(max);

        return {
            mappedMin.x,
            mappedMin.y,
            mappedMax.x - mappedMin.x,
            mappedMax.y - mappedMin.y
        };
    }

    [[nodiscard]] auto inverseMap(const Rect<T>& rect) const -> Rect<T> {
        const Vec2<T> min{rect.x, rect.y};
        const Vec2<T> max{rect.x + rect.width, rect.y + rect.height };

        const auto mappedMin = inverseMap(min);
        const auto mappedMax = inverseMap(max);

        return {
            mappedMin.x,
            mappedMin.y,
            mappedMax.x - mappedMin.x,
            mappedMax.y - mappedMin.y
        };
    }

    void map(Rect<T>& rect) const {
        rect = map(static_cast<const Rect<T>&>(rect));
    }

    void inverseMap(Rect<T>& rect) const {
        rect = inverseMap(
            static_cast<const Rect<T>&>(rect)
        );
    }


    //--------------------------------------------------------------------------
    //  Scale.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto scaleX() const -> double { return scaleX_; }
    [[nodiscard]] auto scaleY() const -> double { return scaleY_; }

private:
    Rect<T> source_{};
    Rect<T> destination_{};

    double scaleX_ = 1.0;
    double scaleY_ = 1.0;
};


//------------------------------------------------------------------------------
//  Standard types.
//------------------------------------------------------------------------------

using RemapRectf = RemapRect<float>;
using RemapRectd = RemapRect<double>;

} // namespace Grain