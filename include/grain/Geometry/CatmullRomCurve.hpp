#pragma once

#include <grain/Math/Vec2.hpp>

#include <cstdint>
#include <vector>


namespace Grain {


class CatmullRomCurve {

public:
    enum class Error : int8_t {
        None = 0,
        InvalidArgument,
        InvalidState,
        AllocationFailure
    };

public:
    CatmullRomCurve() noexcept = default;
    explicit CatmullRomCurve(int32_t point_capacity) noexcept;

    CatmullRomCurve(const CatmullRomCurve&) = default;
    auto operator = (const CatmullRomCurve&) -> CatmullRomCurve& = default;

    CatmullRomCurve(CatmullRomCurve&&) noexcept = default;
    auto operator = (CatmullRomCurve&&) noexcept -> CatmullRomCurve& = default;

    ~CatmullRomCurve() = default;


    //--------------------------------------------------------------------------
    //  Points
    //--------------------------------------------------------------------------

    auto addPoint(const Vec2d& point) noexcept -> Error;

    auto extend() noexcept -> Error;

    auto reset() noexcept -> void {
        points_.clear();
        extended_ = false;
    }

    auto setPointCapacity(int32_t capacity) noexcept -> bool;

    [[nodiscard]]
    auto pointAtIndex(int32_t index, Vec2d& out_point) const noexcept -> bool;

    [[nodiscard]]
    auto pointCount() const noexcept -> int32_t {
        return static_cast<int32_t>(points_.size());
    }

    [[nodiscard]]
    auto lastPointIndex() const noexcept -> int32_t {
        return pointCount() - 1;
    }

    [[nodiscard]]
    auto segmentCount() const noexcept -> int32_t {
        return points_.size() >= 4
            ? static_cast<int32_t>(points_.size() - 3)
            : 0;
    }

    [[nodiscard]]
    auto lastSegmentIndex() const noexcept -> int32_t {
        return segmentCount() - 1;
    }

    [[nodiscard]]
    auto isExtended() const noexcept -> bool {
        return extended_;
    }


    //--------------------------------------------------------------------------
    //  Parameters
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto alpha() const noexcept -> double {
        return alpha_;
    }

    auto setAlpha(double alpha) noexcept -> bool;

    [[nodiscard]]
    auto pointDistanceThreshold() const noexcept -> double {
        return point_distance_threshold_;
    }

    auto setPointDistanceThreshold(double threshold) noexcept -> bool;


    //--------------------------------------------------------------------------
    //  Geometry
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto rawLength() const noexcept -> double;

    auto pointOnCurve(double t, Vec2d& out_point) const noexcept -> bool;


private:
    [[nodiscard]]
    static auto knotInterval(
        const Vec2d& p0,
        const Vec2d& p1,
        double alpha) noexcept -> double;

    [[nodiscard]]
    static auto interpolate(
        const Vec2d& p0,
        const Vec2d& p1,
        double t0,
        double t1,
        double t) noexcept -> Vec2d;

    [[nodiscard]]
    static auto evaluateSegment(
        const Vec2d& p0,
        const Vec2d& p1,
        const Vec2d& p2,
        const Vec2d& p3,
        double t,
        double alpha) noexcept -> Vec2d;


private:
    std::vector<Vec2d> points_;

    double alpha_ = 0.5;
    double point_distance_threshold_ = 0.01;

    bool extended_ = false;
};


} // namespace Grain
