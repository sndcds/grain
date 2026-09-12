//
//  Quadrilateral.cpp
//
//  This file is part of GrainLib, see <https://grain.one>.
//

#include <grain/Math/Quadrilateral.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace Grain {

//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

Quadrilateral::Quadrilateral(
    const Vec2d& p1,
    const Vec2d& p2,
    const Vec2d& p3,
    const Vec2d& p4
) noexcept {
    set(p1, p2, p3, p4);
}

Quadrilateral::Quadrilateral(
    const Vec2d& min,
    const Vec2d& max
) noexcept {
    setByMinMax(min, max);
}

Quadrilateral::Quadrilateral(const Rectd& rect) noexcept {
    setByRect(rect);
}


//------------------------------------------------------------------------------
// Points
//------------------------------------------------------------------------------

void Quadrilateral::set(
    const Vec2d& p1,
    const Vec2d& p2,
    const Vec2d& p3,
    const Vec2d& p4
) noexcept {
    points_[0] = p1;
    points_[1] = p2;
    points_[2] = p3;
    points_[3] = p4;

    valid_ = true;
    solvePerspective();
}

void Quadrilateral::setPoint(
    std::size_t index,
    const Vec2d& point
) noexcept {
    if (index >= points_.size()) {
        return;
    }

    points_[index] = point;

    valid_ = true;
    solvePerspective();
}

void Quadrilateral::setByMinMax(
    const Vec2d& min,
    const Vec2d& max
) noexcept {
    set(
        Vec2d(min.x, min.y),
        Vec2d(max.x, min.y),
        Vec2d(max.x, max.y),
        Vec2d(min.x, max.y)
    );
}

void Quadrilateral::setByRect(const Rectd& rect) noexcept {
    set(
        Vec2d(rect.x, rect.y),
        Vec2d(rect.right(), rect.y),
        Vec2d(rect.right(), rect.bottom()),
        Vec2d(rect.x, rect.bottom())
    );
}


//------------------------------------------------------------------------------
// Geometry
//------------------------------------------------------------------------------

double Quadrilateral::area() const noexcept {
    const double diagonal1 = points_[0].distance(points_[2]);
    const double diagonal2 = points_[1].distance(points_[3]);

    const double side1 = points_[0].distance(points_[1]);
    const double side2 = points_[1].distance(points_[2]);
    const double side3 = points_[2].distance(points_[3]);
    const double side4 = points_[3].distance(points_[0]);

    const double m =
        side2 * side2 +
        side4 * side4 -
        side1 * side1 -
        side3 * side3;

    const double value =
        4.0 * diagonal1 * diagonal1 * diagonal2 * diagonal2 -
        m * m;

    return 0.25 * std::sqrt(std::max(0.0, value));
}

bool Quadrilateral::isSimple() const noexcept {
    if (!valid_) {
        return false;
    }

    // Test whether edge p1-p2 intersects edge p3-p4.
    const Vec2d edge12 = points_[1] - points_[0];

    const double s1 =
        edge12.cross(points_[2] - points_[0]);

    const double s2 =
        edge12.cross(points_[3] - points_[0]);

    // Test whether p1 and p2 lie on opposite sides
    // of the line p3-p4.
    const Vec2d edge34 = points_[3] - points_[2];

    const double s3 =
        edge34.cross(points_[0] - points_[2]);

    const double s4 =
        edge34.cross(points_[1] - points_[2]);

    if (s1 == 0.0 || s2 == 0.0 ||
        s3 == 0.0 || s4 == 0.0) {
        return false;
        }

    if ((s1 > 0.0) == (s2 > 0.0)) {
        return true;
    }

    if ((s3 > 0.0) == (s4 > 0.0)) {
        return true;
    }

    return false;
}

bool Quadrilateral::isConvex() const noexcept {
    bool hasPositive = false;
    bool hasNegative = false;

    for (std::size_t i = 0; i < 4; ++i) {
        const Vec2d& p0 = points_[i];
        const Vec2d& p1 = points_[(i + 1) % 4];
        const Vec2d& p2 = points_[(i + 2) % 4];

        const double crossProduct =
            (p1 - p0).cross(p2 - p1);

        if (crossProduct > 0.0) {
            hasPositive = true;
        }
        else if (crossProduct < 0.0) {
            hasNegative = true;
        }

        if (hasPositive && hasNegative) {
            return false;
        }
    }

    return true;
}

double Quadrilateral::flattestAngle() const noexcept {
    double maxAngle = 0.0;

    for (std::size_t i = 0; i < 4; ++i) {
        const Vec2d& current = points_[i];
        const Vec2d& next = points_[(i + 1) % 4];
        const Vec2d& afterNext = points_[(i + 2) % 4];

        const Vec2d v1 = current - next;
        const Vec2d v2 = afterNext - next;

        const double angle = 180.0 - v1.angle(v2);

        if (angle > maxAngle) {
            maxAngle = angle;
        }
    }

    return maxAngle;
}

bool Quadrilateral::contains(const Vec2d& point) const noexcept {
    if (!valid_) {
        return false;
    }

    constexpr double epsilon = 1e-12;

    const double s1 =
        (points_[1] - points_[0]).cross(point - points_[0]);

    const double s2 =
        (points_[2] - points_[1]).cross(point - points_[1]);

    const double s3 =
        (points_[3] - points_[2]).cross(point - points_[2]);

    const double s4 =
        (points_[0] - points_[3]).cross(point - points_[3]);

    const bool hasPositive =
        s1 > epsilon ||
        s2 > epsilon ||
        s3 > epsilon ||
        s4 > epsilon;

    const bool hasNegative =
        s1 < -epsilon ||
        s2 < -epsilon ||
        s3 < -epsilon ||
        s4 < -epsilon;

    return !(hasPositive && hasNegative);
}

