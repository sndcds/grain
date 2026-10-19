#pragma once

#include <iostream>
#include <grain/Graphics/GraphicState.hpp>
#include <grain/Graphics/Color.hpp>
#include <grain/Graphics/Font.hpp>
#include <grain/Graphics/GraphicPath.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Layout.hpp>

#include <vector>
#include <memory>

namespace Grain {

class Gradient;
class TextLayout;

class GraphicContext {
public:
    virtual ~GraphicContext() = default;

    GraphicContext(const GraphicContext&) = delete;

    GraphicContext& operator=(const GraphicContext&) = delete;

    GraphicContext(GraphicContext&&) = delete;

    GraphicContext& operator=(GraphicContext&&) = delete;


    //--------------------------------------------------------------------------
    //  Stack
    //--------------------------------------------------------------------------

    virtual void save() = 0;
    virtual void restore() = 0;


    //--------------------------------------------------------------------------
    //  Clip
    //--------------------------------------------------------------------------

    virtual void clip(const Rectd& clip_rect) {
        (void)clip_rect;
    }

    virtual void clipPath() = 0;

    virtual void clipPathEvenOdd() = 0;


    //--------------------------------------------------------------------------
    //  Blend
    //--------------------------------------------------------------------------

    virtual void setBlendMode(ColorBlendMode blend_mode) {
        state_.blend_mode_ = blend_mode;
    }


    //--------------------------------------------------------------------------
    //  Fill
    //--------------------------------------------------------------------------

    virtual void setFillColor(const Color& color) {
        state_.fill_color_ = color;
    }


    //--------------------------------------------------------------------------
    //  Stroke
    //--------------------------------------------------------------------------

    virtual void setStrokeColor(const Color&  color) {
        state_.stroke_color_ = color;
    }

    virtual void setStrokeWidth(double width) {
        state_.line_width_ = width;
    }

    virtual void setStrokeJoinStyle(StrokeJoinStyle style) {
        state_.line_join_ = style;

    }

    virtual void setStrokeCapStyle(StrokeCapStyle style) {
        state_.line_cap_ = style;
    }

    virtual void setStrokeDash(
        double phase,
        const double* lengths,
        size_t count
    ) = 0;


    //--------------------------------------------------------------------------
    //  Path
    //--------------------------------------------------------------------------

    virtual void beginPath() = 0;
    virtual void moveTo(const Vec2d& point) = 0;
    virtual void lineTo(const Vec2d& point) = 0;
    virtual void curveTo(
        const Vec2d& control1,
        const Vec2d& control2,
        const Vec2d& point
    ) = 0;
    virtual void closePath() = 0;

    virtual void addRectPath(const Rectd& rect) = 0;
    virtual void addEllipsePath(const Rectd& rect) = 0;
    virtual void addCirclePath(const Vec2d& center, double radius) = 0;
    virtual void addRingPath(
        const Vec2d& center,
        double outer_radius,
        double inner_radius
    ) = 0;

    void addPath(const GraphicPath& path) {
        if (path.pointCount() > 1) {
            const GraphicPathPoint* firstPoint = nullptr;
            const GraphicPathPoint* prevPoint = nullptr;

            for (int32_t i = 0; i < path.pointCount(); i++) {
                auto point = path.pointPtrAtIndex(i);

                if (i == 0) {
                    moveTo(point->anchor);
                    firstPoint = point;
                }
                else {
                    if (prevPoint->rightFlag && point->leftFlag) {
                        curveTo(prevPoint->right, point->left, point->anchor);
                    }
                    else if (prevPoint->rightFlag) {
                        curveTo(prevPoint->right, point->anchor, point->anchor);
                    }
                    else if (point->leftFlag) {
                        curveTo(prevPoint->anchor, point->left, point->anchor);
                    }
                    else {
                        lineTo(point->anchor);
                    }
                }

                prevPoint = point;
            }

            if (path.isClosed()) {
                if (firstPoint) {
                    if (prevPoint->rightFlag && firstPoint->leftFlag) {
                        curveTo(prevPoint->right, firstPoint->left, firstPoint->anchor);
                    }
                    else if (prevPoint->rightFlag) {
                        curveTo(prevPoint->right, firstPoint->anchor, firstPoint->anchor);
                    }
                    else if (firstPoint->leftFlag) {
                        curveTo(prevPoint->anchor, firstPoint->left, firstPoint->anchor);
                    }
                    else {
                        lineTo(firstPoint->anchor);
                    }
                }

                closePath();
            }
        }
    }

