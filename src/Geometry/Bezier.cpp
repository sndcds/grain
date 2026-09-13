#include <grain/Geometry/Bezier.hpp>

#include <algorithm>
#include <cmath>
#include <cfloat>
#include <limits>
#include <new>
#include <ostream>
#include <vector>

namespace Grain {

Bezier::Bezier(
    double x0,
    double y0,
    double x1,
    double y1,
    double x2,
    double y2,
    double x3,
    double y3
) noexcept
{
    pos_[0].set(x0, y0);
    pos_[1].set(x1, y1);
    pos_[2].set(x2, y2);
    pos_[3].set(x3, y3);
}


Bezier::Bezier(
    const Vec2d& p0,
    const Vec2d& p1,
    const Vec2d& p2,
    const Vec2d& p3
) noexcept
{
    set(p0, p1, p2, p3);
}


Bezier::Bezier(
    const Vec2d& p0,
    const Vec2d& p1,
    const Vec2d& p2
) noexcept
{
    setQuadratic(p0, p1, p2);
}


Bezier::Bezier(const Vec2d* positions) noexcept
{
    if (!positions) {
        return;
    }

    pos_[0] = positions[0];
    pos_[1] = positions[1];
    pos_[2] = positions[2];
    pos_[3] = positions[3];
}


Bezier::Bezier(
    const BezierValueCurvePoint& p0,
    const BezierValueCurvePoint& p1
) noexcept
{
    // This constructor should be implemented together with the
    // new BezierValueCurvePoint API.
    //
    // Do not access its old internal members here.
    //
    // For now this intentionally remains unavailable until
    // BezierValueCurvePoint has been migrated.
    (void)p0;
    (void)p1;
}


Vec2d Bezier::startPos() const noexcept
{
    return pos_[0];
}


Vec2d Bezier::controlPos1() const noexcept
{
    return pos_[1];
}


Vec2d Bezier::controlPos2() const noexcept
{
    return pos_[2];
}


Vec2d Bezier::endPos() const noexcept
{
    return pos_[3];
}


Vec2d Bezier::posAtPointIndex(int32_t index) const noexcept
{
    index = std::clamp(index, int32_t{0}, int32_t{3});
    return pos_[index];
}


Vec2d Bezier::posOnCurve(double t) const noexcept
{
    const double u = 1.0 - t;

    const double u2 = u * u;
    const double t2 = t * t;

    const double b0 = u * u2;
    const double b1 = 3.0 * u2 * t;
    const double b2 = 3.0 * u * t2;
    const double b3 = t * t2;

    return
        pos_[0] * b0 +
        pos_[1] * b1 +
        pos_[2] * b2 +
        pos_[3] * b3;
}


Rectd Bezier::bounds() const noexcept
{
    const Vec2d& p0 = pos_[0];
    const Vec2d& p1 = pos_[1];
    const Vec2d& p2 = pos_[2];
    const Vec2d& p3 = pos_[3];

    double minX = std::min(p0.x, p3.x);
    double maxX = std::max(p0.x, p3.x);
    double minY = std::min(p0.y, p3.y);
    double maxY = std::max(p0.y, p3.y);

    auto updateAxis = [](
        double p0,
        double p1,
        double p2,
        double p3,
        double& minValue,
        double& maxValue
    ) {
        const double a =
            -p0 + 3.0 * p1 - 3.0 * p2 + p3;

        const double b =
            2.0 * (p0 - 2.0 * p1 + p2);

        const double c =
            p1 - p0;

        const double discriminant =
            b * b - 4.0 * a * c;

        auto add = [&](double t) {
            if (t <= 0.0 || t >= 1.0) {
                return;
            }

            const double value = evaluate(
                t,
                p0,
                p1,
                p2,
                p3
            );

            minValue = std::min(minValue, value);
            maxValue = std::max(maxValue, value);
        };

        if (std::abs(a) < std::numeric_limits<double>::epsilon()) {
            if (std::abs(b) >= std::numeric_limits<double>::epsilon()) {
                add(-c / b);
            }
            return;
        }

        if (discriminant < 0.0) {
            return;
        }

        const double sqrtDiscriminant =
            std::sqrt(discriminant);

        add((-b + sqrtDiscriminant) / (2.0 * a));
        add((-b - sqrtDiscriminant) / (2.0 * a));
    };

    updateAxis(
        p0.x,
        p1.x,
        p2.x,
        p3.x,
        minX,
        maxX
    );

    updateAxis(
        p0.y,
        p1.y,
        p2.y,
        p3.y,
        minY,
        maxY
    );

    return Rectd(
        minX,
        minY,
        maxX - minX,
        maxY - minY
    );
}


double Bezier::approximatedCurveLength(
    int32_t resolution
) const noexcept
{
    resolution = std::max(resolution, int32_t{2});

    const double step =
        1.0 / static_cast<double>(resolution);

    Vec2d previous = posOnCurve(0.0);

    double length = 0.0;

    for (int32_t i = 1; i <= resolution; ++i) {
        const double t =
            static_cast<double>(i) * step;

        const Vec2d current = posOnCurve(t);

        length += current.distance(previous);
        previous = current;
    }

    return length;
}


Vec2d Bezier::tangent1() const noexcept
{
    return pos_[1] - pos_[0];
}


Vec2d Bezier::tangent2() const noexcept
{
    return pos_[2] - pos_[3];
}


void Bezier::set(
    const Vec2d& p0,
    const Vec2d& p1,
    const Vec2d& p2,
    const Vec2d& p3
) noexcept
{
    pos_[0] = p0;
    pos_[1] = p1;
    pos_[2] = p2;
    pos_[3] = p3;
}


void Bezier::setQuadratic(
    const Vec2d& p0,
    const Vec2d& p1,
    const Vec2d& p2
) noexcept
{
    pos_[0] = p0;
    pos_[1] = p0 + (p1 - p0) * (2.0 / 3.0);
    pos_[2] = p2 + (p1 - p2) * (2.0 / 3.0);
    pos_[3] = p2;
}


void Bezier::set(
    double x0,
    double y0,
    double x1,
    double y1,
    double x2,
    double y2,
    double x3,
    double y3
) noexcept
{
    pos_[0].set(x0, y0);
    pos_[1].set(x1, y1);
    pos_[2].set(x2, y2);
    pos_[3].set(x3, y3);
}


void Bezier::setPointAtIndex(
    int32_t index,
    const Vec2d& position
) noexcept
{
    if (index < 0 || index > 3) {
        return;
    }

    pos_[index] = position;
}


void Bezier::setHorizontalSegment(
    const Vec2d& left,
    const Vec2d& right,
    const Vec2d& leftFactor,
    const Vec2d& rightFactor
) noexcept
{
    const double width = right.x - left.x;
    const double height = right.y - left.y;

    pos_[0] = left;

    pos_[1].x = left.x + leftFactor.x * width;
    pos_[1].y = left.y + leftFactor.y * height;

    pos_[2].x = right.x - rightFactor.x * width;
    pos_[2].y = right.y - rightFactor.y * height;

    pos_[3] = right;
}


void Bezier::setWithTangents(
    const Vec2d& start,
    const Vec2d& end,
    const Vec2d& tangent1,
    const Vec2d& tangent2
) noexcept
{
    const Vec2d chord = end - start;
    const double distance = chord.length() / 3.0;

    pos_[0] = start;
    pos_[1] = start + tangent1.normalized() * distance;
    pos_[2] = end - tangent2.normalized() * distance;
    pos_[3] = end;
}


bool Bezier::hitBounds(
    const Vec2d& position,
    double radius
) const noexcept
{
    return bounds().contains(position, radius);
}


int32_t Bezier::hitPoint(
    const Vec2d& position,
    double radius
) const noexcept
{
    int32_t result = -1;
    double minimumDistance = std::numeric_limits<double>::max();

    for (int32_t i = 0; i < 4; ++i) {
        const double distance = position.distance(pos_[i]);

        if (distance < minimumDistance) {
            minimumDistance = distance;
            result = i;
        }
    }

    return minimumDistance < radius ? result : -1;
}


double Bezier::hit(
    const Vec2d& position,
    double radius
) const noexcept
{
    if (!hitBounds(position, radius)) {
        return std::numeric_limits<double>::max();
    }

    constexpr int32_t resolution = 8;
    constexpr int32_t iterations = 16;

    double start = 0.0;
    double end = 1.0;
    double result = std::numeric_limits<double>::max();

    for (int32_t iteration = 0; iteration <= iterations; ++iteration) {
        const double step =
            (end - start) /
            static_cast<double>(resolution);

        double minimumDistance =
            std::numeric_limits<double>::max();

        int32_t minimumIndex = -1;

        for (int32_t i = 0; i <= resolution; ++i) {
            const double t =
                start + static_cast<double>(i) * step;

            const double distance =
                position.distance(posOnCurve(t));

            if (distance < minimumDistance) {
                minimumDistance = distance;
                minimumIndex = i;
                result = t;
            }
        }

        if (minimumIndex < 0) {
            return std::numeric_limits<double>::max();
        }

        if (
            iteration == iterations &&
            minimumDistance > radius
        ) {
            return std::numeric_limits<double>::max();
        }

        start = std::clamp(
            result - step,
            0.0,
            1.0
        );

        end = std::clamp(
            result + step,
            0.0,
            1.0
        );
    }

    return result;
}


bool Bezier::split(
    double t,
    Bezier& left,
    Bezier& right
) const noexcept
{
    t = std::clamp(t, 0.0, 1.0);

    const Vec2d a =
        pos_[0] + (pos_[1] - pos_[0]) * t;

    const Vec2d b =
        pos_[1] + (pos_[2] - pos_[1]) * t;

    const Vec2d c =
        pos_[2] + (pos_[3] - pos_[2]) * t;

    const Vec2d ab =
        a + (b - a) * t;

    const Vec2d bc =
        b + (c - b) * t;

    const Vec2d abc =
        ab + (bc - ab) * t;

    left.set(
        pos_[0],
        a,
        ab,
        abc
    );

    right.set(
        abc,
        bc,
        c,
        pos_[3]
    );

    return true;
}


bool Bezier::truncate(
    double start,
    double end,
    Bezier& result
) const noexcept
{
    start = std::clamp(start, 0.0, 1.0);
    end = std::clamp(end, 0.0, 1.0);

    if (start >= end) {
        return false;
    }

    Bezier first;
    Bezier second;

    split(end, first, second);

    const double localEnd =
        end > 0.0 ? (end - start) / end : 0.0;

    Bezier prefix;
    first.split(
        localEnd,
        prefix,
        result
    );

    return true;
}


void Bezier::buildVec2LUT(
    Vec2d* lut,
    int32_t resolution
) const noexcept
{
    if (!lut || resolution <= 1) {
        return;
    }

    for (int32_t i = 0; i < resolution; ++i) {
        const double t =
            static_cast<double>(i) /
            static_cast<double>(resolution - 1);

        lut[i] = posOnCurve(t);
    }
}


void Bezier::translate(
    double x,
    double y
) noexcept
{
    translate(Vec2d{x, y});
}


void Bezier::translate(
    const Vec2d& offset
) noexcept
{
    for (auto& position : pos_) {
        position += offset;
    }
}


void Bezier::translateX(double x) noexcept
{
    for (auto& position : pos_) {
        position.x += x;
    }
}


void Bezier::translateY(double y) noexcept
{
    for (auto& position : pos_) {
        position.y += y;
    }
}


void Bezier::scale(
    double x,
    double y
) noexcept
{
    scale(Vec2d{x, y});
}


void Bezier::scale(
    const Vec2d& scale
) noexcept
{
    for (auto& position : pos_) {
        position *= scale;
    }
}


void Bezier::scaleX(double x) noexcept
{
    for (auto& position : pos_) {
        position.x *= x;
    }
}


void Bezier::scaleY(double y) noexcept
{
    for (auto& position : pos_) {
        position.y *= y;
    }
}


void Bezier::transformByRect(
    const Rectd& rect
) noexcept
{
    scale(rect.width,rect.height);
    translate(rect.x,rect.y);
}


void Bezier::approximateQuadraticBezierControlPos(
    Vec2d& control
) const noexcept
{
    control =
        (pos_[1] + pos_[2]) * (3.0 / 4.0) -
        (pos_[0] + pos_[3]) * (1.0 / 4.0);
}


int32_t Bezier::arcToBezierPosArray(
    const Vec2d& start,
    const Vec2d& radii,
    double rotation,
    bool largeArcFlag,
    bool sweepFlag,
    const Vec2d& end,
    int32_t maxSegmentCount,
    Vec2d* output
) noexcept
{
    if (
        !output ||
        maxSegmentCount <= 0 ||
        radii.x == 0.0 ||
        radii.y == 0.0 ||
        start.distance(end) <
            std::numeric_limits<double>::epsilon()
    ) {
        return 0;
    }

    const double rxInitial = std::abs(radii.x);
    const double ryInitial = std::abs(radii.y);

    double rx = rxInitial;
    double ry = ryInitial;

    const double dx = (start.x - end.x) / 2.0;
    const double dy = (start.y - end.y) / 2.0;

    const double rotationRadians =
        rotation * std::numbers::pi / 180.0;

    const double cosRotation =
        std::cos(rotationRadians);

    const double sinRotation =
        std::sin(rotationRadians);

    const double x1p =
        cosRotation * dx +
        sinRotation * dy;

    const double y1p =
        -sinRotation * dx +
        cosRotation * dy;

    double rx2 = rx * rx;
    double ry2 = ry * ry;

    const double x1p2 = x1p * x1p;
    const double y1p2 = y1p * y1p;

    const double radiusCheck =
        x1p2 / rx2 +
        y1p2 / ry2;

    if (radiusCheck > 1.0) {
        const double scale =
            std::sqrt(radiusCheck);

        rx *= scale;
        ry *= scale;

        rx2 = rx * rx;
        ry2 = ry * ry;
    }

    const double denominator =
        rx2 * y1p2 +
        ry2 * x1p2;

    if (denominator <= 0.0) {
        return 0;
    }

    const double sign =
        largeArcFlag != sweepFlag ? 1.0 : -1.0;

    double factor =
        ((rx2 * ry2) -
         (rx2 * y1p2) -
         (ry2 * x1p2)) /
        denominator;

    factor = std::max(0.0, factor);

    const double cxp =
        sign *
        std::sqrt(factor) *
        (rx * y1p / ry);

    const double cyp =
        sign *
        std::sqrt(factor) *
        (-ry * x1p / rx);

    const double cx =
        cosRotation * cxp -
        sinRotation * cyp +
        (start.x + end.x) / 2.0;

    const double cy =
        sinRotation * cxp +
        cosRotation * cyp +
        (start.y + end.y) / 2.0;

    const auto vectorAngle = [](
        double ux,
        double uy,
        double vx,
        double vy
    ) noexcept {
        const double denominator =
            std::sqrt(
                (ux * ux + uy * uy) *
                (vx * vx + vy * vy)
            );

        if (denominator <= 0.0) {
            return 0.0;
        }

        const double cosine =
            std::clamp(
                (ux * vx + uy * vy) / denominator,
                -1.0,
                1.0
            );

        double angle = std::acos(cosine);

        if (ux * vy - uy * vx < 0.0) {
            angle = -angle;
        }

        return angle;
    };

    const double theta =
        vectorAngle(
            1.0,
            0.0,
            (x1p - cxp) / rx,
            (y1p - cyp) / ry
        );

    double deltaTheta =
        vectorAngle(
            (x1p - cxp) / rx,
            (y1p - cyp) / ry,
            (-x1p - cxp) / rx,
            (-y1p - cyp) / ry
        );

    if (!sweepFlag && deltaTheta > 0.0) {
        deltaTheta -= 2.0 * std::numbers::pi;
    }
    else if (sweepFlag && deltaTheta < 0.0) {
        deltaTheta += 2.0 * std::numbers::pi;
    }

    int32_t segmentCount =
        static_cast<int32_t>(
            std::ceil(
                std::abs(deltaTheta) /
                (std::numbers::pi / 2.0)
            )
        );

    segmentCount =
        std::min(segmentCount, maxSegmentCount);

    if (segmentCount <= 0) {
        return 0;
    }

    const double sweep =
        deltaTheta /
        static_cast<double>(segmentCount);

    const double alpha =
        std::sin(sweep / 2.0) *
        4.0 / 3.0 /
        (1.0 + std::cos(sweep / 2.0));

    Vec2d* p = output;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const double t0 =
            theta +
            static_cast<double>(i) * sweep;

        const double t1 = t0 + sweep;

        const double cos0 = std::cos(t0);
        const double sin0 = std::sin(t0);
        const double cos1 = std::cos(t1);
        const double sin1 = std::sin(t1);

        Vec2d p0;
        Vec2d p1;
        Vec2d p2;
        Vec2d p3;

        p0.x =
            cx +
            cosRotation * (rx * cos0) -
            sinRotation * (ry * sin0);

        p0.y =
            cy +
            sinRotation * (rx * cos0) +
            cosRotation * (ry * sin0);

        p3.x =
            cx +
            cosRotation * (rx * cos1) -
            sinRotation * (ry * sin1);

        p3.y =
            cy +
            sinRotation * (rx * cos1) +
            cosRotation * (ry * sin1);

        p1.x =
            p0.x -
            alpha *
                cosRotation *
                (rx * sin0) -
            alpha *
                sinRotation *
                (ry * cos0);

        p1.y =
            p0.y -
            alpha *
                sinRotation *
                (rx * sin0) +
            alpha *
                cosRotation *
                (ry * cos0);

        p2.x =
            p3.x +
            alpha *
                cosRotation *
                (rx * sin1) +
            alpha *
                sinRotation *
                (ry * cos1);

        p2.y =
            p3.y +
            alpha *
                sinRotation *
                (rx * sin1) -
            alpha *
                cosRotation *
                (ry * cos1);

        if (i == 0) {
            *p++ = p0;
        }

        *p++ = p1;
        *p++ = p2;
        *p++ = p3;
    }

