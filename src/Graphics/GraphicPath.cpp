#include <grain/Graphics/GraphicPath.hpp>

#include <grain/Geometry/Bezier.hpp>
#include <grain/Math/Quadrilateral.hpp>

#include <algorithm>
#include <cmath>
#include <limits>


namespace Grain {


GraphicPath::GraphicPath(int32_t pointCapacity) {
    points_.reserve(
        static_cast<std::size_t>(
            std::max(pointCapacity, 5)
        )
    );
}


//------------------------------------------------------------------------------
// State
//------------------------------------------------------------------------------

auto GraphicPath::hasPoints() const -> bool {
    return !points_.empty();
}


auto GraphicPath::pointCount() const -> int32_t {
    return static_cast<int32_t>(points_.size());
}


auto GraphicPath::segmentCount() const -> int32_t {
    const int32_t count = pointCount();

    if (count < 2) {
        return 0;
    }

    return closed_ ? count : count - 1;
}


//------------------------------------------------------------------------------
// Length
//------------------------------------------------------------------------------

auto GraphicPath::length() -> double {
    update();
    return length_;
}


void GraphicPath::update() {
    if (!must_update_) {
        return;
    }

    updateLength(bezier_segment_resolution_);
    must_update_ = false;
}


//------------------------------------------------------------------------------
// Points
//------------------------------------------------------------------------------

auto GraphicPath::pointPtrAtIndex(int32_t index) -> GraphicPathPoint* {
    if (index < 0 || index >= pointCount()) {
        return nullptr;
    }

    return &points_[static_cast<std::size_t>(index)];
}


auto GraphicPath::pointPtrAtIndex(int32_t index) const -> const GraphicPathPoint* {
    if (index < 0 || index >= pointCount()) {
        return nullptr;
    }

    return &points_[static_cast<std::size_t>(index)];
}


auto GraphicPath::lastPointPtr() -> GraphicPathPoint* {
    if (points_.empty()) {
        return nullptr;
    }

    return &points_.back();
}


auto GraphicPath::lastPointPtr() const -> const GraphicPathPoint* {
    if (points_.empty()) {
        return nullptr;
    }

    return &points_.back();
}


//------------------------------------------------------------------------------
// Geometry
//------------------------------------------------------------------------------

auto GraphicPath::polygonCentroid(Vec2d& outCentroid) const -> double {
    outCentroid.zero();

    const int32_t count = pointCount();

    if (count < 3) {
        return 0.0;
    }

    double twiceArea = 0.0;

    for (int32_t i = 0; i < count; ++i) {
        const int32_t j = (i + 1) % count;

        const Vec2d& p0 = points_[i].anchor;
        const Vec2d& p1 = points_[j].anchor;

        const double cross = p0.cross(p1);

        twiceArea += cross;
        outCentroid += (p0 + p1) * cross;
    }

    if (std::abs(twiceArea) <=
        std::numeric_limits<double>::epsilon())
    {
        outCentroid.zero();
        return 0.0;
    }

    outCentroid *= 1.0 / (3.0 * twiceArea);

    return twiceArea * 0.5;
}


auto GraphicPath::simplePolygonCentroid() const -> Vec2d {
    if (points_.empty()) {
        return {};
    }

    Vec2d centroid;

    for (const auto& point : points_) {
        centroid += point.anchor;
    }

    centroid *= 1.0 /
        static_cast<double>(points_.size());

    return centroid;
}


auto GraphicPath::bounds() const -> Rectd {
    Rectd result;
    bounds(result);
    return result;
}


auto GraphicPath::bounds(Rectd& outBounds ) const -> bool {
    if (!hasPoints()) {
        outBounds.zero();
        return false;
    }

    bool initialized = false;

    for (int32_t i = 0; i < segmentCount(); ++i) {
        Bezier bezier;

        if (!bezierAtIndex(i, bezier)) {
            continue;
        }

        const Rectd segmentBounds = bezier.bounds();

        if (!initialized) {
            outBounds = segmentBounds;
            initialized = true;
        }
        else {
            outBounds += segmentBounds;
        }
    }

    if (!initialized) {
        outBounds.zero();
        return false;
    }

    return true;
}


//------------------------------------------------------------------------------
// Bézier
//------------------------------------------------------------------------------

auto GraphicPath::bezierAtIndex(int32_t segmentIndex, Bezier& outBezier) const -> bool {
    const int32_t lastIndex = lastPointIndex();

    if (segmentIndex < 0) {
        return false;
    }

    if (segmentIndex < lastIndex) {
        return bezierFromTwoPathPoints(
            pointPtrAtIndex(segmentIndex),
            pointPtrAtIndex(segmentIndex + 1),
            outBezier
        );
    }

    if (closed_ && segmentIndex == lastIndex) {
        return bezierFromTwoPathPoints(
            pointPtrAtIndex(lastIndex),
            pointPtrAtIndex(0),
            outBezier
        );
    }

    return false;
}


auto GraphicPath::bezierFromTwoPathPoints(
    const GraphicPathPoint* p1,
    const GraphicPathPoint* p2,
    Bezier& outBezier
) -> bool {
    if ((p1 == nullptr) || (p2 == nullptr)) {
        return false;
    }

    outBezier.set(
        p1->anchor,
        p1->right_flag ? p1->right : p1->anchor,
        p2->left_flag ? p2->left : p2->anchor,
        p2->anchor
    );

    return true;
}


//------------------------------------------------------------------------------
// Length calculation
//------------------------------------------------------------------------------

void GraphicPath::updateLength(int32_t bezierResolution) {
    length_ = 0.0;

    for (auto& point : points_) {
        point.bezier_segment_length = 0.0;
    }

    const int32_t count = segmentCount();

    for (int32_t i = 0; i < count; ++i) {
        Bezier bezier;

        if (!bezierAtIndex(i, bezier)) {
            continue;
        }

        const double segmentLength =
            bezier.approximatedCurveLength(
                bezierResolution
            );

        const int32_t destinationIndex =
            (i + 1) % pointCount();

        points_[destinationIndex].bezier_segment_length =
            segmentLength;

        length_ += segmentLength;
    }
}


//------------------------------------------------------------------------------
// Modification
//------------------------------------------------------------------------------

void GraphicPath::clear() {
    points_.clear();
    length_ = 0.0;
    must_update_ = true;
}


void GraphicPath::addPoint(
    double x, double y,
    bool useLeft, double lx, double ly,
    bool useRight, double rx, double ry
) {
    points_.emplace_back(
        x, y,
        useLeft, lx, ly,
        useRight, rx, ry
    );

    must_update_ = true;
}


void GraphicPath::addPoint(const GraphicPathPoint* point) {
    if (point == nullptr) {
        return;
    }

    points_.push_back(*point);
    must_update_ = true;
}


void GraphicPath::addPoint(const Vec2d& pos) {
    points_.emplace_back(pos.x, pos.y);
    must_update_ = true;
}


void GraphicPath::addPoint(double x, double y){
    points_.emplace_back(x, y);
    must_update_ = true;
}


void GraphicPath::addPoint(const Vec2d& pos, const Vec2d& left, const Vec2d& right) {
    points_.emplace_back(pos, true, left, true, right);
    must_update_ = true;
}


void GraphicPath::addPoint(
    double x, double y,
    double lx, double ly,
    double rx, double ry
) {
    addPoint(x, y, true, lx, ly, true, rx, ry);
}


void GraphicPath::addPointLeft(const Vec2d& pos, const Vec2d& left) {
    points_.emplace_back(pos, true, left, false, Vec2d());
    must_update_ = true;
}


void GraphicPath::addPointLeft(double x, double y, double lx, double ly) {
    addPoint(x, y, true, lx, ly, false, 0.0, 0.0);
}


void GraphicPath::addPointRight(const Vec2d& pos, const Vec2d& right) {
    points_.emplace_back(pos, false, Vec2d(), true, right);
    must_update_ = true;
}


void GraphicPath::addPointRight(double x,double y, double rx, double ry) {
    addPoint(x, y, false, 0.0, 0.0, true, rx, ry);
}


//------------------------------------------------------------------------------
// Point by angle
//------------------------------------------------------------------------------

void GraphicPath::addPointByAngleDegrees(
    const Vec2d& pos,
    double angle,
    double leftLength,
    double rightLength
) {
    Vec2d left(-leftLength, 0.0);
    left.rotateDegrees(angle);

    Vec2d right(rightLength, 0.0);
    right.rotateDegrees(angle);

    addPoint(
        pos.x,
        pos.y,
        pos.x + left.x,
        pos.y + left.y,
        pos.x + right.x,
        pos.y + right.y
    );
}


void GraphicPath::addPointByAngleDegrees(
    double x,
    double y,
    double angle,
    double leftLength,
    double rightLength
) {
    Vec2d left(-leftLength, 0.0);
    left.rotate(angle);

    Vec2d right(rightLength, 0.0);
    right.rotate(angle);

    addPoint(
        x,
        y,
        x + left.x,
        y + left.y,
        x + right.x,
        y + right.y
    );
}


void GraphicPath::addPointByAngleDegrees(
    const Vec2d& pos,
    double leftAngle,
    double leftLength,
    double rightAngle,
    double rightLength
) {
    Vec2d left(-leftLength, 0.0);
    left.rotateDegrees(leftAngle);

    Vec2d right(rightLength, 0.0);
    right.rotateDegrees(rightAngle);

    addPoint(
        pos.x,
        pos.y,
        pos.x + left.x,
        pos.y + left.y,
        pos.x + right.x,
        pos.y + right.y
    );
}


void GraphicPath::addPointByAngleDegrees(
    double x,
    double y,
    double leftAngle,
    double leftLength,
    double rightAngle,
    double rightLength
) {
    Vec2d left(-leftLength, 0.0);
    left.rotate(leftAngle);

    Vec2d right(rightLength, 0.0);
    right.rotate(rightAngle);

    addPoint(
        x,
        y,
        x + left.x,
        y + left.y,
        x + right.x,
        y + right.y
    );
}


//------------------------------------------------------------------------------
// Bézier modification
//------------------------------------------------------------------------------

void GraphicPath::addArcAsBezier(
    const Vec2d& radii,
    double xAxisRotation,
    bool largeArcFlag,
    bool sweepFlag,
    const Vec2d& endPos,
    int32_t maxSegmentCount
) {
    constexpr int32_t maxSegments = 16;

    if (points_.empty()) {
        return;
    }

    maxSegmentCount =
        std::clamp(
            maxSegmentCount,
            1,
            maxSegments
        );

    const Vec2d startPos = points_.back().anchor;

    std::array<Vec2d, maxSegments * 3 + 1> positions{};

    const int32_t segmentCount =
        Bezier::arcToBezierPosArray(
            startPos,
            radii,
            xAxisRotation,
            largeArcFlag,
            sweepFlag,
            endPos,
            maxSegmentCount,
            positions.data()
        );

    if (segmentCount <= 0) {
        return;
    }

    points_.back().right = positions[1];
    points_.back().right_flag = true;

    int32_t positionIndex = 3;

    for (int32_t i = 0; i < segmentCount - 1; ++i) {
        addPoint(
            positions[positionIndex],
            positions[positionIndex - 1],
            positions[positionIndex + 1]
        );

        positionIndex += 3;
    }

    addPointLeft(
        positions[positionIndex],
        positions[positionIndex - 1]
    );

    must_update_ = true;
}


void GraphicPath::addBezier(
    const Vec2d& control1Pos,
    const Vec2d& control2Pos,
    const Vec2d& endPos
) {
    if (points_.empty()) {
        return;
    }

    setLastRight(control1Pos);
    addPointLeft(endPos, control2Pos);
}


void GraphicPath::addQuadraticBezier(const Vec2d& controlPos, const Vec2d& endPos) {
    if (points_.empty()) {
        return;
    }

    Bezier bezier(
        points_.back().anchor,
        controlPos,
        endPos
    );

    addBezier(
        bezier.controlPos1(),
        bezier.controlPos2(),
        bezier.endPos()
    );
}


void GraphicPath::addSmoothBezier(const Vec2d& control2Pos, const Vec2d& endPos) {
    if (points_.empty()) {
        return;
    }

    const auto& point = points_.back();

    const Vec2d control1Pos =
        point.left_flag
            ? point.left.reflectedPoint(point.anchor)
            : point.anchor;

    addBezier(
        control1Pos,
        control2Pos,
        endPos
    );
}


void GraphicPath::addSmoothQuadraticBezier(const Vec2d& endPos) {
    if (points_.empty()) {
        return;
    }

    const int32_t pointIndex = lastPointIndex();

    const auto& point = points_[pointIndex];

    Vec2d controlPos;

    if (point.left_flag && pointIndex > 0) {
        Bezier previousBezier;

        if (bezierAtIndex(pointIndex - 1, previousBezier)) {
            Vec2d previousQuadraticControl;

            previousBezier.approximateQuadraticBezierControlPos(
                previousQuadraticControl
            );

            controlPos =
                previousQuadraticControl
                    .reflectedPoint(point.anchor);
        }
        else {
            controlPos =
                point.anchor +
                (endPos - point.anchor) * 0.5;
        }
    }
    else {
        controlPos =
            point.anchor +
            (endPos - point.anchor) * 0.5;
    }

    addQuadraticBezier(
        controlPos,
        endPos
    );
}


//------------------------------------------------------------------------------
// Control points
//------------------------------------------------------------------------------

void GraphicPath::setLastLeft(const Vec2d& left) {
    if (points_.empty()) {
        return;
    }

    points_.back().left = left;
    points_.back().left_flag = true;

    must_update_ = true;
}


void GraphicPath::setLastRight(const Vec2d& right) {
    if (points_.empty()) {
        return;
    }

    points_.back().right = right;
    points_.back().right_flag = true;

    must_update_ = true;
}


//------------------------------------------------------------------------------
// Transform
//------------------------------------------------------------------------------

void GraphicPath::translatePoint(int32_t index, double tx, double ty) {
    auto* point = pointPtrAtIndex(index);

    if (point == nullptr) {
        return;
    }

    point->translate(tx, ty);
    must_update_ = true;
}


void GraphicPath::rotatePointDegrees(int32_t index, double degrees) {
    auto* point = pointPtrAtIndex(index);

    if (point == nullptr) {
        return;
    }

    point->rotateDegrees(degrees);
    must_update_ = true;
}


//------------------------------------------------------------------------------
// Projection
//------------------------------------------------------------------------------

void GraphicPath::projectToQuadrilateral(
    const Quadrilateral& quadrilateral,
    const Mat3d* matrix
) {
    for (auto& point : points_) {
        point.projectToQuadrilateral(
            quadrilateral,
            matrix
        );
    }

    must_update_ = true;
}


//------------------------------------------------------------------------------
// Split
//------------------------------------------------------------------------------

void GraphicPath::split(
    double start,
    double end,
    GraphicPathSplitParam& out_split_param
) {
    out_split_param = {};

    update();

    start = std::clamp(start, 0.0, 1.0);
    end = std::clamp(end, start, 1.0);

    if ((end - start) <=
        std::numeric_limits<double>::epsilon())
    {
        return;
    }

    if (length_ <= 0.0 || pointCount() < 2) {
        return;
    }

    out_split_param.start = start;
    out_split_param.end = end;

    const double start_distance = start * length_;
    const double endDistance = end * length_;

    double accumulated = 0.0;

    for (int32_t segmentIndex = 0;
         segmentIndex < segmentCount();
         ++segmentIndex)
    {
        Bezier bezier;

        if (!bezierAtIndex(segmentIndex, bezier)) {
            continue;
        }

        const int32_t destination_index = (segmentIndex + 1) % pointCount();
        const double segment_length = points_[destination_index].bezier_segment_length;
        const double segment_start = accumulated;
        const double segment_end = accumulated + segment_length;

        if (out_split_param.start_index < 0 && start_distance <= segment_end) {
            out_split_param.start_index = segmentIndex;

            out_split_param.t0 =
                segment_length > 0.0
                    ? (start_distance - segment_start) /
                      segment_length
                    : 0.0;
        }

        if (out_split_param.end_index < 0 && endDistance <= segment_end) {
            out_split_param.end_index = segmentIndex;

            out_split_param.t1 =
                segment_length > 0.0
                    ? (endDistance - segment_start) /
                      segment_length
                    : 0.0;

            break;
        }

        accumulated = segment_end;
    }

    if (out_split_param.start_index < 0) {
        out_split_param.start_index = 0;
        out_split_param.t0 = 0.0;
    }

    if (out_split_param.end_index < 0) {
        out_split_param.end_index = segmentCount() - 1;
        out_split_param.t1 = 1.0;
    }

    out_split_param.t0 = std::clamp(out_split_param.t0, 0.0, 1.0);
    out_split_param.t1 = std::clamp(out_split_param.t1, 0.0, 1.0);

    out_split_param.valid = true;
}


} // namespace Grain