Vec2d Quadrilateral::centroid() const noexcept {
    const Vec2d triangle1 =
        (points_[0] + points_[1] + points_[2]) / 3.0;

    const Vec2d triangle2 =
        (points_[2] + points_[3] + points_[0]) / 3.0;

    return (triangle1 + triangle2) * 0.5;
}


//------------------------------------------------------------------------------
// Parametric coordinates
//------------------------------------------------------------------------------

Vec2d Quadrilateral::project(
    double u,
    double v
) const noexcept {
    const double denominator =
        can_project_perspective_
            ? g_ * u + h_ * v + 1.0
            : 1.0;

    if (std::abs(denominator) <= std::numeric_limits<double>::epsilon()) {
        return Vec2d(
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN()
        );
    }

    return Vec2d(
        (a_ * u + b_ * v) / denominator + points_[0].x,
        (d_ * u + e_ * v) / denominator + points_[0].y
    );
}

Vec2d Quadrilateral::project(const Vec2d& uv) const noexcept {
    return project(uv.x, uv.y);
}

bool Quadrilateral::project(
    const Vec2d& uv,
    Vec2d& result
) const noexcept {
    result = project(uv);
    return std::isfinite(result.x) && std::isfinite(result.y);
}

bool Quadrilateral::map(
    double x,
    double y,
    Vec2d& result
) const noexcept {
    const double dx = x - points_[0].x;
    const double dy = y - points_[0].y;

    const double gu = can_project_perspective_ ? g_ : 0.0;
    const double hv = can_project_perspective_ ? h_ : 0.0;

    const double aa = dx * gu - a_;
    const double ab = dx * hv - b_;
    const double ba = dy * gu - d_;
    const double bb = dy * hv - e_;

    const double determinant =
        aa * bb - ab * ba;

    constexpr double epsilon =
        std::numeric_limits<double>::epsilon();

    if (std::abs(determinant) <= epsilon) {
        return false;
    }

    const double u =
        (ab * dy - dx * bb) / determinant;

    const double v =
        (dx * ba - aa * dy) / determinant;

    result = Vec2d(u, v);

    return std::isfinite(u) && std::isfinite(v);
}

Vec2d Quadrilateral::map(const Vec2d& position) const noexcept {
    Vec2d result;

    if (!map(position.x, position.y, result)) {
        return Vec2d(
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN()
        );
    }

    return result;
}

Vec2d Quadrilateral::map(
    double x,
    double y
) const noexcept
{
    Vec2d result;

    if (!map(x, y, result)) {
        const auto nan = std::numeric_limits<double>::quiet_NaN();
        return Vec2d{nan, nan};
    }

    return result;
}

void Quadrilateral::projectPoints(
    Vec2d* points,
    std::size_t count
) const noexcept {
    if (points == nullptr) {
        return;
    }

    for (std::size_t i = 0; i < count; ++i) {
        points[i] = project(points[i]);
    }
}


//------------------------------------------------------------------------------
// Lines
//------------------------------------------------------------------------------

Lined Quadrilateral::horizontalLine(double v) const noexcept {
    return Lined(
        project(0.0, v),
        project(1.0, v)
    );
}

Lined Quadrilateral::verticalLine(double u) const noexcept {
    return Lined(
        project(u, 0.0),
        project(u, 1.0)
    );
}


//------------------------------------------------------------------------------
// Bézier helpers
//------------------------------------------------------------------------------

std::array<Vec2d, 12>
Quadrilateral::bezierCirclePoints() const noexcept {
    constexpr double kappa = 0.551915024494;

    const double a = 0.0;
    const double b = 0.5 - kappa / 2.0;
    const double c = 0.5;
    const double d = 0.5 + kappa / 2.0;
    const double e = 1.0;

    return {
        project(c, a),
        project(d, a),

        project(e, b),
        project(e, c),
        project(e, d),

        project(d, e),
        project(c, e),

        project(b, e),
        project(a, d),

        project(a, c),
        project(a, b),

        project(b, a)
    };
}


//------------------------------------------------------------------------------
// Transformation
//------------------------------------------------------------------------------

void Quadrilateral::remap(const RemapRectd& rect) noexcept {
    for (Vec2d& point : points_) {
        rect.mapInPlace(point);
    }

    valid_ = true;
    solvePerspective();
}


//------------------------------------------------------------------------------
// Perspective transformation
//------------------------------------------------------------------------------

bool Quadrilateral::solvePerspective() noexcept {
    can_project_perspective_ = false;

    const double x1 = points_[0].x;
    const double x2 = points_[1].x;
    const double x3 = points_[2].x;
    const double x4 = points_[3].x;

    const double y1 = points_[0].y;
    const double y2 = points_[1].y;
    const double y3 = points_[2].y;
    const double y4 = points_[3].y;

    const double t =
        (x3 - x2) * (y3 - y4) -
        (x3 - x4) * (y3 - y2);

    if (std::abs(t) <= std::numeric_limits<double>::epsilon()) {
        return false;
    }

    g_ =
        ((x3 - x1) * (y3 - y4) -
         (x3 - x4) * (y3 - y1)) / t;

    h_ =
        ((x3 - x2) * (y3 - y1) -
         (x3 - x1) * (y3 - y2)) / t;

    a_ = g_ * (x2 - x1);
    d_ = g_ * (y2 - y1);

    b_ = h_ * (x4 - x1);
    e_ = h_ * (y4 - y1);

    g_ -= 1.0;
    h_ -= 1.0;

    can_project_perspective_ = true;

    return true;
}

} // namespace Grain