    void addRoundBarPath(const Rectd& rect) noexcept {
        if (std::fabs(rect.width - rect.height) < std::numeric_limits<float>::epsilon()) {
            // Draw as circle
            addCirclePath(rect.center(), rect.width / 2.0);
        }
        else {
            bool horizontal = rect.width > rect.height;
            double radius = horizontal ? rect.height / 2.0 : rect.width / 2.0;
            double a = radius * 0.55228475;

            double x1 = rect.x;
            double x2 = rect.x + radius - a;
            double x3 = rect.x + radius;
            double x6 = rect.x + rect.width;
            double x5 = x6 - radius + a;
            double x4 = x6 - radius;

            double y1 = rect.y;
            double y2 = rect.y + radius - a;
            double y3 = rect.y + radius;
            double y6 = rect.y + rect.height;
            double y5 = y6 - radius + a;
            double y4 = y6 - radius;

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


    void addRoundRectPath(const Rectd& rect, double radius) {
        if (radius > rect.width / 2) {
            radius = rect.width / 2;
        }
        if (radius > rect.height / 2) {
            radius = rect.height / 2;
        }

        double a = radius * 0.3907;
        double b = radius * 0.5944;
        double c = radius * 0.7854;
        double d = radius * 0.9764;

        double x0 = rect.x;
        double x1 = rect.x + radius;
        double x1a = x1 - a;
        double x1b = x1 - b;
        double x1c = x1 - c;
        double x1d = x1 - d;
        double x2 = rect.x + rect.width - radius;
        double x2a = x2 + a;
        double x2b = x2 + b;
        double x2c = x2 + c;
        double x2d = x2 + d;
        double x3 = rect.x + rect.width;

        double y0 = rect.y;
        double y1 = rect.y + radius;
        double y1a = y1 - a;
        double y1b = y1 - b;
        double y1c = y1 - c;
        double y1d = y1 - d;
        double y2 = rect.y + rect.height - radius;
        double y2a = y2 + a;
        double y2b = y2 + b;
        double y2c = y2 + c;
        double y2d = y2 + d;
        double y3 = rect.y + rect.height;

        moveTo({x2, y0});
        curveTo({x2a, y0}, {x2b, y1d}, {x2c, y1c});
        curveTo({x2 + d, y1 - b}, {x3, y1 - a}, {x3, y1});
        lineTo({x3, y2});
        curveTo({x3, y2a}, {x2d, y2b}, {x2c, y2c});
        curveTo({x2b, y2d}, {x2a, y3}, {x2, y3});
        lineTo({x1, y3});
        curveTo({x1a, y3}, {x1b, y2d}, {x1c, y2c});
        curveTo({x1d, y2b}, {x0, y2a}, {x0, y2});
        lineTo({x0, y1});
        curveTo({x0, y1a}, {x1d, y1b}, {x1c, y1c});
        curveTo({x1b, y1d}, {x1a, y0}, {x1, y0});
        closePath();
    }


    //--------------------------------------------------------------------------
    //  Draw
    //--------------------------------------------------------------------------

    virtual void fillPath() = 0;
    virtual void fillPathEvenOdd() = 0;
    virtual void fillRect(const Rectd& rect) = 0;
    virtual void fillCircle(const Vec2d& center, double radius) = 0;
    virtual void fillEllipse(const Rectd& rect) = 0;

    virtual void strokePath() = 0;
    virtual void strokeRect(const Rectd& rect) = 0;
    virtual void strokeCircle(const Vec2d& center, double radius) = 0;
    virtual void strokeEllipse(const Rectd& rect) = 0;

    virtual void fillGradient(
        Gradient* gradient,
        const Vec2d& start_pos,
        const Vec2d& end_pos,
        bool draw_before,
        bool draw_after
    ) = 0;


    //--------------------------------------------------------------------------
    //  Transform
    //--------------------------------------------------------------------------

    virtual void translate(double x, double y) {
        state_.transform_ *= Mat3<double>::translation(x, y);
    }

    virtual void scale(double x, double y) {
        state_.transform_ *= Mat3<double>::scaling(x, y);
    }

    virtual void rotate(double radians) {
        state_.transform_ *= Mat3<double>::rotation(radians);
    }

    virtual void rotateDegrees(double degrees) {
        state_.transform_ *= Mat3<double>::rotationDegrees(degrees);
    }


    //--------------------------------------------------------------------------
    //  Text
    //--------------------------------------------------------------------------

    virtual std::unique_ptr<TextLayout> createTextLayout(
        const String& text,
        const Font& font
    ) = 0;

    virtual void drawText(
        const String& text,
        const Vec2d& pos,
        const Font* font,
        const Color& color
    ) = 0;

    virtual double drawTextInRect(
        const String& text,
        const Rectd& rect,
        Alignment alignment,
        const Font* font,
        const Color& color
    ) = 0;

    virtual void drawTextLayout(
        const TextLayout& layout,
        const Vec2d& position,
        const Color& color
    ) = 0;

    virtual void drawTextLayoutRange(
        const TextLayout& layout,
        const Vec2d& position,
        int32_t start,
        int32_t end,
        const Color& color
    ) = 0;

protected:
    GraphicContext() = default;

    GraphicState state_;
    std::vector<GraphicState> stateStack_;
};

} // namespace Grain