#pragma once

#include <grain/Math/Vec2.hpp>

#include <array>
#include <cmath>
#include <cstdint>
#include <numbers>


namespace Grain {


template <class T>
class Circle {

public:
    //--------------------------------------------------------------------------
    //  Construction
    //--------------------------------------------------------------------------

    Circle() noexcept = default;

    Circle(const Vec2<T>& center, T radius) noexcept
        : center_(center),
          radius_(radius) {
    }

    Circle(T x, T y, T radius) noexcept
        : center_(x, y),
          radius_(radius) {
    }


    //--------------------------------------------------------------------------
    //  Properties
    //--------------------------------------------------------------------------

    [[nodiscard]] auto isValid() const noexcept -> bool {
        return std::isfinite(static_cast<double>(center_.x_)) &&
               std::isfinite(static_cast<double>(center_.y_)) &&
               std::isfinite(static_cast<double>(radius_)) &&
               radius_ >= T(0);
    }

    [[nodiscard]] auto center() const noexcept -> const Vec2<T>& {
        return center_;
    }

    [[nodiscard]] auto radius() const noexcept -> T {
        return radius_;
    }

    [[nodiscard]] auto minX() const noexcept -> T {
        return center_.x_ - radius_;
    }

    [[nodiscard]] auto minY() const noexcept -> T {
        return center_.y_ - radius_;
    }

    [[nodiscard]] auto maxX() const noexcept -> T {
        return center_.x_ + radius_;
    }

    [[nodiscard]] auto maxY() const noexcept -> T {
        return center_.y_ + radius_;
    }

    [[nodiscard]] auto area() const noexcept -> double {
        const double r = static_cast<double>(radius_);
        return std::numbers::pi_v<double> * r * r;
    }

    [[nodiscard]] auto circumference() const noexcept -> double {
        return 2.0 * std::numbers::pi_v<double> *
               static_cast<double>(radius_);
    }


    //--------------------------------------------------------------------------
    //  Modification
    //--------------------------------------------------------------------------

    auto set(const Vec2<T>& center, T radius) noexcept -> void {
        center_ = center;
        radius_ = radius;
    }

    auto set(T x, T y, T radius) noexcept -> void {
        center_.x_ = x;
        center_.y_ = y;
        radius_ = radius;
    }

    auto setCenter(const Vec2<T>& center) noexcept -> void {
        center_ = center;
    }

    auto setRadius(T radius) noexcept -> void {
        radius_ = radius;
    }

    auto setCircumference(T circumference) noexcept -> void {
        radius_ = static_cast<T>(
            static_cast<double>(circumference) /
            (2.0 * std::numbers::pi_v<double>)
        );
    }

    auto translate(T tx, T ty) noexcept -> void {
        center_.x_ += tx;
        center_.y_ += ty;
    }

    auto translate(const Vec2<T>& translation) noexcept -> void {
        center_.x_ += translation.x_;
        center_.y_ += translation.y_;
    }


    //--------------------------------------------------------------------------
    //  Tangents
    //--------------------------------------------------------------------------

    /*
        Computes the four outer tangent points connecting two circles.

        Output order:
            0: First circle, tangent 1
            1: Second circle, tangent 1
            2: First circle, tangent 2
            3: Second circle, tangent 2

        Returns false if the circles are invalid or no real outer
        tangents exist.
    */
    [[nodiscard]]
    auto outerTangentPoints(
        const Circle<T>& circle,
        std::array<Vec2<T>, 4>& out_points
    ) const noexcept -> bool {

        return calculateTangentPoints(circle, false, out_points);
    }


    /*
        Computes the four inner tangent points connecting two circles.

        Output order:
            0: First circle, tangent 1
            1: Second circle, tangent 1
            2: First circle, tangent 2
            3: Second circle, tangent 2

        Returns false if the circles are invalid or no real inner
        tangents exist.
    */
    [[nodiscard]]
    auto innerTangentPoints(
        const Circle<T>& circle,
        std::array<Vec2<T>, 4>& out_points
    ) const noexcept -> bool {

        return calculateTangentPoints(circle, true, out_points);
    }


private:
    //--------------------------------------------------------------------------
    //  Tangent calculation
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto calculateTangentPoints(
        const Circle<T>& circle,
        bool inner,
        std::array<Vec2<T>, 4>& out_points
    ) const noexcept -> bool {

        if (!isValid() || !circle.isValid()) {
            return false;
        }

        const double x1 = static_cast<double>(center_.x_);
        const double y1 = static_cast<double>(center_.y_);
        const double r1 = static_cast<double>(radius_);

        const double x2 = static_cast<double>(circle.center_.x_);
        const double y2 = static_cast<double>(circle.center_.y_);
        const double r2 = static_cast<double>(circle.radius_);

        const double dx = x2 - x1;
        const double dy = y2 - y1;

        const double distance = std::hypot(dx, dy);

        if (distance <= 0.0) {
            return false;
        }

        // Determine the required normal projection.
        const double radius_difference = inner
            ? r1 + r2
            : r1 - r2;

        const double k = radius_difference / distance;

        // No real tangent exists if |k| > 1.
        if (std::abs(k) > 1.0) {
            return false;
        }

        // Unit direction between the circle centers.
        const double ux = dx / distance;
        const double uy = dy / distance;

        // Perpendicular unit direction.
        const double px = -uy;
        const double py = ux;

        const double h = std::sqrt(std::max(0.0, 1.0 - k * k));

        std::array<Vec2<T>, 4> points{};

        for (int32_t i = 0; i < 2; ++i) {

            const double sign = (i == 0) ? 1.0 : -1.0;

            // Normal vector of the tangent.
            const double nx = ux * k + px * h * sign;
            const double ny = uy * k + py * h * sign;

            // Tangent point on the first circle.
            points[i * 2].x_ = static_cast<T>(x1 + r1 * nx);
            points[i * 2].y_ = static_cast<T>(y1 + r1 * ny);

            // Tangent point on the second circle.
            const double direction = inner ? -1.0 : 1.0;

            points[i * 2 + 1].x_ =
                static_cast<T>(x2 + r2 * nx * direction);

            points[i * 2 + 1].y_ =
                static_cast<T>(y2 + r2 * ny * direction);
        }

        out_points = points;

        return true;
    }


    //------------------------------------------------------------------------------
    //  Static utilities
    //------------------------------------------------------------------------------

    static auto area(const double radius) -> double{
        return std::numbers::pi * radius * radius;
    }

    static auto circumference(const double radius) -> double{
        return 2.0 * std::numbers::pi * radius;
    }

    static auto radiusFromArea(const double area) -> double{
        return std::sqrt(area / std::numbers::pi);
    }

private:
    Vec2<T> center_{};
    T radius_ = T(10);
};


//------------------------------------------------------------------------------
//  Standard types
//------------------------------------------------------------------------------

using Circlei = Circle<int32_t>;    // 32-bit integer
using Circlel = Circle<int64_t>;    // 64-bit integer
using Circlef = Circle<float>;      // 32-bit floating point
using Circled = Circle<double>;     // 64-bit floating point


} // namespace Grain
