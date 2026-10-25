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

    int32_t start_index = -1;
    int32_t end_index = -1;

    double t0 = 0.0;
    double t1 = 1.0;
};


class GraphicPath {
public:
    explicit GraphicPath(int32_t pointCapacity = 5);

    ~GraphicPath() = default;

    GraphicPath(const GraphicPath&) = default;
    auto operator = (const GraphicPath&) -> GraphicPath& = default;

    GraphicPath(GraphicPath&&) = default;
    auto operator = (GraphicPath&&) -> GraphicPath& = default;


    //--------------------------------------------------------------------------
    // State
    //--------------------------------------------------------------------------

    [[nodiscard]] auto isClosed() const -> bool {
        return closed_;
    }

    void setClosed(bool closed) {
        closed_ = closed;
        must_update_ = true;
    }

    [[nodiscard]] auto hasPoints() const -> bool;

    [[nodiscard]] auto pointCount() const -> int32_t;

    [[nodiscard]] auto lastPointIndex() const -> int32_t {
        return pointCount() - 1;
    }

    [[nodiscard]] auto segmentCount() const -> int32_t;


    //--------------------------------------------------------------------------
    // Length
    //--------------------------------------------------------------------------

    [[nodiscard]] auto length() -> double;

    void setBezierSegmentResolution(int32_t resolution) {
        bezier_segment_resolution_ = resolution;
        must_update_ = true;
    }

    [[nodiscard]] auto bezierSegmentResolution() const -> int32_t {
        return bezier_segment_resolution_;
    }


    //--------------------------------------------------------------------------
    // Points
    //--------------------------------------------------------------------------

    auto pointPtrAtIndex(int32_t index) -> GraphicPathPoint*;

    [[nodiscard]] auto pointPtrAtIndex(int32_t index) const -> const GraphicPathPoint*;

    auto lastPointPtr() -> GraphicPathPoint*;

    [[nodiscard]] auto lastPointPtr() const -> const GraphicPathPoint*;


    //--------------------------------------------------------------------------
    // Geometry
    //--------------------------------------------------------------------------

    [[nodiscard]] auto bounds() const -> Rectd;

    auto bounds(Rectd& outBounds) const -> bool;

    auto polygonCentroid(Vec2d& outCentroid) const  -> double;

    [[nodiscard]] auto simplePolygonCentroid() const -> Vec2d;


    //--------------------------------------------------------------------------
    // Bézier
    //--------------------------------------------------------------------------

    auto bezierAtIndex(int32_t segmentIndex, Bezier& outBezier) const -> bool;

    static auto bezierFromTwoPathPoints(
        const GraphicPathPoint* p1,
        const GraphicPathPoint* p2,
        Bezier& outBezier
    ) -> bool;


    //--------------------------------------------------------------------------
    // Modification
    //--------------------------------------------------------------------------

    void clear();

    void addPoint(
        double x,
        double y,
        bool useLeft,
        double lx,
        double ly,
        bool useRight,
        double rx,
        double ry
    );

    void addPoint(const GraphicPathPoint* point);

    void addPoint(const Vec2d& pos);

    void addPoint(double x, double y);

    void addPoint(const Vec2d& pos, const Vec2d& left, const Vec2d& right);

    void addPoint(
        double x, double y,
        double lx, double ly,
        double rx, double ry
    );

    void addPointLeft(const Vec2d& pos, const Vec2d& left);

    void addPointLeft(double x, double y, double lx, double ly );

    void addPointRight(const Vec2d& pos, const Vec2d& right);

    void addPointRight(double x, double y, double rx, double ry);

    void addPointByAngleDegrees(
        const Vec2d& pos,
        double angle,
        double leftLength,
        double rightLength
    );

    void addPointByAngleDegrees(
        double x,
        double y,
        double angle,
        double leftLength,
        double rightLength
    );

    void addPointByAngleDegrees(
        const Vec2d& pos,
        double leftAngle,
        double leftLength,
        double rightAngle,
        double rightLength
    );

    void addPointByAngleDegrees(
        double x,
        double y,
        double leftAngle,
        double leftLength,
        double rightAngle,
        double rightLength
    );


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
    );

    void addBezier(
        const Vec2d& control1Pos,
        const Vec2d& control2Pos,
        const Vec2d& endPos
    );

    void addQuadraticBezier(const Vec2d& controlPos, const Vec2d& endPos);

    void addSmoothBezier(const Vec2d& control2Pos, const Vec2d& endPos);

    void addSmoothQuadraticBezier(const Vec2d& endPos);


    //--------------------------------------------------------------------------
    // Control points
    //--------------------------------------------------------------------------

    void setLastLeft(const Vec2d& left);
    void setLastRight(const Vec2d& right);


    //--------------------------------------------------------------------------
    // Transform
    //--------------------------------------------------------------------------

    void translatePoint(int32_t index, double tx, double ty);

    void rotatePointDegrees(int32_t index, double degrees);


    //--------------------------------------------------------------------------
    // Projection
    //--------------------------------------------------------------------------

    void projectToQuadrilateral(
        const Quadrilateral& quadrilateral,
        const Mat3d* matrix = nullptr
    );


    //--------------------------------------------------------------------------
    // Split
    //--------------------------------------------------------------------------

    void split(
        double start,
        double end,
        GraphicPathSplitParam& out_split_param
    );


private:
    void update();

    void updateLength(int32_t bezierResolution);


    std::vector<GraphicPathPoint> points_;

    bool closed_ = false;
    bool must_update_ = true;

    double length_ = 0.0;

    int32_t bezier_segment_resolution_ = 20;
};

} // namespace Grain