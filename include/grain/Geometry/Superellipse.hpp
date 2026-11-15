#pragma once

#include <grain/Math/Vec2.hpp>
#include <grain/Graphics/GraphicPath.hpp>

#include <cmath>


namespace Grain {


class Superellipse {

public:
    //--------------------------------------------------------------------------
    //  Construction
    //--------------------------------------------------------------------------

    Superellipse() noexcept = default;

    Superellipse(double a, double b, double n) noexcept
        : a_(a),
          b_(b),
          n_(n) {
    }


    //--------------------------------------------------------------------------
    //  Properties
    //--------------------------------------------------------------------------

    [[nodiscard]] auto a() const noexcept -> double {
        return a_;
    }

    [[nodiscard]] auto b() const noexcept -> double {
        return b_;
    }

    [[nodiscard]] auto exponent() const noexcept -> double {
        return n_;
    }

    [[nodiscard]] auto isValid() const noexcept -> bool {
        return std::isfinite(a_) &&
               std::isfinite(b_) &&
               std::isfinite(n_) &&
               a_ > 0.0 &&
               b_ > 0.0 &&
               n_ > 0.0;
    }


    //--------------------------------------------------------------------------
    //  Modification
    //--------------------------------------------------------------------------

    auto set(double a, double b, double n) noexcept -> void {
        a_ = a;
        b_ = b;
        n_ = n;
    }

    auto setA(double a) noexcept -> void {
        a_ = a;
    }

    auto setB(double b) noexcept -> void {
        b_ = b;
    }

    auto setExponent(double n) noexcept -> void {
        n_ = n;
    }


    //--------------------------------------------------------------------------
    //  Geometry
    //--------------------------------------------------------------------------

    /*
        Returns a point on the superellipse.

        Parameter t is an angle in radians.

        The parameterization is:

            x = a * sign(cos(t)) * |cos(t)|^(2/n)
            y = b * sign(sin(t)) * |sin(t)|^(2/n)
    */
    [[nodiscard]]
    static auto posAtT(
        double t,
        double a,
        double b,
        double n
    ) noexcept -> Vec2d {

        if (!isValidParameters(a, b, n) || !std::isfinite(t)) {
            return {};
        }

        const double cos_t = std::cos(t);
        const double sin_t = std::sin(t);

        const double exponent = 2.0 / n;

        return {
            a * signedPower(cos_t, exponent),
            b * signedPower(sin_t, exponent)
        };
    }


    /*
        Returns the tangent direction at parameter t.

        The result is the derivative with respect to t.

        For n < 2, the derivative may become unbounded at
        the coordinate-axis crossings. In that case, this
        function returns a normalized limiting tangent.
    */
    [[nodiscard]]
    static auto tangentAtT(
        double t,
        double a,
        double b,
        double n
    ) noexcept -> Vec2d {

        if (!isValidParameters(a, b, n) || !std::isfinite(t)) {
            return {};
        }

        const double cos_t = std::cos(t);
        const double sin_t = std::sin(t);

        const double exponent = 2.0 / n - 1.0;

        const double dx = -a * exponent *
            signedPower(sin_t, 1.0) *
            signedPower(cos_t, exponent - 1.0);

        const double dy = b * exponent *
            signedPower(cos_t, 1.0) *
            signedPower(sin_t, exponent - 1.0);

        const double length = std::hypot(dx, dy);

        if (std::isfinite(length) && length > 0.0) {
            return { dx / length, dy / length };
        }

        // Limiting tangent at singular points.
        // This also handles the special case n == 2.
        const double eps = 1.0e-12;

        const double ct = std::abs(cos_t);
        const double st = std::abs(sin_t);

        if (ct < eps) {
            return { -std::copysign(1.0, sin_t), 0.0 };
        }

        if (st < eps) {
            return { 0.0, std::copysign(1.0, cos_t) };
        }

        return {};
    }


    // Instance convenience methods.

    [[nodiscard]]
    auto posAtT(double t) const noexcept -> Vec2d {
        return posAtT(t, a_, b_, n_);
    }

    [[nodiscard]]
    auto tangentAtT(double t) const noexcept -> Vec2d {
        return tangentAtT(t, a_, b_, n_);
    }


    [[nodiscard]]
    auto toGraphicPath(
        int32_t segments_per_quadrant = 4
    ) const -> GraphicPath {

        GraphicPath path(segments_per_quadrant * 4 + 1);

        if (!isValid() || segments_per_quadrant < 1) {
            return path;
        }

        constexpr double two_pi = std::numbers::pi * 2.0;

        const int32_t segment_count = segments_per_quadrant * 4;
        const double step = two_pi / static_cast<double>(segment_count);

        for (int32_t i = 0; i <= segment_count; ++i) {

            const double t = static_cast<double>(i) * step;

            const Vec2d position = posAtT(t);

            if (i == 0) {
                path.addPoint(position);
                continue;
            }

            // Tangent direction at the previous anchor.
            const double previous_t = t - step;
            const Vec2d previous_position = posAtT(previous_t);

            const Vec2d previous_before = posAtT(previous_t - step * 0.01);
            const Vec2d previous_after = posAtT(previous_t + step * 0.01);

            Vec2d previous_tangent = previous_after - previous_before;
            previous_tangent.normalize();

            // Tangent direction at the current anchor.
            const Vec2d current_before = posAtT(t - step * 0.01);
            const Vec2d current_after = posAtT(t + step * 0.01);

            Vec2d current_tangent = current_after - current_before;
            current_tangent.normalize();

            const Vec2d previous = {
                previous_position.x,
                previous_position.y
            };

            const double chord_length = std::hypot(
                position.x - previous.x,
                position.y - previous.y
            );

            const double handle_length = chord_length / 3.0;

            const Vec2d control1 = previous + previous_tangent * handle_length;
            const Vec2d control2 = position - current_tangent * handle_length;

            path.addBezier(control1, control2, position);
        }

        path.setClosed(true);

        return path;
    }

private:
    //--------------------------------------------------------------------------
    //  Helpers
    //--------------------------------------------------------------------------

    [[nodiscard]]
    static auto isValidParameters(
        double a,
        double b,
        double n
    ) noexcept -> bool {

        return std::isfinite(a) &&
               std::isfinite(b) &&
               std::isfinite(n) &&
               a > 0.0 &&
               b > 0.0 &&
               n > 0.0;
    }

    [[nodiscard]]
    static auto signedPower(
        double value,
        double exponent
    ) noexcept -> double {

        return std::copysign(
            std::pow(std::abs(value), exponent),
            value
        );
    }


private:
    double a_ = 1.0;
    double b_ = 1.0;
    double n_ = 2.0;
};


} // namespace Grain
