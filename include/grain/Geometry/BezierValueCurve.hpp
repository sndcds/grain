#pragma once

#include <grain/Math/Vec2.hpp>
#include <grain/DSP/WeightedSamples.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Bezier.hpp>
#include <grain/Geometry/Layout.hpp>
#include <grain/Containers/List.hpp>


namespace Grain {

class BezierValueCurve;

class BezierValueCurvePoint {
    friend class Bezier;
    friend class BezierValueCurve;

public:
    enum Status : int8_t {
        None = 0x0,
        Selected = 0x1 << 0,
        Undeletable = 0x1 << 1,
        FixedX = 0x1 << 2,
        FixedY = 0x1 << 3,
        DecayBegin = 0x1 << 4,

        Fixed = FixedX | FixedY,
        FixedEnd = FixedX | Undeletable
    };

    enum class Part : int8_t {
        None,
        Point,
        Left,
        Right
    };

    enum class PointType : int8_t {
        Linear,
        Corner,
        Left,
        Right,
        Smooth1,
        Smooth2
    };

protected:
    BezierValueCurve* bezier_value_curve_ = nullptr;

    Vec2d pos_;               ///< Coordinate
    Vec2d left_;              ///< Left control point relative to `pos_`
    Vec2d right_;             ///< Right control point relative to `pos_`
    Vec2d left_pos_;          ///< Left control point absolute coordinate
    Vec2d right_pos_;         ///< Right control point absolute coordinate
    Vec2d used_left_pos_;     ///< Limited left control point, absolute coordinate
    Vec2d used_right_pos_;    ///< Limited right control point, absolute coordinate

    PointType point_type_ = PointType::Linear;
    Status status_ = Status::None;

    Vec2d remembered_pos_;
    Status remembered_status_ = Status::None;

public:
    BezierValueCurvePoint() = default;

    BezierValueCurvePoint(const BezierValueCurvePoint&) = default;

    [[nodiscard]] auto pos() const -> Vec2d { return pos_; }
    [[nodiscard]] auto left() const -> Vec2d { return left_; }
    [[nodiscard]] auto right() const -> Vec2d { return right_; }
    [[nodiscard]] auto leftPos() const -> Vec2d { return left_pos_; }
    [[nodiscard]] auto rightPos() const -> Vec2d { return right_pos_; }
    [[nodiscard]] auto usedLeftPos() const -> Vec2d { return used_left_pos_; }
    [[nodiscard]] auto usedRightPos() const -> Vec2d { return used_right_pos_; }
    [[nodiscard]] auto pointType() const -> PointType { return point_type_; }
    [[nodiscard]] auto status() const -> Status { return status_; }
    [[nodiscard]] auto leftDistance() const -> double;
    [[nodiscard]] auto rightDistance() const -> double;

    [[nodiscard]] auto isStatus(Status mask, Status status) const -> bool {
        return (status_ & mask) == status;
    }
    [[nodiscard]] auto isSelected() const -> bool {
        return (status_ & Status::Selected) != 0;
    }
    [[nodiscard]] auto isDeleteable() const -> bool {
        return (status_ & Status::Undeletable) == 0;
    }
    [[nodiscard]] auto isXFixed() const -> bool {
        return (status_ & Status::FixedX) != 0;
    }
    [[nodiscard]] auto isYFixed() const -> bool {
        return (status_ & Status::FixedY) != 0;
    }
    [[nodiscard]] auto isDecayBegin() const -> bool {
        return (status_ & Status::DecayBegin) != 0;
    }

    [[nodiscard]] auto hasVisibleControlPoints() const -> bool;
    [[nodiscard]] auto hasLeftControl() const -> bool;
    [[nodiscard]] auto hasRightControl() const -> bool;
    [[nodiscard]] auto usesLeftControl() const -> bool;
    [[nodiscard]] auto usesRightControl() const -> bool;

    [[nodiscard]] auto isSmooth() const -> bool;

    auto setPointType(PointType point_type) -> bool;
    auto changeStatus(Status status, bool flag) -> bool;
    auto select() -> bool { return changeStatus(Status::Selected, true); }
    auto deselect() -> bool { return changeStatus(Status::Selected, false); }
    void invertSelection();
    auto setUndeletable(bool flag) -> bool { return changeStatus(Status::Undeletable, flag); }
    auto setFixed(bool h_flag, bool v_flag) -> bool;
    auto setHorizontalFixed(bool flag) -> bool { return changeStatus(Status::FixedX, flag); }
    auto setVerticalFixed(bool flag) -> bool { return changeStatus(Status::FixedY, flag); }

    void setPos(const Vec2d& pos);
    void setX(double x);
    void setY(double y);

    void clampX(double min, double max);
    void clampY(double min, double max);

    void curveMustUpdate() const;

    void remember();
    void backToRememberedStatus();


    [[nodiscard]] auto compare(const BezierValueCurvePoint* point) const -> int32_t;
};


class BezierValueCurve {
public:
    enum Error : int8_t {
        kErr_ToFewPoints,
        kErr_StartIndexOutOfRange,
        kErr_EndIndexOutOfRange,
        kErrInvalidWidth
    };

    enum class Mode : int8_t {
        Standard = 0,
        Envelope,
        Filter
    };

protected:
    List<BezierValueCurvePoint> points_;
    WeightedSamples* weighted_samples_ = nullptr;

