
#pragma once

#include "grain/Geometry/Rect.hpp"
#include "grain/Math/Vec2.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>


namespace Grain {


template <typename T>
class Bounds {

public:
    T min_x_{};
    T min_y_{};
    T max_x_{};
    T max_y_{};


    //--------------------------------------------------------------------------
    //  Construction
    //--------------------------------------------------------------------------

    constexpr Bounds() noexcept = default;

    constexpr Bounds(T min_x, T min_y, T max_x, T max_y) noexcept
        : min_x_(min_x),
          min_y_(min_y),
          max_x_(max_x),
          max_y_(max_y) {}


    //--------------------------------------------------------------------------
    //  Assignment
    //--------------------------------------------------------------------------

    auto operator = (const Vec2<T>& v) noexcept -> Bounds& {
        set(v);
        return *this;
    }

    auto operator = (const Rect<T>& r) noexcept -> Bounds& {
        const T x2 = r.x + r.width;
        const T y2 = r.y + r.height;

        min_x_ = std::min(r.x, x2);
        min_y_ = std::min(r.y, y2);
        max_x_ = std::max(r.x, x2);
        max_y_ = std::max(r.y, y2);

        return *this;
    }


    //--------------------------------------------------------------------------
    //  Comparison
    //--------------------------------------------------------------------------

    [[nodiscard]]
    constexpr auto operator == (const Bounds& other) const noexcept -> bool {
        return min_x_ == other.min_x_ &&
               min_y_ == other.min_y_ &&
               max_x_ == other.max_x_ &&
               max_y_ == other.max_y_;
    }

    [[nodiscard]]
    constexpr auto operator != (const Bounds& other) const noexcept -> bool {
        return !(*this == other);
    }


    //--------------------------------------------------------------------------
    //  Combining bounds
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto operator + (const Bounds& other) const noexcept -> Bounds {
        Bounds result = *this;
        result += other;
        return result;
    }

    [[nodiscard]]
    auto operator + (const Vec2<T>& v) const noexcept -> Bounds {
        Bounds result = *this;
        result += v;
        return result;
    }

    [[nodiscard]]
    auto operator + (const Rect<T>& r) const noexcept -> Bounds {
        Bounds result = *this;
        result += r;
        return result;
    }

    auto operator += (const Bounds& other) noexcept -> Bounds& {
        min_x_ = std::min(min_x_, other.min_x_);
        min_y_ = std::min(min_y_, other.min_y_);
        max_x_ = std::max(max_x_, other.max_x_);
        max_y_ = std::max(max_y_, other.max_y_);
        return *this;
    }

    auto operator += (const Vec2<T>& v) noexcept -> Bounds& {
        add(v);
        return *this;
    }

    auto operator += (const Rect<T>& r) noexcept -> Bounds& {
        add(r);
        return *this;
    }


    //--------------------------------------------------------------------------
    //  Accessors
    //--------------------------------------------------------------------------

    [[nodiscard]]
    constexpr auto minX() const noexcept -> T { return min_x_; }

    [[nodiscard]]
    constexpr auto minY() const noexcept -> T { return min_y_; }

    [[nodiscard]]
    constexpr auto maxX() const noexcept -> T { return max_x_; }

    [[nodiscard]]
    constexpr auto maxY() const noexcept -> T { return max_y_; }

    [[nodiscard]]
    constexpr auto width() const noexcept -> T {
        return max_x_ - min_x_;
    }

    [[nodiscard]]
    constexpr auto height() const noexcept -> T {
        return max_y_ - min_y_;
    }

    [[nodiscard]]
    constexpr auto centerX() const noexcept -> T {
        return min_x_ + (max_x_ - min_x_) / T{2};
    }

    [[nodiscard]]
    constexpr auto centerY() const noexcept -> T {
        return min_y_ + (max_y_ - min_y_) / T{2};
    }

    [[nodiscard]]
    constexpr auto center() const noexcept -> Vec2<T> {
        return {centerX(), centerY()};
    }

    [[nodiscard]]
    constexpr auto pos1() const noexcept -> Vec2<T> {
        return {min_x_, min_y_};
    }

