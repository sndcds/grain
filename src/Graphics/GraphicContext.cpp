#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Graphics/GraphicPath.hpp>
#include <grain/Graphics/GraphicPathPoint.hpp>

#include <algorithm>
#include <cmath>
#include <limits>


namespace Grain {


//------------------------------------------------------------------------------
//  Clip
//------------------------------------------------------------------------------

void GraphicContext::clip(const Rectd& clipRect) {

    (void)clipRect;
}


//------------------------------------------------------------------------------
//  Blend
//------------------------------------------------------------------------------

void GraphicContext::setBlendMode(ColorBlendMode blendMode) {

    state_.blend_mode_ = blendMode;
}


//------------------------------------------------------------------------------
//  Fill
//------------------------------------------------------------------------------

void GraphicContext::setFillColor(const Color& color) {

    state_.fill_color_ = color;
}


//--------------------------------------------------------------------------
//  Stroke
//--------------------------------------------------------------------------

void GraphicContext::setStrokeColor(const Color& color) {

    state_.stroke_color_ = color;
}


void GraphicContext::setStrokeWidth(double width) {

    state_.line_width_ = width;
}


void GraphicContext::setStrokeJoinStyle(StrokeJoinStyle style) {

    state_.line_join_ = style;
}


void GraphicContext::setStrokeCapStyle(StrokeCapStyle style) {

    state_.line_cap_ = style;
}


//------------------------------------------------------------------------------
//  Path
//------------------------------------------------------------------------------

void GraphicContext::addRectPath(const Rectd& rect) {

    moveTo({rect.x, rect.bottom()});
    lineTo({rect.right(), rect.bottom()});
    lineTo({rect.right(), rect.y});
    lineTo({rect.x, rect.y});
    closePath();
}


void GraphicContext::addFramePath(
    const Rectd& outer,
    const Rectd& inner
) {

    // Outer contour (clockwise).
    moveTo({outer.x, outer.bottom()});
    lineTo({outer.right(), outer.bottom()});
    lineTo({outer.right(), outer.y});
    lineTo({outer.x, outer.y});
    closePath();

    // Inner contour (counter-clockwise).
    moveTo({inner.x, inner.y});
    lineTo({inner.right(), inner.y});
    lineTo({inner.right(), inner.bottom()});
    lineTo({inner.x, inner.bottom()});
    closePath();
}


void GraphicContext::addPath(const GraphicPath& path) {

    if (path.pointCount() <= 1) {
        return;
    }

    const GraphicPathPoint* firstPoint = nullptr;
    const GraphicPathPoint* previousPoint = nullptr;

    for (int32_t i = 0; i < path.pointCount(); ++i) {

        const auto* point = path.pointPtrAtIndex(i);

        if (point == nullptr) {
            continue;
        }

        if (i == 0) {

            moveTo(point->anchor);
            firstPoint = point;
        }
        else {

            if (previousPoint->right_flag && point->left_flag) {

                curveTo(
                    previousPoint->right,
                    point->left,
                    point->anchor
                );
            }
            else if (previousPoint->right_flag) {

                curveTo(
                    previousPoint->right,
                    point->anchor,
                    point->anchor
                );
            }
            else if (point->left_flag) {

                curveTo(
                    previousPoint->anchor,
                    point->left,
                    point->anchor
                );
            }
            else {

                lineTo(point->anchor);
            }
        }

        previousPoint = point;
    }

    if (path.isClosed()) {

        if (firstPoint != nullptr && previousPoint != nullptr) {

            if (previousPoint->right_flag && firstPoint->left_flag) {

                curveTo(
                    previousPoint->right,
                    firstPoint->left,
                    firstPoint->anchor
                );
            }
            else if (previousPoint->right_flag) {

                curveTo(
                    previousPoint->right,
                    firstPoint->anchor,
                    firstPoint->anchor
                );
            }
            else if (firstPoint->left_flag) {

                curveTo(
                    previousPoint->anchor,
                    firstPoint->left,
                    firstPoint->anchor
                );
            }
            else {

                lineTo(firstPoint->anchor);
            }
        }

        closePath();
    }
}


//------------------------------------------------------------------------------
//  Add compound path
//------------------------------------------------------------------------------

void GraphicContext::addPath(const GraphicCompoundPath& path) {

    const int32_t count = path.pathCount();
    if (count == 0) {
        return;
    }

    for (int32_t i = 0; i < count; ++i) {
        const GraphicPath* subPath = path.pathPtrAtIndex(i);
        if (subPath == nullptr) {
            continue;
        }

        addPath(*subPath);
    }
}


void GraphicContext::addRoundBarPath(const Rectd& rect) {

    if (std::abs(rect.width - rect.height) <
        std::numeric_limits<double>::epsilon()) {

        // Draw as circle.
        addCirclePath(rect.center(), rect.width / 2.0);
    }
    else {

        const bool horizontal = rect.width > rect.height;

        const double radius = horizontal
            ? rect.height / 2.0
            : rect.width / 2.0;

        const double a = radius * 0.55228475;

        const double x1 = rect.x;
        const double x2 = rect.x + radius - a;
        const double x3 = rect.x + radius;
        const double x6 = rect.x + rect.width;
        const double x5 = x6 - radius + a;
        const double x4 = x6 - radius;

        const double y1 = rect.y;
        const double y2 = rect.y + radius - a;
        const double y3 = rect.y + radius;
        const double y6 = rect.y + rect.height;
        const double y5 = y6 - radius + a;
        const double y4 = y6 - radius;

        moveTo({x4, y1});
        curveTo({x5, y1}, {x6, y2}, {x6, y3});

        if (!horizontal) {
            lineTo({x6, y4});
        }

        curveTo({x6, y5}, {x5, y6}, {x4, y6});

        if (horizontal) {
            lineTo({x3, y6});
        }

        curveTo({x2, y6}, {x1, y5}, {x1, y4});

        if (!horizontal) {
            lineTo({x1, y3});
        }

        curveTo({x1, y2}, {x2, y1}, {x3, y1});

        if (horizontal) {
            lineTo({x4, y1});
        }

        closePath();
    }
}


void GraphicContext::addRoundRectPath(
    const Rectd& rect,
    double radius
) {

    radius = std::min(radius, rect.width / 2.0);
    radius = std::min(radius, rect.height / 2.0);

    const double a = radius * 0.3907;
    const double b = radius * 0.5944;
    const double c = radius * 0.7854;
    const double d = radius * 0.9764;

    const double x0 = rect.x;
    const double x1 = rect.x + radius;
    const double x1a = x1 - a;
    const double x1b = x1 - b;
    const double x1c = x1 - c;
    const double x1d = x1 - d;

    const double x2 = rect.x + rect.width - radius;
    const double x2a = x2 + a;
    const double x2b = x2 + b;
    const double x2c = x2 + c;
    const double x2d = x2 + d;

    const double x3 = rect.x + rect.width;

    const double y0 = rect.y;
    const double y1 = rect.y + radius;
    const double y1a = y1 - a;
    const double y1b = y1 - b;
    const double y1c = y1 - c;
    const double y1d = y1 - d;

    const double y2 = rect.y + rect.height - radius;
    const double y2a = y2 + a;
    const double y2b = y2 + b;
    const double y2c = y2 + c;
    const double y2d = y2 + d;

    const double y3 = rect.y + rect.height;

    moveTo({x2, y0});

    curveTo(
        {x2a, y0},
        {x2b, y1d},
        {x2c, y1c}
    );

    curveTo(
        {x2 + d, y1 - b},
        {x3, y1 - a},
        {x3, y1}
    );

    lineTo({x3, y2});

    curveTo(
        {x3, y2a},
        {x2d, y2b},
        {x2c, y2c}
    );

    curveTo(
        {x2b, y2d},
        {x2a, y3},
        {x2, y3}
    );

    lineTo({x1, y3});

    curveTo(
        {x1a, y3},
        {x1b, y2d},
        {x1c, y2c}
    );

    curveTo(
        {x1d, y2b},
        {x0, y2a},
        {x0, y2}
    );

    lineTo({x0, y1});

    curveTo(
        {x0, y1a},
        {x1d, y1b},
        {x1c, y1c}
    );

    curveTo(
        {x1b, y1d},
        {x1a, y0},
        {x1, y0}
    );

    closePath();
}


//------------------------------------------------------------------------------
//  Transform
//------------------------------------------------------------------------------

void GraphicContext::translate(double x, double y) {

    state_.transform_ *= Mat3<double>::translation(x, y);
}


void GraphicContext::scale(double x, double y) {

    state_.transform_ *= Mat3<double>::scaling(x, y);
}


void GraphicContext::rotate(double radians) {

    state_.transform_ *= Mat3<double>::rotation(radians);
}


void GraphicContext::rotateDegrees(double degrees) {

    state_.transform_ *= Mat3<double>::rotationDegrees(degrees);
}


} // namespace Grain
