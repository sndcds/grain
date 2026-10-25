#include "grain/Graphics/GraphicPathPoint.hpp"
#include "grain/Math/Quadrilateral.hpp"

#include <cmath>
#include <limits>

namespace Grain {

GraphicPathPoint::GraphicPathPoint(
    double x,
    double y
)
    : anchor(x, y),
      left(x, y),
      right(x, y)
{
}

constexpr double epsilon = 1e-12;

GraphicPathPoint::GraphicPathPoint(
    double x,
    double y,
    double lx,
    double ly,
    double rx,
    double ry
)
    : anchor(x, y),
      left(lx, ly),
      right(rx, ry),
      left_flag(anchor.distance(left) > epsilon),
      right_flag(anchor.distance(right) > epsilon)
{
}

GraphicPathPoint::GraphicPathPoint(
    double x,
    double y,
    bool leftFlag,
    double lx,
    double ly,
    bool rightFlag,
    double rx,
    double ry
)
    : anchor(x, y),
      left(lx, ly),
      right(rx, ry),
      left_flag(leftFlag),
      right_flag(rightFlag)
{
}

GraphicPathPoint::GraphicPathPoint(
    const Vec2d& anchor,
    bool leftFlag,
    const Vec2d& left,
    bool rightFlag,
    const Vec2d& right
)
    : anchor(anchor),
      left(left),
      right(right),
      left_flag(leftFlag),
      right_flag(rightFlag)
{
}


//------------------------------------------------------------------------------
// Transform
//------------------------------------------------------------------------------

void GraphicPathPoint::translate(const Vec2d& offset) {
    anchor.translate(offset);
    left.translate(offset);
    right.translate(offset);
}

void GraphicPathPoint::translate(double x, double y) {
    anchor.translate(x, y);
    left.translate(x, y);
    right.translate(x, y);
}

void GraphicPathPoint::rotate(double radians) {
    left.rotate(anchor, radians);
    right.rotate(anchor, radians);
}


void GraphicPathPoint::rotateDegrees(double degrees) {
    left.rotateDegrees(anchor, degrees);
    right.rotateDegrees(anchor, degrees);
}


//------------------------------------------------------------------------------
// Projection
//------------------------------------------------------------------------------

void GraphicPathPoint::projectToQuadrilateral(
    const Quadrilateral& quadrilateral,
    const Mat3d* matrix
) {
    anchor = quadrilateral.project(anchor);
    left = quadrilateral.project(left);
    right = quadrilateral.project(right);

    if (matrix != nullptr) {
        anchor = matrix->transform(anchor);
        left = matrix->transform(left);
        right = matrix->transform(right);
    }
}

} // namespace Grain