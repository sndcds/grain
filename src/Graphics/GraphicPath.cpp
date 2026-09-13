#include <grain/Graphics/GraphicPath.hpp>

#include <grain/Geometry/Bezier.hpp>
#include <grain/Math/Quadrilateral.hpp>

#include <algorithm>
#include <cmath>
#include <limits>


namespace Grain {


GraphicPath::GraphicPath(
    int32_t pointCapacity
) noexcept
{
    points_.reserve(
        static_cast<std::size_t>(
            std::max(pointCapacity, 5)
        )
    );
}


//------------------------------------------------------------------------------
// State
//------------------------------------------------------------------------------

bool GraphicPath::hasPoints() const noexcept {
    return !points_.empty();
}


int32_t GraphicPath::pointCount() const noexcept {
    return static_cast<int32_t>(points_.size());
}


int32_t GraphicPath::segmentCount() const noexcept {
    const int32_t count = pointCount();

    if (count < 2) {
        return 0;
    }

    return closed_ ? count : count - 1;
}


//------------------------------------------------------------------------------
// Length
//------------------------------------------------------------------------------

double GraphicPath::length() noexcept {
    update();
    return length_;
}


void GraphicPath::update() noexcept {
    if (!mustUpdate_) {
        return;
    }

    updateLength(bezierSegmentResolution_);
    mustUpdate_ = false;
}


//------------------------------------------------------------------------------
// Points
//------------------------------------------------------------------------------

GraphicPathPoint* GraphicPath::pointPtrAtIndex(
    int32_t index
) noexcept
{
    if (index < 0 || index >= pointCount()) {
        return nullptr;
    }

    return &points_[static_cast<std::size_t>(index)];
}


const GraphicPathPoint* GraphicPath::pointPtrAtIndex(
    int32_t index
) const noexcept
{
    if (index < 0 || index >= pointCount()) {
        return nullptr;
    }

    return &points_[static_cast<std::size_t>(index)];
}


GraphicPathPoint* GraphicPath::lastPointPtr() noexcept {
    if (points_.empty()) {
        return nullptr;
    }

    return &points_.back();
}


const GraphicPathPoint* GraphicPath::lastPointPtr() const noexcept {
    if (points_.empty()) {
        return nullptr;
    }

    return &points_.back();
}


//------------------------------------------------------------------------------
// Geometry
//------------------------------------------------------------------------------

double GraphicPath::polygonCentroid(
    Vec2d& outCentroid
) const noexcept
{
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


Vec2d GraphicPath::simplePolygonCentroid() const noexcept {
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


Rectd GraphicPath::bounds() noexcept {
    Rectd result;
    bounds(result);
    return result;
}


bool GraphicPath::bounds(
    Rectd& outBounds
) noexcept
{
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

bool GraphicPath::bezierAtIndex(
    int32_t segmentIndex,
    Bezier& outBezier
) noexcept
{
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


bool GraphicPath::bezierFromTwoPathPoints(
    const GraphicPathPoint* p1,
    const GraphicPathPoint* p2,
    Bezier& outBezier
) noexcept
{
    if (!p1 || !p2) {
        return false;
    }

    outBezier.set(
        p1->anchor,
        p1->rightFlag ? p1->right : p1->anchor,
        p2->leftFlag ? p2->left : p2->anchor,
        p2->anchor
    );

    return true;
}


//------------------------------------------------------------------------------
// Length calculation
//------------------------------------------------------------------------------

void GraphicPath::updateLength(
    int32_t bezierResolution
) noexcept
{
    length_ = 0.0;

    for (auto& point : points_) {
        point.bezierSegmentLength = 0.0;
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

        points_[destinationIndex].bezierSegmentLength =
            segmentLength;

        length_ += segmentLength;
    }
}


//------------------------------------------------------------------------------
// Modification
//------------------------------------------------------------------------------

void GraphicPath::clear() noexcept {
    points_.clear();
    length_ = 0.0;
    mustUpdate_ = true;
}


void GraphicPath::addPoint(
    double x,
    double y,
    bool useLeft,
    double lx,
    double ly,
    bool useRight,
    double rx,
    double ry
) noexcept
{
    points_.emplace_back(
        x,
        y,
        useLeft,
        lx,
        ly,
        useRight,
        rx,
        ry
    );

    mustUpdate_ = true;
}


void GraphicPath::addPoint(
    const GraphicPathPoint* point
) noexcept
{
    if (!point) {
        return;
    }

    points_.push_back(*point);
    mustUpdate_ = true;
}


void GraphicPath::addPoint(
    const Vec2d& pos
) noexcept
{
    points_.emplace_back(pos.x, pos.y);
    mustUpdate_ = true;
}


void GraphicPath::addPoint(
    double x,
    double y
) noexcept
{
    points_.emplace_back(x, y);
    mustUpdate_ = true;
}


void GraphicPath::addPoint(
    const Vec2d& pos,
    const Vec2d& left,
    const Vec2d& right
) noexcept
{
    points_.emplace_back(
        pos,
        true,
        left,
        true,
        right
    );

    mustUpdate_ = true;
}


void GraphicPath::addPoint(
    double x,
    double y,
    double lx,
    double ly,
    double rx,
    double ry
) noexcept
{
    addPoint(
        x,
        y,
        true,
        lx,
        ly,
        true,
        rx,
        ry
    );
}


void GraphicPath::addPointLeft(
    const Vec2d& pos,
    const Vec2d& left
) noexcept
{
    points_.emplace_back(
        pos,
        true,
        left,
        false,
        Vec2d()
    );

    mustUpdate_ = true;
}


void GraphicPath::addPointLeft(
    double x,
    double y,
    double lx,
    double ly
) noexcept
{
    addPoint(
        x,
        y,
        true,
        lx,
        ly,
        false,
        0.0,
        0.0
    );
}


void GraphicPath::addPointRight(
    const Vec2d& pos,
    const Vec2d& right
) noexcept
{
    points_.emplace_back(
        pos,
        false,
        Vec2d(),
        true,
        right
    );

    mustUpdate_ = true;
}


void GraphicPath::addPointRight(
    double x,
    double y,
    double rx,
    double ry
) noexcept
{
    addPoint(
        x,
        y,
        false,
        0.0,
        0.0,
        true,
        rx,
        ry
    );
}


//------------------------------------------------------------------------------
// Point by angle
//------------------------------------------------------------------------------

void GraphicPath::addPointByAngleDegrees(
    const Vec2d& pos,
    double angle,
    double leftLength,
    double rightLength
) noexcept
{
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
) noexcept
{
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
) noexcept
{
    Vec2d left(-leftLength, 0.0);
    left.rotate(leftAngle);

    Vec2d right(rightLength, 0.0);
    right.rotate(rightAngle);

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
) noexcept
{
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
) noexcept
{
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

    Vec2d positions[maxSegments * 3 + 1];

    const int32_t segmentCount =
        Bezier::arcToBezierPosArray(
            startPos,
            radii,
            xAxisRotation,
            largeArcFlag,
            sweepFlag,
            endPos,
            maxSegmentCount,
            positions
        );

    if (segmentCount <= 0) {
        return;
    }

    points_.back().right = positions[1];
    points_.back().rightFlag = true;

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

    mustUpdate_ = true;
}


void GraphicPath::addBezier(
    const Vec2d& control1Pos,
    const Vec2d& control2Pos,
    const Vec2d& endPos
) noexcept
{
    if (points_.empty()) {
        return;
    }

    setLastRight(control1Pos);
    addPointLeft(endPos, control2Pos);
}


void GraphicPath::addQuadraticBezier(
    const Vec2d& controlPos,
    const Vec2d& endPos
) noexcept
{
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


void GraphicPath::addSmoothBezier(
    const Vec2d& control2Pos,
    const Vec2d& endPos
) noexcept
{
    if (points_.empty()) {
        return;
    }

    const auto& point = points_.back();

    const Vec2d control1Pos =
        point.leftFlag
            ? point.left.reflectedPoint(point.anchor)
            : point.anchor;

    addBezier(
        control1Pos,
        control2Pos,
        endPos
    );
}


void GraphicPath::addSmoothQuadraticBezier(
    const Vec2d& endPos
) noexcept
{
    if (points_.empty()) {
        return;
    }

    const int32_t pointIndex = lastPointIndex();

    const auto& point = points_[pointIndex];

    Vec2d controlPos;

    if (point.leftFlag && pointIndex > 0) {
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

void GraphicPath::setLastLeft(
    const Vec2d& left
) noexcept
{
    if (points_.empty()) {
        return;
    }

    points_.back().left = left;
    points_.back().leftFlag = true;

    mustUpdate_ = true;
}


void GraphicPath::setLastRight(
    const Vec2d& right
) noexcept
{
    if (points_.empty()) {
        return;
    }

    points_.back().right = right;
    points_.back().rightFlag = true;

    mustUpdate_ = true;
}


//------------------------------------------------------------------------------
// Transform
//------------------------------------------------------------------------------

void GraphicPath::translatePoint(
    int32_t index,
    double tx,
    double ty
) noexcept
{
    auto* point = pointPtrAtIndex(index);

    if (!point) {
        return;
    }

    point->translate(tx, ty);
    mustUpdate_ = true;
}


void GraphicPath::rotatePoint(
    int32_t index,
    double angle
) noexcept
{
    auto* point = pointPtrAtIndex(index);

    if (!point) {
        return;
    }

    point->rotate(angle);
    mustUpdate_ = true;
}


//------------------------------------------------------------------------------
// Projection
//------------------------------------------------------------------------------

void GraphicPath::projectToQuadrilateral(
    const Quadrilateral& quadrilateral,
    const Mat3d* matrix
) noexcept
{
    for (auto& point : points_) {
        point.projectToQuadrilateral(
            quadrilateral,
            matrix
        );
    }

    mustUpdate_ = true;
}


//------------------------------------------------------------------------------
// Split
//------------------------------------------------------------------------------

void GraphicPath::split(
    double start,
    double end,
    GraphicPathSplitParam& outSplitParam
)
{
    outSplitParam = {};

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

    outSplitParam.start = start;
    outSplitParam.end = end;

    const double startDistance = start * length_;
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

        const int32_t destinationIndex =
            (segmentIndex + 1) % pointCount();

        const double segmentLength =
            points_[destinationIndex].bezierSegmentLength;

        const double segmentStart = accumulated;
        const double segmentEnd =
            accumulated + segmentLength;

        if (outSplitParam.startIndex < 0 &&
            startDistance <= segmentEnd)
        {
            outSplitParam.startIndex = segmentIndex;

            outSplitParam.t0 =
                segmentLength > 0.0
                    ? (startDistance - segmentStart) /
                      segmentLength
                    : 0.0;
        }

        if (outSplitParam.endIndex < 0 &&
            endDistance <= segmentEnd)
        {
            outSplitParam.endIndex = segmentIndex;

            outSplitParam.t1 =
                segmentLength > 0.0
                    ? (endDistance - segmentStart) /
                      segmentLength
                    : 0.0;

            break;
        }

        accumulated = segmentEnd;
    }

    if (outSplitParam.startIndex < 0) {
        outSplitParam.startIndex = 0;
        outSplitParam.t0 = 0.0;
    }

    if (outSplitParam.endIndex < 0) {
        outSplitParam.endIndex =
            segmentCount() - 1;

        outSplitParam.t1 = 1.0;
    }

    outSplitParam.t0 =
        std::clamp(outSplitParam.t0, 0.0, 1.0);

    outSplitParam.t1 =
        std::clamp(outSplitParam.t1, 0.0, 1.0);

    outSplitParam.valid = true;
}


} // namespace Grain