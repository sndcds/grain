#pragma once

#include <grain/Graphics/GraphicState.hpp>
#include <grain/Color/Color.hpp>
#include <grain/Text/Font.hpp>
#include <grain/Graphics/GraphicPath.hpp>
#include <grain/Graphics/GraphicCompoundPath.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Geometry.hpp>

#include <memory>

namespace Grain {

class Image;
class Gradient;
class TextLayout;

class GraphicContext {
public:
    virtual ~GraphicContext() = default;

    GraphicContext(const GraphicContext&) = delete;

    auto operator = (const GraphicContext&) -> GraphicContext& = delete;

    GraphicContext(GraphicContext&&) = delete;

    auto operator = (GraphicContext&&) -> GraphicContext& = delete;


    //--------------------------------------------------------------------------
    //  Factory
    //--------------------------------------------------------------------------

    static auto createGraphicContext(Image& image) -> std::unique_ptr<GraphicContext>;


    //--------------------------------------------------------------------------
    //  Stack
    //--------------------------------------------------------------------------

    virtual void save() = 0;
    virtual void restore() = 0;


    //--------------------------------------------------------------------------
    //  Clip
    //--------------------------------------------------------------------------

    virtual void clip(const Rectd& clip_rect);
    virtual void clipPath() = 0;
    virtual void clipPathEvenOdd() = 0;


    //--------------------------------------------------------------------------
    //  Blend
    //--------------------------------------------------------------------------

    virtual void setBlendMode(ColorBlendMode blend_mode);


    //--------------------------------------------------------------------------
    //  Rendering configuration
    //--------------------------------------------------------------------------

    virtual void setAllowsAntialiasing(bool flag) = 0;
    virtual void setShouldSmoothFonts(bool flag) = 0;
    virtual void setShouldSubpixelQuantizeFonts(bool flag) = 0;

    virtual void setTextMatrix(
        double a, double b, double c, double d,
        double tx, double ty,
        bool flipped = true
    ) = 0;


    //--------------------------------------------------------------------------
    //  Fill
    //--------------------------------------------------------------------------

    virtual void setFillColor(const Color& color);


    //--------------------------------------------------------------------------
    //  Stroke
    //--------------------------------------------------------------------------

    virtual void setStrokeColor(const Color&  color);
    virtual void setStrokeWidth(double width);
    virtual void setStrokeJoinStyle(StrokeJoinStyle style);
    virtual void setStrokeCapStyle(StrokeCapStyle style);


    virtual void setStrokeDash(
        double phase, const double* lengths, size_t count
    ) = 0;

    virtual void setStrokeMiterLimit(double limit) = 0;

    virtual void setStrokeSolid() = 0;


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

    virtual void addRectPath(const Rectd& rect);
    virtual void addFramePath(const Rectd& outer, const Rectd& inner);

    virtual void addEllipsePath(const Rectd& rect) = 0;
    virtual void addCirclePath(const Vec2d& center, double radius) = 0;
    virtual void addRingPath(
        const Vec2d& center,
        double outer_radius,
        double inner_radius
    ) = 0;


    void addPath(const GraphicPath& path);
    void addPath(const GraphicCompoundPath& path);

    void addRoundBarPath(const Rectd& rect);
    void addRoundRectPath(const Rectd& rect, double radius);


    //--------------------------------------------------------------------------
    //  Draw
    //--------------------------------------------------------------------------

    virtual void fillPath() = 0;
    virtual void fillPathEvenOdd() = 0;
    virtual void fillRect(const Rectd& rect) = 0;
    virtual void fillCircle(const Vec2d& center, double radius) = 0;
    virtual void fillEllipse(const Rectd& rect) = 0;

    virtual void strokePath() = 0;
    virtual void fillStrokedPath(
        float line_width,
        StrokeCapStyle cap_style,
        StrokeJoinStyle join_style,
        float miter_limit) = 0;
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

    virtual void fillRadialGradient(
        Gradient* gradient,
        const Vec2d& start_center,
        double start_radius,
        const Vec2d& end_center,
        double end_radius,
        bool draw_before,
        bool draw_after
    ) = 0;


    //--------------------------------------------------------------------------
    //  Text
    //--------------------------------------------------------------------------

    virtual auto createTextLayout(
        const String& text,
        const Font& font
    ) -> std::unique_ptr<TextLayout> = 0;

    virtual void drawText(
        const String& text,
        const Vec2d& pos,
        const Font* font,
        const Color& color
    ) = 0;

    virtual auto drawTextInRect(
        const String& text,
        const Rectd& rect,
        Alignment alignment,
        const Font* font
    ) -> double = 0;

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


    //--------------------------------------------------------------------------
    //  Image
    //--------------------------------------------------------------------------

    virtual void drawImage(
        const Image& image,
        const Rectd& rect,
        float alpha = 1.0f
    ) = 0;


    //--------------------------------------------------------------------------
    //  Transform
    //--------------------------------------------------------------------------

    virtual void translate(double x, double y);
    virtual void scale(double x, double y);
    virtual void rotate(double radians);
    virtual void rotateDegrees(double degrees);


protected:
    GraphicContext() = default;

    GraphicState state_;
    std::vector<GraphicState> state_stack_;
};

} // namespace Grain