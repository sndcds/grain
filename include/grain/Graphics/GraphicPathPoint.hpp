#pragma once

#include <grain/Math/Vec2.hpp>
#include <grain/Math/Mat3.hpp>

namespace Grain {

class Quadrilateral;

class GraphicPathPoint {
public:
    GraphicPathPoint() = default;
    GraphicPathPoint(double x, double y);
    GraphicPathPoint(double x, double y, double lx, double ly, double rx, double ry);

    GraphicPathPoint(
        double x, double y,
        bool leftFlag, double lx, double ly,
        bool rightFlag, double rx, double ry
    );

    GraphicPathPoint(
        const Vec2d& anchor,
        bool leftFlag, const Vec2d& left,
        bool rightFlag, const Vec2d& right
    );

    ~GraphicPathPoint() = default;

    GraphicPathPoint(const GraphicPathPoint&) = default;
    auto operator = (const GraphicPathPoint&) -> GraphicPathPoint& = default;

    GraphicPathPoint(GraphicPathPoint&&) = default;
    auto operator = (GraphicPathPoint&&) -> GraphicPathPoint& = default;


    //--------------------------------------------------------------------------
    // Transform
    //--------------------------------------------------------------------------

    void translate(const Vec2d& offset);

    void translate(double x, double y);

    void rotate(double radians);
    void rotateDegrees(double degrees);


    //--------------------------------------------------------------------------
    // Projection
    //--------------------------------------------------------------------------

    void projectToQuadrilateral(
        const Quadrilateral& quadrilateral,
        const Mat3d* matrix = nullptr
    );


    //--------------------------------------------------------------------------
    // Data
    //--------------------------------------------------------------------------

    Vec2d anchor;
    Vec2d left;
    Vec2d right;

    bool left_flag = false;
    bool right_flag = false;

    double bezier_segment_length = 0.0;
};

} // namespace Grain