    return segmentCount;
}


double Bezier::evaluate(
    double t,
    double p0,
    double p1,
    double p2,
    double p3
) noexcept
{
    const double u = 1.0 - t;

    return
        u * u * u * p0 +
        3.0 * u * u * t * p1 +
        3.0 * u * t * t * p2 +
        t * t * t * p3;
}


std::optional<Bezier> Bezier::fitCubicBezierToPoints(
    std::span<const Vec2d> points
)
{
    constexpr std::size_t maxPointCount = 2048;

    if (points.size() < 2 || points.size() > maxPointCount) {
        return std::nullopt;
    }

    Bezier result;

    const Vec2d p0 = points.front();
    const Vec2d p3 = points.back();

    // Degenerate case: all points are identical.
    double totalLength = 0.0;

    for (std::size_t i = 1; i < points.size(); ++i) {
        totalLength += (points[i] - points[i - 1]).length();
    }

    if (totalLength <= std::numeric_limits<double>::epsilon()) {
        result.set(p0, p0, p3, p3);
        return result;
    }

    // Chord-length parameterization.
    std::vector<double> t(points.size());
    t[0] = 0.0;

    for (std::size_t i = 1; i < points.size(); ++i) {
        t[i] = t[i - 1]
             + (points[i] - points[i - 1]).length() / totalLength;
    }

    t.back() = 1.0;

    const Vec2d tangentStart = points[1] - points[0];
    const Vec2d tangentEnd = points[points.size() - 2] - points.back();

    const double tangentStartLength = tangentStart.length();
    const double tangentEndLength = tangentEnd.length();

    // If either endpoint tangent is degenerate, fall back to a straight
    // line between the first and last point.
    if (tangentStartLength <= std::numeric_limits<double>::epsilon() ||
        tangentEndLength <= std::numeric_limits<double>::epsilon()) {

        const Vec2d delta = p3 - p0;

        result.set(
            p0,
            p0 + delta / 3.0,
            p0 + delta * (2.0 / 3.0),
            p3
        );

        return result;
    }

    const Vec2d unitTangentStart =
        tangentStart / tangentStartLength;

    const Vec2d unitTangentEnd =
        tangentEnd / tangentEndLength;

    double c00 = 0.0;
    double c01 = 0.0;
    double c11 = 0.0;

    double x0 = 0.0;
    double x1 = 0.0;

    for (std::size_t i = 0; i < points.size(); ++i) {
        const double u = t[i];
        const double oneMinusU = 1.0 - u;

        const double b0 = oneMinusU * oneMinusU * oneMinusU;
        const double b1 = 3.0 * oneMinusU * oneMinusU * u;
        const double b2 = 3.0 * oneMinusU * u * u;
        const double b3 = u * u * u;

        const Vec2d a1 = unitTangentStart * b1;
        const Vec2d a2 = unitTangentEnd * b2;

        c00 += a1.dot(a1);
        c01 += a1.dot(a2);
        c11 += a2.dot(a2);

        const Vec2d tmp =
            points[i]
            - p0 * (b0 + b1)
            - p3 * (b2 + b3);

        x0 += a1.dot(tmp);
        x1 += a2.dot(tmp);
    }

    const double determinant = c00 * c11 - c01 * c01;

    if (std::abs(determinant) <= 1e-10) {
        // Singular system. Use a straight-line cubic as a robust fallback.
        const Vec2d delta = p3 - p0;

        result.set(
            p0,
            p0 + delta / 3.0,
            p0 + delta * (2.0 / 3.0),
            p3
        );

        return result;
    }

    const double alpha1 =
        (x0 * c11 - x1 * c01) / determinant;

    const double alpha2 =
        (c00 * x1 - c01 * x0) / determinant;

    // Negative or extremely small handle lengths indicate that the
    // least-squares solution is not geometrically useful. Fall back to
    // a straight-line cubic.
    if (alpha1 <= std::numeric_limits<double>::epsilon() ||
        alpha2 <= std::numeric_limits<double>::epsilon()) {

        const Vec2d delta = p3 - p0;

        result.set(
            p0,
            p0 + delta / 3.0,
            p0 + delta * (2.0 / 3.0),
            p3
        );

        return result;
    }

    result.set(
        p0,
        p0 + unitTangentStart * alpha1,
        p3 + unitTangentEnd * alpha2,
        p3
    );

    return result;
}


std::ostream& operator<<(
    std::ostream& os,
    const Bezier& bezier
)
{
    os
        << bezier.pos_[0].x << ", "
        << bezier.pos_[0].y << " .. "
        << bezier.pos_[1].x << ", "
        << bezier.pos_[1].y << " .. "
        << bezier.pos_[2].x << ", "
        << bezier.pos_[2].y << " .. "
        << bezier.pos_[3].x << ", "
        << bezier.pos_[3].y;

    return os;
}

} // namespace Grain