    int32_t default_resolution_ = 1000;

    Mode mode_ = Mode::Standard;
    double limit_min_x_ = 0.0;
    double limit_max_x_ = 1000.0;
    double limit_min_y_ = -1000.0;
    double limit_max_y_ = 1000.0;

    int32_t fractional_digits_ = 6;

    bool must_sort_ = false;
    bool must_update_ = true;
    int32_t modification_count_ = 0;

    int32_t weighted_samples_modification_count_ = 0;
    int32_t weighted_samples_resolution_ = -1;
    int32_t weighted_samples_start_point_index_ = -1;
    int32_t weighted_samples_end_point_index_ = -1;
    int32_t read_decay_point_index_ = -1;

public:
    BezierValueCurve();
    BezierValueCurve(const BezierValueCurve& bezier_value_curve);
    ~BezierValueCurve();


    [[nodiscard]] auto isValid() const -> bool;

    [[nodiscard]] auto defaultResolution() const -> int32_t { return default_resolution_; }
    void setDefaultResolution(int32_t resolution) { default_resolution_ = resolution < 2 ? 2 : resolution; }

    [[nodiscard]] auto length() const -> int32_t { return static_cast<int32_t>(points_.size()); }
    [[nodiscard]] auto lastPointIndex() const -> int32_t { return static_cast<int32_t>(points_.size()) - 1; }
    [[nodiscard]] auto hasPoints() const -> bool { return length() > 0; }

    [[nodiscard]] auto selectedPointsCount() const -> int32_t;
    [[nodiscard]] auto modificationCount() const -> int32_t { return modification_count_; }

    [[nodiscard]] auto decayPointIndex() const -> int32_t;
    auto mutDecayPoint() -> BezierValueCurvePoint* { return mutPointAtIndex(decayPointIndex()); };

    void rangeY(double& out_min_y, double& out_max_y) const;
    [[nodiscard]] auto bbox(bool selected_only) const -> Rectd;

    [[nodiscard]] auto points() const -> const List<BezierValueCurvePoint>& {
        return points_;
    }

    [[nodiscard]] auto pointCount() const -> int32_t { return static_cast<int32_t>(points_.size()); }
    [[nodiscard]] auto mutPointAtIndex(int32_t index) -> BezierValueCurvePoint*;
    [[nodiscard]] auto pointAtIndex(int32_t index) const -> const BezierValueCurvePoint*;
    [[nodiscard]] auto mutFirstPoint() -> BezierValueCurvePoint* { return mutPointAtIndex(0); }
    [[nodiscard]] auto firstPoint() const -> const BezierValueCurvePoint* { return pointAtIndex(0); }
    [[nodiscard]] auto mutLastPoint() -> BezierValueCurvePoint* { return mutPointAtIndex(pointCount() - 1); }
    [[nodiscard]] auto lastPoint() const -> const BezierValueCurvePoint* { return pointAtIndex(pointCount() - 1); }

    [[nodiscard]] auto firstSelectedPoint() -> BezierValueCurvePoint*;
    [[nodiscard]] auto decayBeginPoint() -> BezierValueCurvePoint*;


    [[nodiscard]] auto mode() const -> Mode { return mode_; }
    void setMode(Mode mode) { mode_ = mode; }


    void setLimits(double min_x, double max_x, double min_y, double max_y);
    [[nodiscard]] auto decayBeginIndex() const -> int32_t;
    void setDecayBeginIndex(int32_t decay_begin_index);


    auto setTypeOfSelectedPoints(BezierValueCurvePoint::PointType point_type) -> bool;
    auto horizontalCenterSelectedPoints() -> bool;
    auto verticalCenterSelectedPoints() -> bool;
    auto alignSelectedPoints(Alignment align) -> bool;

    void selectAllPoints();
    void deselectAllPoints();
    void selectPointsInRect(const Rectd& rect);

    [[nodiscard]] auto addPoint() -> BezierValueCurvePoint*;
    auto addLinearPoint(double x, double y) -> BezierValueCurvePoint*;
    auto addPoint(double x, double y, double lx, double ly, double rx, double ry, BezierValueCurvePoint::PointType point_type, BezierValueCurvePoint::Status status = BezierValueCurvePoint::Status::None) -> BezierValueCurvePoint*;
    auto removePoint(int32_t index) -> bool;
    [[nodiscard]] auto removeSelectedPoints() -> int32_t;
    void removeAllPoints() { clear(); }
    void clear();
    void flipVertical();

    auto split(int32_t segment_index, float t, bool select = false) -> bool;

    void rememberAllPoints();
    auto moveRememberedSelectedPoints(const Vec2d& delta) -> bool;

    void mustSort();
    void mustUpdate();

    [[nodiscard]] auto lookup(float t) -> float { return lookup(t, default_resolution_); }
    [[nodiscard]] auto lookup(float t, int32_t resolution) -> float;

    void _sortPoints();
    static auto sortPointsCompareFunc(const BezierValueCurvePoint& a, const BezierValueCurvePoint& b) -> bool;
    auto _update() -> bool;

    auto _updateWeightedSamples(int32_t resolution) -> bool;
    auto _updateWeightedSamples(int32_t start_point_index, int32_t end_point_index, int32_t resolution) -> bool;
};

} // End of namespace Grain
