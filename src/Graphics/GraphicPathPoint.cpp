#include "grain/Graphics/GraphichPathPoint.hpp"
#include "grain/Math/Quadrilateral.hpp"

#include <cmath>
#include <limits>

namespace Grain {

GraphicPathPoint::GraphicPathPoint(
    double x,
    double y
) noexcept
    : anchor(x, y),
      left(x, y),
      right(x, y)
{
}

GraphicPathPoint::GraphicPathPoint(
    double x,
    double y,
    double lx,
    double ly,
    double rx,
    double ry
) noexcept
    : anchor(x, y),
      left(lx, ly),
      right(rx, ry),
      leftFlag(anchor.distance(left) > std::numeric_limits<float>::min()),
      rightFlag(anchor.distance(right) > std::numeric_limits<float>::min())
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
) noexcept
    : anchor(x, y),
      left(lx, ly),
      right(rx, ry),
      leftFlag(leftFlag),
      rightFlag(rightFlag)
{
}

GraphicPathPoint::GraphicPathPoint(
    const Vec2d& anchor,
    bool leftFlag,
    const Vec2d& left,
    bool rightFlag,
    const Vec2d& right
) noexcept
    : anchor(anchor),
      left(left),
      right(right),
      leftFlag(leftFlag),
      rightFlag(rightFlag)
{
}


//------------------------------------------------------------------------------
// Transform
//------------------------------------------------------------------------------

void GraphicPathPoint::translate(
    const Vec2d& offset
) noexcept
{
    anchor.translate(offset);
    left.translate(offset);
    right.translate(offset);
}

void GraphicPathPoint::translate(
    double x,
    double y
) noexcept
{
    anchor.translate(x, y);
    left.translate(x, y);
    right.translate(x, y);
}

void GraphicPathPoint::rotate(
    double degrees
) noexcept
{
    left.rotate(anchor, degrees);
    right.rotate(anchor, degrees);
}


//------------------------------------------------------------------------------
// Projection
//------------------------------------------------------------------------------

void GraphicPathPoint::projectToQuadrilateral(
    const Quadrilateral& quadrilateral,
    const Mat3d* matrix
) noexcept
{
    if (matrix != nullptr) {
        anchor = matrix->transform(anchor);
        left = matrix->transform(left);
        right = matrix->transform(right);
    }

    quadrilateral.project(anchor);
    quadrilateral.project(left);
    quadrilateral.project(right);
}

} // namespace Grain