    [[nodiscard]]
    constexpr auto pos2() const noexcept -> Vec2<T> {
        return {max_x_, min_y_};
    }

    [[nodiscard]]
    constexpr auto pos3() const noexcept -> Vec2<T> {
        return {max_x_, max_y_};
    }

    [[nodiscard]]
    constexpr auto pos4() const noexcept -> Vec2<T> {
        return {min_x_, max_y_};
    }

    [[nodiscard]]
    constexpr auto shortSide() const noexcept -> T {
        return std::min(width(), height());
    }

    [[nodiscard]]
    constexpr auto longSide() const noexcept -> T {
        return std::max(width(), height());
    }

    [[nodiscard]]
    constexpr auto aspectRatio() const noexcept -> double {
        const double w = static_cast<double>(width());
        const double h = static_cast<double>(height());

        if (w > std::numeric_limits<double>::epsilon() &&
            h > std::numeric_limits<double>::epsilon()) {
            return h / w;
        }

        return 1.0;
    }

    [[nodiscard]]
    constexpr auto isLandscape() const noexcept -> bool {
        return aspectRatio() < 1.0;
    }

    [[nodiscard]]
    constexpr auto isPortrait() const noexcept -> bool {
        return aspectRatio() > 1.0;
    }

    [[nodiscard]]
    constexpr auto isSquare() const noexcept -> bool {
        return std::fabs(1.0 - aspectRatio()) <=
               std::numeric_limits<double>::epsilon();
    }

    [[nodiscard]]
    constexpr auto isValid() const noexcept -> bool {
        return min_x_ <= max_x_ && min_y_ <= max_y_;
    }

    [[nodiscard]]
    constexpr auto rect() const noexcept -> Rect<T> {
        return {min_x_, min_y_, width(), height()};
    }


    //--------------------------------------------------------------------------
    //  Initialization
    //--------------------------------------------------------------------------

    void initForMinMaxSearch() noexcept {
        min_x_ = std::numeric_limits<T>::max();
        min_y_ = std::numeric_limits<T>::max();
        max_x_ = std::numeric_limits<T>::lowest();
        max_y_ = std::numeric_limits<T>::lowest();
    }

    void set(T x, T y) noexcept {
        min_x_ = max_x_ = x;
        min_y_ = max_y_ = y;
    }

    void set(const Vec2<T>& v) noexcept {
        set(v.x, v.y);
    }

    void set(T min_x, T min_y, T max_x, T max_y) noexcept {
        min_x_ = min_x;
        min_y_ = min_y;
        max_x_ = max_x;
        max_y_ = max_y;
    }

    void set(const Vec2<T>& min, const Vec2<T>& max) noexcept {
        set(min.x, min.y, max.x, max.y);
    }

    void set(T* values, int32_t n) noexcept {
        if (values && n == 4) {
            set(values[0], values[1], values[2], values[3]);
        }
    }

    void setLonlat(T lon1, T lat1, T lon2, T lat2) noexcept {
        set(lon1, lat1, lon2, lat2);
    }

    void sanitizeMinMax() noexcept {
        if (min_x_ > max_x_) {
            std::swap(min_x_, max_x_);
        }

        if (min_y_ > max_y_) {
            std::swap(min_y_, max_y_);
        }
    }


    //--------------------------------------------------------------------------
    //  Adding geometry
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto add(const Vec2<T>& v) noexcept -> bool {
        bool changed = false;

        changed |= addX(v.x);
        changed |= addY(v.y);

        return changed;
    }

    [[nodiscard]]
    auto add(const Vec2<T>* v) noexcept -> bool {
        return v ? add(*v) : false;
    }

    [[nodiscard]]
    auto add(const Vec2<T>* vertices, int32_t count) noexcept -> bool {
        if (!vertices || count <= 0) {
            return false;
        }

        bool changed = false;

        for (int32_t i = 0; i < count; ++i) {
            changed |= add(vertices[i]);
        }

        return changed;
    }

