#pragma once

#include <grain/Math/Vec2.hpp>

#include <cstdint>


namespace Grain {


class Arc {

public:
    enum class SetMode : int8_t {
        Undefined = -1,
        ThreePoints,
        CenterAndAngles,
        Endpoint
    };


public:

    //--------------------------------------------------------------------------
    //  Construction
    //--------------------------------------------------------------------------

    Arc() noexcept = default;

    Arc(
        const Vec2d& start_pos,
        const Vec2d& mid_pos,
        const Vec2d& end_pos
    ) noexcept;


    //--------------------------------------------------------------------------
    //  Information
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto isValid() const noexcept -> bool {
        return valid_;
    }

    [[nodiscard]]
    auto isCircle() const noexcept -> bool;

    [[nodiscard]]
    auto setMode() const noexcept -> SetMode {
        return set_mode_;
    }

    [[nodiscard]]
    auto center() const noexcept -> Vec2d {
        return center_;
    }

    [[nodiscard]]
    auto radiusX() const noexcept -> double {
        return radii_.x;
    }

    [[nodiscard]]
    auto radiusY() const noexcept -> double {
        return radii_.y;
    }

    [[nodiscard]]
    auto radiusScale() const noexcept -> double {
        return radius_scale_;
    }

    [[nodiscard]]
    auto effectiveRadiusX() const noexcept -> double;

    [[nodiscard]]
    auto effectiveRadiusY() const noexcept -> double;

    [[nodiscard]]
    auto radii() const noexcept -> Vec2d {
        return radii_;
    }

    [[nodiscard]]
    auto startPos() const noexcept -> Vec2d {
        return start_pos_;
    }

    [[nodiscard]]
    auto midPos() const noexcept -> Vec2d {
        return mid_pos_;
    }

    [[nodiscard]]
    auto endPos() const noexcept -> Vec2d {
        return end_pos_;
    }

    [[nodiscard]]
    auto startX() const noexcept -> double {
        return start_pos_.x;
    }

    [[nodiscard]]
    auto startY() const noexcept -> double {
        return start_pos_.y;
    }

    [[nodiscard]]
    auto midX() const noexcept -> double {
        return mid_pos_.x;
    }

    [[nodiscard]]
    auto midY() const noexcept -> double {
        return mid_pos_.y;
    }

    [[nodiscard]]
    auto endX() const noexcept -> double {
        return end_pos_.x;
    }

    [[nodiscard]]
    auto endY() const noexcept -> double {
        return end_pos_.y;
    }

    [[nodiscard]]
    auto rotation() const noexcept -> double {
        return rotation_;
    }

    [[nodiscard]]
    auto startAngle() const noexcept -> double {
        return start_angle_;
    }

    [[nodiscard]]
    auto endAngle() const noexcept -> double {
        return end_angle_;
    }

    [[nodiscard]]
    auto isClockwise() const noexcept -> bool {
        return clockwise_flag_;
    }

    [[nodiscard]]
    auto isLargeArc() const noexcept -> bool {
        return large_arc_flag_;
    }


    //--------------------------------------------------------------------------
    //  Position
    //--------------------------------------------------------------------------

    auto posAt(
        double t,
        Vec2d& out_pos
    ) const noexcept -> bool;


    //--------------------------------------------------------------------------
    //  Modification
    //--------------------------------------------------------------------------

    auto setPosAtIndex(
        int32_t index,
        const Vec2d& pos
    ) noexcept -> bool;

    auto setStartPos(const Vec2d& pos) noexcept -> bool;
    auto setStartX(double x) noexcept -> bool;
    auto setStartY(double y) noexcept -> bool;

    auto setMidPos(const Vec2d& pos) noexcept -> bool;
    auto setMidPosX(double x) noexcept -> bool;
    auto setMidPosY(double y) noexcept -> bool;

    auto setEndPos(const Vec2d& pos) noexcept -> bool;
    auto setEndX(double x) noexcept -> bool;
    auto setEndY(double y) noexcept -> bool;

    auto setRadius(double radius) noexcept -> bool;
    auto setRadiusX(double radius) noexcept -> bool;
    auto setRadiusY(double radius) noexcept -> bool;

    auto setRotation(double rotation) noexcept -> bool;

    auto setStartAngle(double angle) noexcept -> bool;
    auto setEndAngle(double angle) noexcept -> bool;

    auto setClockwiseFlag(bool flag) noexcept -> void;
    auto setLargeArcFlag(bool flag) noexcept -> void;


    //--------------------------------------------------------------------------
    //  Definition
    //--------------------------------------------------------------------------

    auto setByThreePoints(
        const Vec2d& start_pos,
        const Vec2d& mid_pos,
        const Vec2d& end_pos
    ) noexcept -> bool;


    auto setByCenterAngles(
        const Vec2d& center,
        double radius,
        double start_angle,
        double end_angle,
        bool clockwise_flag
    ) noexcept -> bool;


    auto setByEndpoint(
        const Vec2d& start_pos,
        const Vec2d& end_pos,
        const Vec2d& radii,
        double rotation,
        bool large_arc_flag,
        bool clockwise_flag
    ) noexcept -> bool;


private:

    //--------------------------------------------------------------------------
    //  Internal calculations
    //--------------------------------------------------------------------------

    auto updateEndpointGeometry() noexcept -> void;

    auto updateValidity() noexcept -> void;


private:

    SetMode set_mode_ = SetMode::Undefined;

    bool valid_ = false;

    Vec2d start_pos_{};
    Vec2d end_pos_{};
    Vec2d mid_pos_{};

    Vec2d radii_{};
    Vec2d center_{};

    double rotation_ = 0.0;

    bool large_arc_flag_ = false;
    bool clockwise_flag_ = false;

    double start_angle_ = 0.0;
    double end_angle_ = 0.0;

    double radius_scale_ = 1.0;

};


} // namespace Grain
