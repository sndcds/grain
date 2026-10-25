#pragma once

#include <grain/Math/Vec2.hpp>
#include <grain/Geometry/Rect.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace Grain {

template <typename T>
class Line {
public:
    using ValueType = T;
    using Point = Vec2<T>;

    constexpr Line() = default;

    constexpr Line(const Point& p1, const Point& p2)
        : p1_(p1), p2_(p2) {
    }


    /**
     * Creates a line centered at `center`, with the given length
     * and rotation in degrees.
     */
    Line(const Point& center, T length, T degrees)
        : p1_(-length / static_cast<T>(2), T(0)),
          p2_( length / static_cast<T>(2), T(0)) {
        rotateDegrees(degrees);
        translate(center);
    }

    ~Line() = default;

    Line(const Line&) = default;
    auto operator = (const Line&) -> Line& = default;
    Line(Line&&) = default;
    auto operator = (Line&&) -> Line& = default;


    //--------------------------------------------------------------------------
    // Points
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto p1() const -> const Point& { return p1_; }
    [[nodiscard]] constexpr auto p1() -> Point& { return p1_; }

    [[nodiscard]] constexpr auto p2() const -> const Point& { return p2_; }
    [[nodiscard]] constexpr auto p2() -> Point& { return p2_; }


    //--------------------------------------------------------------------------
    //  Comparison.
    //--------------------------------------------------------------------------

    [[nodiscard]] constexpr auto operator == (const Line& other) const -> bool {
        return p1_ == other.p1_ && p2_ == other.p2_;
    }

    [[nodiscard]] constexpr auto operator != (const Line& other) const -> bool {
        return !(*this == other);
    }


