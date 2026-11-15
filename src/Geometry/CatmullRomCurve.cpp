#include <grain/Geometry/CatmullRomCurve.hpp>

#include <algorithm>
#include <cmath>
#include <limits>


namespace Grain {


namespace {

constexpr double kMinimumKnotInterval = 1.0e-12;


} // anonymous namespace


//------------------------------------------------------------------------------
//  Construction
//------------------------------------------------------------------------------

CatmullRomCurve::CatmullRomCurve(int32_t point_capacity) noexcept {

    if (point_capacity > 0) {
        try {
            points_.reserve(static_cast<size_t>(point_capacity));
        }
        catch (...) {
            // The curve remains valid with its default capacity.
        }
    }
}


//------------------------------------------------------------------------------
//  Points
//------------------------------------------------------------------------------

auto CatmullRomCurve::addPoint(const Vec2d& point) noexcept -> Error {

    if (extended_) {
        return Error::InvalidState;
    }

    if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
        return Error::InvalidArgument;
    }

    if (!points_.empty()) {
        const auto& last_point = points_.back();

        if (last_point.distance(point) < point_distance_threshold_) {
            // Ignore points that are too close to the previous point.
            return Error::None;
        }
    }

    try {
        points_.push_back(point);
    }
    catch (...) {
        return Error::AllocationFailure;
    }

    return Error::None;
}


auto CatmullRomCurve::extend() noexcept -> Error {

    if (extended_) {
        return Error::InvalidState;
    }

    if (points_.size() < 2) {
        return Error::InvalidState;
    }

    try {
        std::vector<Vec2d> extended_points;

        extended_points.reserve(points_.size() + 2);

        const Vec2d start_point =
            points_[0] + (points_[0] - points_[1]);

        const size_t last_index = points_.size() - 1;

        const Vec2d end_point =
            points_[last_index] +
            (points_[last_index] - points_[last_index - 1]);

        extended_points.push_back(start_point);

        for (const auto& point : points_) {
            extended_points.push_back(point);
        }

        extended_points.push_back(end_point);

        points_.swap(extended_points);
        extended_ = true;
    }
    catch (...) {
        return Error::AllocationFailure;
    }

    return Error::None;
}


auto CatmullRomCurve::setPointCapacity(int32_t capacity) noexcept -> bool {

    if (capacity < 0) {
        return false;
    }

    try {
        points_.reserve(static_cast<size_t>(capacity));
    }
    catch (...) {
        return false;
    }

    return true;
}


auto CatmullRomCurve::pointAtIndex(
    int32_t index,
    Vec2d& out_point) const noexcept -> bool {

    if (index < 0 || index >= pointCount()) {
        return false;
    }

    out_point = points_[static_cast<size_t>(index)];

    return true;
}


//------------------------------------------------------------------------------
//  Parameters
//------------------------------------------------------------------------------

auto CatmullRomCurve::setAlpha(double alpha) noexcept -> bool {

    if (!std::isfinite(alpha) || alpha < 0.0 || alpha > 1.0) {
        return false;
    }

    alpha_ = alpha;

    return true;
}


auto CatmullRomCurve::setPointDistanceThreshold(
    double threshold) noexcept -> bool {

    if (!std::isfinite(threshold) || threshold < 0.0) {
        return false;
    }

    point_distance_threshold_ = threshold;

    return true;
}


//------------------------------------------------------------------------------
//  Geometry
//------------------------------------------------------------------------------

auto CatmullRomCurve::rawLength() const noexcept -> double {

    double length = 0.0;

    for (size_t i = 0; i + 1 < points_.size(); ++i) {
        length += points_[i].distance(points_[i + 1]);
    }

    return length;
}


auto CatmullRomCurve::pointOnCurve(
    double t,
    Vec2d& out_point) const noexcept -> bool {

    const int32_t segment_count = segmentCount();

    if (segment_count <= 0 || !std::isfinite(t)) {
        return false;
    }

    t = std::clamp(t, 0.0, 1.0);

    const double scaled_t = t * segment_count;

    int32_t segment_index = static_cast<int32_t>(scaled_t);

    double local_t = scaled_t - segment_index;

    // The final parameter value belongs to the final segment.
    if (segment_index >= segment_count) {
        segment_index = segment_count - 1;
        local_t = 1.0;
    }

    const size_t index = static_cast<size_t>(segment_index);

    out_point = evaluateSegment(
        points_[index],
        points_[index + 1],
        points_[index + 2],
        points_[index + 3],
        local_t,
        alpha_);

    return true;
}


//------------------------------------------------------------------------------
//  Catmull-Rom implementation
//------------------------------------------------------------------------------

auto CatmullRomCurve::knotInterval(
    const Vec2d& p0,
    const Vec2d& p1,
    double alpha) noexcept -> double {

    const double distance = p0.distance(p1);

    const double interval = std::pow(distance, alpha);

    return std::max(interval, kMinimumKnotInterval);
}


auto CatmullRomCurve::interpolate(
    const Vec2d& p0,
    const Vec2d& p1,
    double t0,
    double t1,
    double t) noexcept -> Vec2d {

    const double denominator = t1 - t0;

    if (std::abs(denominator) < kMinimumKnotInterval) {
        return p0;
    }

    const double weight0 = (t1 - t) / denominator;
    const double weight1 = (t - t0) / denominator;

    return p0 * weight0 + p1 * weight1;
}


auto CatmullRomCurve::evaluateSegment(
    const Vec2d& p0,
    const Vec2d& p1,
    const Vec2d& p2,
    const Vec2d& p3,
    double t,
    double alpha) noexcept -> Vec2d {

    // Calculate the knot parameters.

    const double t0 = 0.0;

    const double t1 = t0 + knotInterval(p0, p1, alpha);
    const double t2 = t1 + knotInterval(p1, p2, alpha);
    const double t3 = t2 + knotInterval(p2, p3, alpha);

    // Map local parameter [0, 1] to [t1, t2].

    t = t1 + t * (t2 - t1);

    // First interpolation level.

    const Vec2d a1 = interpolate(p0, p1, t0, t1, t);
    const Vec2d a2 = interpolate(p1, p2, t1, t2, t);
    const Vec2d a3 = interpolate(p2, p3, t2, t3, t);

    // Second interpolation level.

    const Vec2d b1 = interpolate(a1, a2, t0, t2, t);
    const Vec2d b2 = interpolate(a2, a3, t1, t3, t);

    // Final interpolation.

    return interpolate(b1, b2, t1, t2, t);
}


} // namespace Grain