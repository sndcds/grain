#pragma once

#include <grain/Graphics/GraphicPathPoint.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Math/Mat3.hpp>
#include <grain/Math/Vec2.hpp>

#include <cstdint>
#include <vector>

namespace Grain {

class Bezier;
class Quadrilateral;


struct GraphicPathSplitParam {
    bool valid = false;

    double start = 0.0;
    double end = 1.0;

    int32_t startIndex = -1;
    int32_t endIndex = -1;

    double t0 = 0.0;
    double t1 = 1.0;
};


class GraphicPath {
public:
    explicit GraphicPath(
        int32_t pointCapacity = 5
    ) noexcept;

    ~GraphicPath() noexcept = default;

    GraphicPath(const GraphicPath&) = default;
    GraphicPath& operator=(const GraphicPath&) = default;

    GraphicPath(GraphicPath&&) noexcept = default;
    GraphicPath& operator=(GraphicPath&&) noexcept = default;


    //--------------------------------------------------------------------------
    // State
    //--------------------------------------------------------------------------

    [[nodiscard]] bool isClosed() const noexcept {
        return closed_;
    }

    void setClosed(bool closed) noexcept {
        closed_ = closed;
        mustUpdate_ = true;
    }

    [[nodiscard]] bool hasPoints() const noexcept;

    [[nodiscard]] int32_t pointCount() const noexcept;

    [[nodiscard]] int32_t lastPointIndex() const noexcept {
        return pointCount() - 1;
    }

    [[nodiscard]] int32_t segmentCount() const noexcept;


    //--------------------------------------------------------------------------
    // Length
    //--------------------------------------------------------------------------

    [[nodiscard]] double length() noexcept;

    void setBezierSegmentResolution(
        int32_t resolution
    ) noexcept {
        bezierSegmentResolution_ = resolution;
        mustUpdate_ = true;
    }

    [[nodiscard]] int32_t bezierSegmentResolution() const noexcept {
        return bezierSegmentResolution_;
    }


    //--------------------------------------------------------------------------
    // Points
    //--------------------------------------------------------------------------

    GraphicPathPoint* pointPtrAtIndex(
        int32_t index
    ) noexcept;

    const GraphicPathPoint* pointPtrAtIndex(
        int32_t index
    ) const noexcept;

    GraphicPathPoint* lastPointPtr() noexcept;

    const GraphicPathPoint* lastPointPtr() const noexcept;


    //--------------------------------------------------------------------------
    // Geometry
    //--------------------------------------------------------------------------

    [[nodiscard]] Rectd bounds() noexcept;

    bool bounds(
        Rectd& outBounds
    ) noexcept;

    double polygonCentroid(
        Vec2d& outCentroid
    ) const noexcept;

    [[nodiscard]] Vec2d simplePolygonCentroid() const noexcept;


    //--------------------------------------------------------------------------
    // Bézier
    //--------------------------------------------------------------------------

    bool bezierAtIndex(
        int32_t segmentIndex,
        Bezier& outBezier
    ) noexcept;

    static bool bezierFromTwoPathPoints(
        const GraphicPathPoint* p1,
        const GraphicPathPoint* p2,
        Bezier& outBezier
    ) noexcept;


    //--------------------------------------------------------------------------
    // Modification
    //--------------------------------------------------------------------------

    void clear() noexcept;

    void addPoint(
        double x,
        double y,
        bool useLeft,
        double lx,
        double ly,
        bool useRight,
        double rx,
        double ry
    ) noexcept;

    void addPoint(
        const GraphicPathPoint* point
    ) noexcept;

    void addPoint(
        const Vec2d& pos
    ) noexcept;

    void addPoint(
        double x,
        double y
    ) noexcept;

    void addPoint(
        const Vec2d& pos,
        const Vec2d& left,
        const Vec2d& right
    ) noexcept;

    void addPoint(
        double x,
        double y,
        double lx,
        double ly,
        double rx,
        double ry
    ) noexcept;

    void addPointLeft(
        const Vec2d& pos,
        const Vec2d& left
    ) noexcept;

    void addPointLeft(
        double x,
        double y,
        double lx,
        double ly
    ) noexcept;

    void addPointRight(
        const Vec2d& pos,
        const Vec2d& right
    ) noexcept;

    void addPointRight(
        double x,
        double y,
        double rx,
        double ry
    ) noexcept;

    void addPointByAngleDegrees(
        const Vec2d& pos,
        double angle,
        double leftLength,
        double rightLength
    ) noexcept;

    void addPointByAngleDegrees(
        double x,
        double y,
        double angle,
        double leftLength,
        double rightLength
    ) noexcept;

    void addPointByAngleDegrees(
        const Vec2d& pos,
        double leftAngle,
        double leftLength,
        double rightAngle,
        double rightLength
    ) noexcept;

    void addPointByAngleDegrees(
        double x,
        double y,
        double leftAngle,
        double leftLength,
        double rightAngle,
        double rightLength
    ) noexcept;


    //--------------------------------------------------------------------------
    // Bézier modification
    //--------------------------------------------------------------------------

    void addArcAsBezier(
        const Vec2d& radii,
        double xAxisRotation,
        bool largeArcFlag,
        bool sweepFlag,
        const Vec2d& endPos,
        int32_t maxSegmentCount = 16
    ) noexcept;

    void addBezier(
        const Vec2d& control1Pos,
        const Vec2d& control2Pos,
        const Vec2d& endPos
    ) noexcept;

    void addQuadraticBezier(
        const Vec2d& controlPos,
        const Vec2d& endPos
    ) noexcept;

    void addSmoothBezier(
        const Vec2d& control2Pos,
        const Vec2d& endPos
    ) noexcept;

    void addSmoothQuadraticBezier(
        const Vec2d& endPos
    ) noexcept;


    //--------------------------------------------------------------------------
    // Control points
    //--------------------------------------------------------------------------

    void setLastLeft(
        const Vec2d& left
    ) noexcept;

    void setLastRight(
        const Vec2d& right
    ) noexcept;


    //--------------------------------------------------------------------------
    // Transform
    //--------------------------------------------------------------------------

    void translatePoint(
        int32_t index,
        double tx,
        double ty
    ) noexcept;

    void rotatePoint(
        int32_t index,
        double angle
    ) noexcept;


    //--------------------------------------------------------------------------
    // Projection
    //--------------------------------------------------------------------------

    void projectToQuadrilateral(
        const Quadrilateral& quadrilateral,
        const Mat3d* matrix = nullptr
    ) noexcept;


    //--------------------------------------------------------------------------
    // Split
    //--------------------------------------------------------------------------

    void split(
        double start,
        double end,
        GraphicPathSplitParam& outSplitParam
    );


private:
    void update() noexcept;

    void updateLength(
        int32_t bezierResolution
    ) noexcept;


private:
    std::vector<GraphicPathPoint> points_;

    bool closed_ = false;
    bool mustUpdate_ = true;

    double length_ = 0.0;

    int32_t bezierSegmentResolution_ = 20;
};

} // namespace Grain