    [[nodiscard]]
    auto setByVec2Array(
        const Vec2<T>* vertices,
        int32_t count) noexcept -> bool {

        initForMinMaxSearch();
        return add(vertices, count);
    }

    [[nodiscard]]
    auto addX(T x) noexcept -> bool {
        bool changed = false;

        if (x < min_x_) {
            min_x_ = x;
            changed = true;
        }

        if (x > max_x_) {
            max_x_ = x;
            changed = true;
        }

        return changed;
    }

    [[nodiscard]]
    auto addY(T y) noexcept -> bool {
        bool changed = false;

        if (y < min_y_) {
            min_y_ = y;
            changed = true;
        }

        if (y > max_y_) {
            max_y_ = y;
            changed = true;
        }

        return changed;
    }

    [[nodiscard]]
    auto add(T x, T y) noexcept -> bool {
        bool changed = false;

        changed |= addX(x);
        changed |= addY(y);

        return changed;
    }

    void add(const Rect<T>& r) noexcept {
        const T x2 = r.x + r.width;
        const T y2 = r.y + r.height;

        addX(std::min(r.x, x2));
        addX(std::max(r.x, x2));
        addY(std::min(r.y, y2));
        addY(std::max(r.y, y2));
    }

    void add(const Bounds& other) noexcept {
        *this += other;
    }


    //--------------------------------------------------------------------------
    //  Limiting
    //--------------------------------------------------------------------------

    void limit(const Bounds& other) noexcept {
        min_x_ = std::max(min_x_, other.min_x_);
        min_y_ = std::max(min_y_, other.min_y_);
        max_x_ = std::min(max_x_, other.max_x_);
        max_y_ = std::min(max_y_, other.max_y_);
    }


    //--------------------------------------------------------------------------
    //  Containment and overlap
    //--------------------------------------------------------------------------

    [[nodiscard]]
    constexpr auto contains(const Vec2<T>& pos) const noexcept -> bool {
        return pos.x >= min_x_ &&
               pos.x <= max_x_ &&
               pos.y >= min_y_ &&
               pos.y <= max_y_;
    }

    [[nodiscard]]
    constexpr auto contains(
        const Vec2<T>& pos,
        T tolerance) const noexcept -> bool {

        return pos.x >= min_x_ - tolerance &&
               pos.x <= max_x_ + tolerance &&
               pos.y >= min_y_ - tolerance &&
               pos.y <= max_y_ + tolerance;
    }

    [[nodiscard]]
    constexpr auto overlaps(const Bounds& other) const noexcept -> bool {
        return !(max_x_ < other.min_x_ ||
                 other.max_x_ < min_x_ ||
                 max_y_ < other.min_y_ ||
                 other.max_y_ < min_y_);
    }


    //--------------------------------------------------------------------------
    //  Scrolling
    //--------------------------------------------------------------------------

    void scrollX(T amount) noexcept {
        min_x_ += amount;
        max_x_ += amount;
    }

    void scrollY(T amount) noexcept {
        min_y_ += amount;
        max_y_ += amount;
    }

    void scroll(T x_amount, T y_amount) noexcept {
        scrollX(x_amount);
        scrollY(y_amount);
    }

    void scrollRight() noexcept {
        scrollX(width() / T{2});
    }

    void scrollLeft() noexcept {
        scrollX(-width() / T{2});
    }

    void scrollUp() noexcept {
        scrollY(height() / T{2});
    }

    void scrollDown() noexcept {
        scrollY(-height() / T{2});
    }


    //--------------------------------------------------------------------------
    //  Zooming
    //--------------------------------------------------------------------------

    void zoom(T factor) noexcept {
        zoomX(factor);
        zoomY(factor);
    }

    void zoom(const Vec2<T>& pivot, T factor) noexcept {
        if (factor == T{}) {
            return;
        }

        min_x_ = pivot.x + (min_x_ - pivot.x) / factor;
        max_x_ = pivot.x + (max_x_ - pivot.x) / factor;

        min_y_ = pivot.y + (min_y_ - pivot.y) / factor;
        max_y_ = pivot.y + (max_y_ - pivot.y) / factor;
    }

