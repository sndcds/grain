#include <grain/Geometry/Bezier.hpp>

#include <algorithm>
#include <cmath>
#include <cfloat>
#include <limits>
#include <new>
#include <ostream>
#include <vector>

namespace Grain {

Bezier::Bezier(const Vec2d& p0, const Vec2d& p1, const Vec2d& p2, const Vec2d& p3) {
    set(p0, p1, p2, p3);
}


Bezier::Bezier(const Vec2d& p0, const Vec2d& p1, const Vec2d& p2 ) {
    setQuadratic(p0, p1, p2);
}


Bezier::Bezier(const Vec2d* positions) {
    if (positions == nullptr) {
        return;
    }

    pos_[0] = positions[0];
    pos_[1] = positions[1];
    pos_[2] = positions[2];
    pos_[3] = positions[3];
}


Bezier::Bezier(const BezierValueCurvePoint& p0, const BezierValueCurvePoint& p1) {

    /* TODO: Implement!
    pos_[0] = p0.pos_;
    pos_[1] = p0.usesRightControl() ?  p0.used_right_pos_ : p0.pos_;
    pos_[2] = p1.usesLeftControl() ? p1.used_left_pos_ : p1.pos_;
    pos_[3] = p1.pos_;
    */
    (void)p0;
    (void)p1;
}


auto operator<<(std::ostream& os, const Bezier& bezier) -> std::ostream& {
    os  << bezier.pos_[0] << " .. "
        << bezier.pos_[1] << " .. "
        << bezier.pos_[2] << " .. "
        << bezier.pos_[3];

    return os;
}



auto Bezier::startPos() const -> Vec2d {
    return pos_[0];
}


auto Bezier::controlPos1() const -> Vec2d {
    return pos_[1];
}


auto Bezier::controlPos2() const -> Vec2d {
    return pos_[2];
}


auto Bezier::endPos() const -> Vec2d {
    return pos_[3];
}


auto Bezier::posAtPointIndex(size_t index) const -> Vec2d {
    index = std::clamp(index, size_t{0}, size_t{3});
    return pos_[index];
}


auto Bezier::posOnCurve(double t) const -> Vec2d {
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


auto Bezier::bounds() const -> Rectd {
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
    ) -> void {
        const double a = -p0 + (3 * p1) - (3 * p2 + p3);
        const double b = 2 * (p0 - 2.0 * p1 + p2);
        const double c = p1 - p0;
        const double discriminant = (b * b) - (4 * a * c);

        auto add = [&](double t) -> void {
            if (t <= 0.0 || t >= 1.0) {
                return;
            }

            const double value = evaluate(t, p0, p1, p2, p3);

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

        const double sqrtDiscriminant = std::sqrt(discriminant);

        add((-b + sqrtDiscriminant) / (2 * a));
        add((-b - sqrtDiscriminant) / (2 * a));
    };

    updateAxis(p0.x, p1.x, p2.x, p3.x, minX, maxX);
    updateAxis(p0.y, p1.y, p2.y, p3.y, minY, maxY);

    return {minX, minY, maxX - minX, maxY - minY};
}


auto Bezier::approximatedCurveLength(int32_t resolution) const -> double {
    resolution = std::max(resolution, int32_t{2});

    const double step = 1.0 / static_cast<double>(resolution);
    double length = 0.0;

    Vec2d previous = posOnCurve(0.0);

    for (int32_t i = 1; i <= resolution; ++i) {
        const double t = step * i;
        const Vec2d current = posOnCurve(t);

        length += current.distance(previous);
        previous = current;
    }

    return length;
}


auto Bezier::tangent1() const -> Vec2d {
    return pos_[1] - pos_[0];
}


auto Bezier::tangent2() const -> Vec2d {
    return pos_[2] - pos_[3];
}


void Bezier::set(
    const Vec2d& p0,
    const Vec2d& p1,
    const Vec2d& p2,
    const Vec2d& p3
) {
    pos_[0] = p0;
    pos_[1] = p1;
    pos_[2] = p2;
    pos_[3] = p3;
}


void Bezier::setQuadratic(
    const Vec2d& p0,
    const Vec2d& p1,
    const Vec2d& p2
) {
    pos_[0] = p0;
    pos_[1] = p0 + (p1 - p0) * 2.0 / 3.0;
    pos_[2] = p2 + (p1 - p2) * 2.0 / 3.0;
    pos_[3] = p2;
}


void Bezier::set(
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3
) {
    pos_[0].set(x0, y0);
    pos_[1].set(x1, y1);
    pos_[2].set(x2, y2);
    pos_[3].set(x3, y3);
}


void Bezier::setPointAtIndex(size_t index, const Vec2d& position) {
    if (index > 3) {
        return;
    }

    pos_[index] = position;
}


void Bezier::setHorizontalSegment(
    const Vec2d& left,
    const Vec2d& right,
    const Vec2d& leftFactor,
    const Vec2d& rightFactor
) {
    const double width = right.x - left.x;
    const double height = right.y - left.y;

    pos_[0] = left;

    pos_[1].x = left.x + (leftFactor.x * width);
    pos_[1].y = left.y + (leftFactor.y * height);

    pos_[2].x = right.x - (rightFactor.x * width);
    pos_[2].y = right.y - (rightFactor.y * height);

    pos_[3] = right;
}


void Bezier::setWithTangents(
    const Vec2d& start,
    const Vec2d& end,
    const Vec2d& tangent1,
    const Vec2d& tangent2
) {
    const Vec2d chord = end - start;
    const double distance = chord.length() / 3.0;

    pos_[0] = start;
    pos_[1] = start + tangent1.normalized() * distance;
    pos_[2] = end - tangent2.normalized() * distance;
    pos_[3] = end;
}


auto Bezier::hitBounds(const Vec2d& position, double radius) const -> bool {
    return bounds().contains(position, radius);
}


auto Bezier::hitPoint(const Vec2d& position, double radius) const -> int32_t {
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


auto Bezier::hit(const Vec2d& position, double radius) const -> double {
    if (!hitBounds(position, radius)) {
        return std::numeric_limits<double>::max();
    }

    constexpr int32_t kResolution = 8;
    constexpr int32_t kIterations = 16;

    double start = 0.0;
    double end = 1.0;
    double result = std::numeric_limits<double>::max();

    for (int32_t iteration = 0; iteration <= kIterations; ++iteration) {
        const double step =(end - start) / kResolution;

        double minimum_distance = std::numeric_limits<double>::max();
        int32_t minimum_index = -1;

        for (int32_t i = 0; i <= kResolution; ++i) {
            const double t = start + (step * i);

            const double distance =
                position.distance(posOnCurve(t));

            if (distance < minimum_distance) {
                minimum_distance = distance;
                minimum_index = i;
                result = t;
            }
        }

        if (minimum_index < 0) {
            return std::numeric_limits<double>::max();
        }

        if (iteration == kIterations && minimum_distance > radius) {
            return std::numeric_limits<double>::max();
        }

        start = std::clamp(result - step, 0.0, 1.0);
        end = std::clamp(result + step, 0.0, 1.0);
    }

    return result;
}


auto Bezier::split(double t, Bezier& left, Bezier& right) const -> bool {
    t = std::clamp(t, 0.0, 1.0);

    const Vec2d a = pos_[0] + (pos_[1] - pos_[0]) * t;
    const Vec2d b = pos_[1] + (pos_[2] - pos_[1]) * t;
    const Vec2d c = pos_[2] + (pos_[3] - pos_[2]) * t;

    const Vec2d ab = a + (b - a) * t;
    const Vec2d bc = b + (c - b) * t;

    const Vec2d abc = ab + (bc - ab) * t;

    left.set(pos_[0], a, ab, abc);
    right.set(abc, bc, c, pos_[3]);

    return true;
}


auto Bezier::truncate(double start, double end, Bezier& result) const -> bool {
    start = std::clamp(start, 0.0, 1.0);
    end = std::clamp(end, 0.0, 1.0);

    if (start >= end) {
        return false;
    }

    Bezier first;
    Bezier second;

    split(end, first, second);

    Bezier prefix;
    const double local_start = end > 0.0 ? start / end : 0.0;
    first.split(local_start, prefix, result);

    return true;
}

void Bezier::buildVec2LUT(Vec2d* lut, int32_t resolution) const {
    if (lut == nullptr || resolution < 2) {
        return;
    }

    for (int32_t i = 0; i < resolution; ++i) {
        const double t =
            static_cast<double>(i) /
            static_cast<double>(resolution - 1);

        lut[i] = posOnCurve(t);
    }
}


void Bezier::translate(double x,double y) {
    translate(Vec2d{x, y});
}


void Bezier::translate(const Vec2d& offset) {
    for (auto& position : pos_) {
        position += offset;
    }
}


void Bezier::translateX(double x) {
    for (auto& position : pos_) {
        position.x += x;
    }
}


void Bezier::translateY(double y) {
    for (auto& position : pos_) {
        position.y += y;
    }
}


void Bezier::scale(double x, double y) {
    scale(Vec2d{x, y});
}


void Bezier::scale(const Vec2d& scale) {
    for (auto& position : pos_) {
        position *= scale;
    }
}


void Bezier::scaleX(double x) {
    for (auto& position : pos_) {
        position.x *= x;
    }
}


void Bezier::scaleY(double y) {
    for (auto& position : pos_) {
        position.y *= y;
    }
}


void Bezier::transformByRect(const Rectd& rect) {
    scale(rect.width,rect.height);
    translate(rect.x,rect.y);
}


void Bezier::approximateQuadraticBezierControlPos(Vec2d& control) const {
    control =
        (pos_[1] + pos_[2]) * (3.0 / 4.0) -
        (pos_[0] + pos_[3]) * (1.0 / 4.0);
}


auto Bezier::arcToBezierPosArray(
    const Vec2d& start,
    const Vec2d& radii,
    double rotation,
    bool large_arc_flag,
    bool sweep_flag,
    const Vec2d& end,
    int32_t max_segment_count,
    Vec2d* output
) -> int32_t {
    if (
        output == nullptr ||
        max_segment_count <= 0 ||
        radii.x == 0.0 ||
        radii.y == 0.0 ||
        start.distance(end) < std::numeric_limits<double>::epsilon()
    ) {
        return 0;
    }

    const double rx_initial = std::abs(radii.x);
    const double ry_initial = std::abs(radii.y);

    double rx = rx_initial;
    double ry = ry_initial;

    const double dx = (start.x - end.x) / 2.0;
    const double dy = (start.y - end.y) / 2.0;

    const double rotation_radians = rotation * std::numbers::pi / 180.0;
    const double cos_rotation = std::cos(rotation_radians);
    const double sin_rotation = std::sin(rotation_radians);

    const double x1p = (cos_rotation * dx) + (sin_rotation * dy);
    const double y1p = (-sin_rotation * dx) + (cos_rotation * dy);

    double rx2 = rx * rx;
    double ry2 = ry * ry;

    const double x1p2 = x1p * x1p;
    const double y1p2 = y1p * y1p;

    const double radius_check = (x1p2 / rx2) + (y1p2 / ry2);
    if (radius_check > 1.0) {
        const double scale = std::sqrt(radius_check);

        rx *= scale;
        ry *= scale;

        rx2 = rx * rx;
        ry2 = ry * ry;
    }

    const double denominator = (rx2 * y1p2) + (ry2 * x1p2);
    if (denominator <= 0.0) {
        return 0;
    }

    const double sign = large_arc_flag != sweep_flag ? 1.0 : -1.0;

    double factor =((rx2 * ry2) - (rx2 * y1p2) - (ry2 * x1p2)) / denominator;
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
        (cos_rotation * cxp) -
        (sin_rotation * cyp) +
        ((start.x + end.x) / 2);

    const double cy =
        (sin_rotation * cxp) +
        (cos_rotation * cyp) +
        ((start.y + end.y) / 2);

    const auto vectorAngle = [](
        double ux,
        double uy,
        double vx,
        double vy
    ) -> double {
        const double denominator =
            std::sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
        if (denominator <= 0.0) {
            return 0.0;
        }

        const double cosine =
            std::clamp((ux * vx + uy * vy) / denominator, -1.0, 1.0);

        double angle = std::acos(cosine);

        if ((ux * vy) - (uy * vx) < 0.0) {
            angle = -angle;
        }

        return angle;
    };

    const double theta =
        vectorAngle(1.0, 0.0, (x1p - cxp) / rx, (y1p - cyp) / ry);

    double deltaTheta =
        vectorAngle(
            (x1p - cxp) / rx,
            (y1p - cyp) / ry,
            (-x1p - cxp) / rx,
            (-y1p - cyp) / ry
        );

    if (!sweep_flag && deltaTheta > 0.0) {
        deltaTheta -= std::numbers::pi * 2;
    }
    else if (sweep_flag && deltaTheta < 0.0) {
        deltaTheta += std::numbers::pi * 2;
    }

    auto segmentCount =
        static_cast<int32_t>(
            std::ceil(std::abs(deltaTheta) / (std::numbers::pi / 2))
        );

    segmentCount = std::min(segmentCount, max_segment_count);

    if (segmentCount <= 0) {
        return 0;
    }

    const double sweep = deltaTheta / segmentCount;

    const double alpha =
        std::sin(sweep / 2) *
        4.0 / 3.0 /
        (1.0 + std::cos(sweep / 2));

    Vec2d* p = output;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const double t0 = theta + (sweep * i);
        const double t1 = t0 + sweep;

        const double cos0 = std::cos(t0);
        const double sin0 = std::sin(t0);
        const double cos1 = std::cos(t1);
        const double sin1 = std::sin(t1);

        Vec2d p0;
        Vec2d p1;
        Vec2d p2;
        Vec2d p3;

        p0.x = cx + (cos_rotation * (rx * cos0)) - (sin_rotation * (ry * sin0));
        p0.y = cy + (sin_rotation * (rx * cos0)) + (cos_rotation * (ry * sin0));

        p3.x = cx + (cos_rotation * (rx * cos1)) - (sin_rotation * (ry * sin1));
        p3.y = cy + (sin_rotation * (rx * cos1)) + (cos_rotation * (ry * sin1));

        p1.x = p0.x - (alpha * cos_rotation * (rx * sin0)) - (alpha * sin_rotation * (ry * cos0));
        p1.y = p0.y - (alpha * sin_rotation * (rx * sin0)) + (alpha * cos_rotation * (ry * cos0));

        p2.x = p3.x + (alpha * cos_rotation * (rx * sin1)) + (alpha * sin_rotation * (ry * cos1));
        p2.y = p3.y + (alpha * sin_rotation * (rx * sin1)) - (alpha * cos_rotation * (ry * cos1));

        if (i == 0) {
            *p++ = p0;
        }

        *p++ = p1;
        *p++ = p2;
        *p++ = p3;
    }

    return segmentCount;
}


auto Bezier::evaluate(
    double t,
    double p0,
    double p1,
    double p2,
    double p3
) -> double {
    const double u = 1.0 - t;

    return
        (u * u * u * p0) +
        (u * u * t * p1 * 3) +
        (u * t * t * p2 * 3) +
        (t * t * t * p3);
}


auto Bezier::fitCubicBezierToPoints(
    std::span<const Vec2d> points
) -> std::optional<Bezier> {
    constexpr std::size_t kMaxPointCount = 2048;

    if (points.size() < 2 || points.size() > kMaxPointCount) {
        return std::nullopt;
    }

    Bezier result;

    const Vec2d p0 = points.front();
    const Vec2d p3 = points.back();

    // Degenerate case: all points are identical.
    double total_length = 0.0;

    for (std::size_t i = 1; i < points.size(); ++i) {
        total_length += (points[i] - points[i - 1]).length();
    }

    if (total_length <= std::numeric_limits<double>::epsilon()) {
        result.set(p0, p0, p3, p3);
        return result;
    }

    // Chord-length parameterization.
    std::vector<double> t(points.size());
    t[0] = 0.0;

    for (std::size_t i = 1; i < points.size(); ++i) {
        t[i] = t[i - 1] + ((points[i] - points[i - 1]).length() / total_length);
    }

    t.back() = 1.0;

    const Vec2d tangent_start = points[1] - points[0];
    const Vec2d tangent_end = points[points.size() - 2] - points.back();

    const double tangent_start_length = tangent_start.length();
    const double tangent_end_length = tangent_end.length();

    // If either endpoint tangent is degenerate, fall back to a straight
    // line between the first and last point.
    if (tangent_start_length <= std::numeric_limits<double>::epsilon() ||
        tangent_end_length <= std::numeric_limits<double>::epsilon()) {

        const Vec2d delta = p3 - p0;

        result.set(
            p0,
            p0 + delta / 3,
            p0 + (delta * 2 / 3),
            p3);

        return result;
    }

    const Vec2d unit_tangent_start = tangent_start / tangent_start_length;
    const Vec2d unit_tangent_end = tangent_end / tangent_end_length;

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

        const Vec2d a1 = unit_tangent_start * b1;
        const Vec2d a2 = unit_tangent_end * b2;

        c00 += a1.dot(a1);
        c01 += a1.dot(a2);
        c11 += a2.dot(a2);

        const Vec2d tmp = points[i] - (p0 * (b0 + b1)) - (p3 * (b2 + b3));

        x0 += a1.dot(tmp);
        x1 += a2.dot(tmp);
    }

    const double determinant = (c00 * c11) - (c01 * c01);

    if (std::abs(determinant) <= 1e-10) {
        // Singular system. Use a straight-line cubic as a robust fallback.
        const Vec2d delta = p3 - p0;

        result.set(
            p0,
            p0 + delta / 3,
            p0 + delta * 2 / 3,
            p3
        );

        return result;
    }

    const double alpha1 = (x0 * c11 - x1 * c01) / determinant;
    const double alpha2 = (c00 * x1 - c01 * x0) / determinant;

    // Negative or extremely small handle lengths indicate that the
    // least-squares solution is not geometrically useful. Fall back to
    // a straight-line cubic.
    if (alpha1 <= std::numeric_limits<double>::epsilon() ||
        alpha2 <= std::numeric_limits<double>::epsilon()) {

        const Vec2d delta = p3 - p0;

        result.set(
            p0,
            p0 + delta / 3,
            p0 + delta * 2 / 3,
            p3
        );

        return result;
    }

    result.set(
        p0,
        p0 + unit_tangent_start * alpha1,
        p3 + unit_tangent_end * alpha2,
        p3
    );

    return result;
}

} // namespace Grain