    //--------------------------------------------------------------------------
    //  Geometry.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto length() const -> T {
        return vec().length();
    }

    [[nodiscard]] constexpr auto squaredLength() const -> T {
        return vec().squaredLength();
    }

    [[nodiscard]] constexpr auto vec() const -> Point {
        return p2_ - p1_;
    }

    [[nodiscard]] auto normalizedVec() const -> Point {
        return vec().normalized();
    }

    [[nodiscard]] constexpr auto center() const -> Point {
        return p1_ + ((p2_ - p1_) / static_cast<T>(2));
    }

    /**
     * Returns a normalized perpendicular vector.
     *
     * The normal points to the left of the directed line p1 -> p2.
     */
    [[nodiscard]] auto normal() const -> Point {
        const Point direction = vec();

        return Point(-direction.y, direction.x).normalized();
    }

    [[nodiscard]] constexpr auto minX() const -> T {
        return std::min(p1_.x, p2_.x);
    }

    [[nodiscard]] constexpr auto minY() const -> T {
        return std::min(p1_.y, p2_.y);
    }

    [[nodiscard]] constexpr auto maxX() const -> T {
        return std::max(p1_.x, p2_.x);
    }

    [[nodiscard]] constexpr auto maxY() const -> T {
        return std::max(p1_.y, p2_.y);
    }

    /**
     * Returns the point at normalized position `t`.
     *
     * t = 0 -> p1
     * t = 1 -> p2
     */
    [[nodiscard]] constexpr auto pointAt(T t) const -> Point {
        return p1_ + vec() * t;
    }

    // Keep this as a compatibility alias if existing code uses pointAtT().
    [[nodiscard]] constexpr auto pointAtT(T t) const -> Point {
        return pointAt(t);
    }

    /**
     * Signed cross product between the line direction and the vector
     * from p1 to the given point.
     *
     * > 0: point is left of the directed line
     * < 0: point is right of the directed line
     * = 0: point is on the line
     */
    [[nodiscard]] constexpr auto side(const Point& point) const -> T {
        return vec().x * (point.y - p1_.y)
             - vec().y * (point.x - p1_.x);
    }

    /**
     * Perpendicular distance from a point to the infinite line.
     *
     * Returns infinity for a degenerate line.
     */
    [[nodiscard]] auto distance(const Point& point) const -> double {
        const Point direction = vec();
        const double lengthSquared =
            static_cast<double>(direction.x) * direction.x +
            static_cast<double>(direction.y) * direction.y;

        if (lengthSquared <= 0.0) {
            return std::numeric_limits<double>::infinity();
        }

        const double cross =
            static_cast<double>(direction.x) *
                (static_cast<double>(point.y) - p1_.y) -
            static_cast<double>(direction.y) *
                (static_cast<double>(point.x) - p1_.x);

        return std::abs(cross) / std::sqrt(lengthSquared);
    }

    /**
     * Shortest distance from a point to this finite line segment.
     *
     * Returns infinity for a degenerate segment.
     */
    [[nodiscard]] auto distanceToSegment(const Point& point) const -> double {
        const Point direction = vec();

        const double lengthSquared =
            static_cast<double>(direction.x) * direction.x +
            static_cast<double>(direction.y) * direction.y;

        if (lengthSquared <= 0.0) {
            return std::numeric_limits<double>::infinity();
        }

        const double t =
            std::clamp(
                static_cast<double>((point - p1_).dot(direction)) /
                    lengthSquared,
                0.0,
                1.0
            );

        const Point closest =
            p1_ + direction * static_cast<T>(t);

        return static_cast<double>(point.distance(closest));
    }

    /**
     * Returns slope and intercept for:
     *
     *     y = slope * x + intercept
     *
     * For a vertical line both values are infinity.
     */
    [[nodiscard]] auto coefficients() const -> Vec2d {
        const double dx =
            static_cast<double>(p2_.x) - p1_.x;

        if (std::abs(dx) <= std::numeric_limits<double>::epsilon()) {
            const double inf =
                std::numeric_limits<double>::infinity();

            return {inf, inf};
        }

        const double dy =
            static_cast<double>(p2_.y) - p1_.y;

        const double slope = dy / dx;
        const double intercept =
            static_cast<double>(p1_.y) - slope * p1_.x;

        return {slope, intercept};
    }


    //--------------------------------------------------------------------------
    //  Mutation.
    //--------------------------------------------------------------------------

    constexpr void set(const Point& p1, const Point& p2) {
        p1_ = p1;
        p2_ = p2;
    }

    constexpr void set(T x1, T y1, T x2, T y2) {
        p1_ = Point(x1, y1);
        p2_ = Point(x2, y2);
    }

    /**
     * Moves both endpoints inward.
     *
     * Positive values shorten the line.
     */
    void inset(T p1Inset, T p2Inset) {
        const Point direction = normalizedVec();

        p1_ += direction * p1Inset;
        p2_ -= direction * p2Inset;
    }

    void translate(T x, T y) {
        const Point offset(x, y);

        p1_ += offset;
        p2_ += offset;
    }

    void translate(const Point& offset) {
        p1_ += offset;
        p2_ += offset;
    }

    /**
     * Rotates the line around the origin.
     */
    void rotateDegrees(double degrees) {
        p1_.rotateDegrees(degrees);
        p2_.rotateDegrees(degrees);
    }

    /**
     * Rotates the line around a pivot.
     */
    void rotateDegrees(const Point& pivot, double degrees) {
        p1_.rotateDegrees(pivot, degrees);
        p2_.rotateDegrees(pivot, degrees);
    }

    /**
     * Rotates p1 around p2.
     */
    void rotateP1Degrees(double degrees) {
        p1_.rotateDegrees(p2_, degrees);
    }

    /**
     * Rotates p2 around p1.
     */
    void rotateP2Degrees(double degrees) {
        p2_.rotateDegrees(p1_, degrees);
    }

    /**
     * Rotates both endpoints around the center of the line.
     */
    void rotateCenteredDegrees(T degrees) {
        const Point pivot = center();

        p1_.rotateDegrees(pivot, degrees);
        p2_.rotateDegrees(pivot, degrees);
    }

    /**
     * Moves the line perpendicular to itself.
     */
    void offset(T distance) {
        Point offsetVector(-vec().y, vec().x);

        if (offsetVector.squaredLength() <= T(0)) {
            return;
        }

        offsetVector.setLength(distance);

        p1_ += offsetVector;
        p2_ += offsetVector;
    }

    /**
     * Scales the line around its center.
     */
    void scaleCentered(T scale) {
        const Point pivot = center();

        p1_.scale(pivot, scale);
        p2_.scale(pivot, scale);
    }


    //--------------------------------------------------------------------------
    //  Intersections.
    //--------------------------------------------------------------------------

    /**
     * Finds the intersection of the two infinite lines.
     *
     * The intersection point is written to `result`.
     *
     * Returns false if the lines are parallel or degenerate.
     *
     * Note: this does NOT require the intersection to lie within either
     * line segment.
     */
    [[nodiscard]] auto intersects(const Line& line, Point& result) const -> bool {
        const Point r = vec();
        const Point s = line.vec();

        const T denominator =
            r.x * s.y - r.y * s.x;

        if (denominator == T(0)) {
            return false;
        }

        const Point delta = line.p1_ - p1_;

        const T t =
            (delta.x * s.y - delta.y * s.x) /
            denominator;

        result = p1_ + r * t;

        return true;
    }

    /**
     * Returns the shortest distance between any pair of endpoints
     * of the two line segments.
     *
     * Note that this is NOT the distance between the segments themselves.
     */
    [[nodiscard]] auto nearestPoint(const Line& line) const -> double {
        double result = std::numeric_limits<double>::infinity();

        result = std::min(
            result,
            static_cast<double>(p1_.distance(line.p1_))
        );

        result = std::min(
            result,
            static_cast<double>(p1_.distance(line.p2_))
        );

        result = std::min(
            result,
            static_cast<double>(p2_.distance(line.p1_))
        );

        result = std::min(
            result,
            static_cast<double>(p2_.distance(line.p2_))
        );

        return result;
    }

    /**
     * Tests whether a point lies within `tolerance` of the finite segment.
     *
     * `distance` receives the actual distance to the closest point.
     */
    [[nodiscard]] auto hit(const Point& point, T tolerance, T& distance) const -> bool {
        const Point direction = vec();

        const double lengthSquared =
            static_cast<double>(direction.x) * direction.x +
            static_cast<double>(direction.y) * direction.y;

        if (lengthSquared <= 0.0) {
            return false;
        }

        const double t =
            std::clamp(
                static_cast<double>((point - p1_).dot(direction)) /
                    lengthSquared,
                0.0,
                1.0
            );

        const Point closest =
            p1_ + direction * static_cast<T>(t);

        distance = static_cast<T>(point.distance(closest));

        return distance <= tolerance;
    }

    /**
     * Tests whether the finite line segment intersects an axis-aligned
     * rectangle.
     */
    [[nodiscard]] auto intersects(const Rect<T>& rect) const -> bool {
        // Liang-Barsky clipping.
        const T dx = p2_.x - p1_.x;
        const T dy = p2_.y - p1_.y;

        T tMin = T(0);
        T tMax = T(1);

        const T minX = rect.x;
        const T maxX = rect.right();
        const T minY = rect.y;
        const T maxY = rect.bottom();

        auto clip = [&](
            T p,
            T q
        ) -> bool {
            if (p == T(0)) {
                return q >= T(0);
            }

            const T r = q / p;

            if (p < T(0)) {
                if (r > tMax) {
                    return false;
                }

                if (r > tMin) {
                    tMin = r;
                }
            }
            else {
                if (r < tMin) {
                    return false;
                }

                if (r < tMax) {
                    tMax = r;
                }
            }

            return true;
        };

        return
            clip(-dx, p1_.x - minX) &&
            clip( dx, maxX - p1_.x) &&
            clip(-dy, p1_.y - minY) &&
            clip( dy, maxY - p1_.y);
    }

private:
    Point p1_{};
    Point p2_{};
};


//------------------------------------------------------------------------------
//  Standard types.
//------------------------------------------------------------------------------

using Linei = Line<int32_t>;
using Linel = Line<int64_t>;
using Linef = Line<float>;
using Lined = Line<double>;

} // namespace Grain