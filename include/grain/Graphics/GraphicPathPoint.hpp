#pragma once

#include <grain/Math/Vec2.hpp>
#include <grain/Math/Mat3.hpp>

namespace Grain {

class Quadrilateral;

class GraphicPathPoint {
public:
    GraphicPathPoint() = default;

    GraphicPathPoint(
        double x,
        double y
    ) noexcept;

    GraphicPathPoint(
        double x,
        double y,
        double lx,
        double ly,
        double rx,
        double ry
    ) noexcept;

    GraphicPathPoint(
        double x,
        double y,
        bool leftFlag,
        double lx,
        double ly,
        bool rightFlag,
        double rx,
        double ry
    ) noexcept;

    GraphicPathPoint(
        const Vec2d& anchor,
        bool leftFlag,
        const Vec2d& left,
        bool rightFlag,
        const Vec2d& right
    ) noexcept;

    ~GraphicPathPoint() = default;

    GraphicPathPoint(const GraphicPathPoint&) = default;
    GraphicPathPoint& operator=(const GraphicPathPoint&) = default;

    GraphicPathPoint(GraphicPathPoint&&) noexcept = default;
    GraphicPathPoint& operator=(GraphicPathPoint&&) noexcept = default;


    //--------------------------------------------------------------------------
    // Transform
    //--------------------------------------------------------------------------

    void translate(const Vec2d& offset) noexcept;

    void translate(
        double x,
        double y
    ) noexcept;

    void rotate(double radians) noexcept;
    void rotateDegrees(double degrees) noexcept;


    //--------------------------------------------------------------------------
    // Projection
    //--------------------------------------------------------------------------

    void projectToQuadrilateral(
        const Quadrilateral& quadrilateral,
        const Mat3d* matrix = nullptr
    ) noexcept;


    //--------------------------------------------------------------------------
    // Data
    //--------------------------------------------------------------------------

    Vec2d anchor;
    Vec2d left;
    Vec2d right;

    bool leftFlag = false;
    bool rightFlag = false;

    double bezierSegmentLength = 0.0;
};

} // namespace Grain