    void zoomX(T factor) noexcept {
        if (factor == T{}) {
            return;
        }

        const T center = centerX();
        const T half_size = width() / factor / T{2};

        min_x_ = center - half_size;
        max_x_ = center + half_size;
    }

    void zoomX(T pivot, T factor) noexcept {
        if (factor == T{}) {
            return;
        }

        min_x_ = pivot + (min_x_ - pivot) / factor;
        max_x_ = pivot + (max_x_ - pivot) / factor;
    }

    void zoomY(T factor) noexcept {
        if (factor == T{}) {
            return;
        }

        const T center = centerY();
        const T half_size = height() / factor / T{2};

        min_y_ = center - half_size;
        max_y_ = center + half_size;
    }

    void zoomY(T pivot, T factor) noexcept {
        if (factor == T{}) {
            return;
        }

        min_y_ = pivot + (min_y_ - pivot) / factor;
        max_y_ = pivot + (max_y_ - pivot) / factor;
    }

    void zoomIn() noexcept {
        zoomInX();
        zoomInY();
    }

    void zoomInX() noexcept {
        const T half_size = width() / T{4};
        const T center = centerX();

        min_x_ = center - half_size;
        max_x_ = center + half_size;
    }

    void zoomInY() noexcept {
        const T half_size = height() / T{4};
        const T center = centerY();

        min_y_ = center - half_size;
        max_y_ = center + half_size;
    }

    void zoomOut() noexcept {
        zoomOutX();
        zoomOutY();
    }

    void zoomOutX() noexcept {
        const T half_size = width();
        const T center = centerX();

        min_x_ = center - half_size;
        max_x_ = center + half_size;
    }

    void zoomOutY() noexcept {
        const T half_size = height();
        const T center = centerY();

        min_y_ = center - half_size;
        max_y_ = center + half_size;
    }


    //--------------------------------------------------------------------------
    //  Extending
    //--------------------------------------------------------------------------

    void extend(double factor) noexcept {
        const T dx = static_cast<T>(width() * factor * 0.5);
        const T dy = static_cast<T>(height() * factor * 0.5);

        min_x_ -= dx;
        max_x_ += dx;

        min_y_ -= dy;
        max_y_ += dy;
    }


    //--------------------------------------------------------------------------
    //  Interpolation
    //--------------------------------------------------------------------------

    void lerp(const Bounds& other, double t) noexcept {
        *this = lerp(*this, other, t);
    }

    [[nodiscard]]
    static auto lerp(
        const Bounds& a,
        const Bounds& b,
        double t) noexcept -> Bounds {

        const auto interpolate = [t](T x, T y) -> T {
            return static_cast<T>(x + t * (y - x));
        };

        return {
            interpolate(a.min_x_, b.min_x_),
            interpolate(a.min_y_, b.min_y_),
            interpolate(a.max_x_, b.max_x_),
            interpolate(a.max_y_, b.max_y_)
        };
    }


    //--------------------------------------------------------------------------
    //  Aspect-ratio helper
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto rectWidthMaxSideLength(T max_side_length) const noexcept -> Rect<T> {
        const T w = width();
        const T h = height();

        if (w <= T{} || h <= T{}) {
            return {};
        }

        const double scale = static_cast<double>(max_side_length) /
                             static_cast<double>(std::max(w, h));

        return {
            T{},
            T{},
            static_cast<T>(w * scale),
            static_cast<T>(h * scale)
        };
    }


    //--------------------------------------------------------------------------
    //  Debugging
    //--------------------------------------------------------------------------

    friend auto operator << (std::ostream& os, const Bounds& bounds)
        -> std::ostream& {

        return os << bounds.min_x_ << ", "
                  << bounds.min_y_ << ", "
                  << bounds.max_x_ << ", "
                  << bounds.max_y_;
    }

};


//------------------------------------------------------------------------------
//  Standard types
//------------------------------------------------------------------------------

using Boundsi = Bounds<int32_t>;
using Boundsl = Bounds<int64_t>;
using Boundsf = Bounds<float>;
using Boundsd = Bounds<double>;


} // namespace Grain
