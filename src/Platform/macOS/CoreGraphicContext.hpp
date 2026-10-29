#pragma once

#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>

#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Geometry/Layout.hpp>

#include "CFObject.hpp"


namespace Grain {

class Image;
class Gradient;
class GraphicContext;


class CoreGraphicContext final : public GraphicContext {
public:
    explicit CoreGraphicContext(CGContextRef context);

    ~CoreGraphicContext() override;

    void save() override;
    void restore() override;


    //-------------------------------------------------------------------------
    // Fill
    //--------------------------------------------------------------------------

    void clip(const Rectd& clip_rect) override;
    void clipPath() override;
    void clipPathEvenOdd() override;


    //--------------------------------------------------------------------------
    // Blend
    //--------------------------------------------------------------------------

    void setBlendMode(ColorBlendMode mode) override;


    //--------------------------------------------------------------------------
    // Fill
    //--------------------------------------------------------------------------

    void setFillColor(const Color& color) override;


    //--------------------------------------------------------------------------
    // Stroke
    //--------------------------------------------------------------------------

    void setStrokeColor(const Color& color) override;
    void setStrokeWidth(double width) override;
    void setStrokeJoinStyle(StrokeJoinStyle style) override;
    void setStrokeCapStyle(StrokeCapStyle style) override;
    void setStrokeDash(double phase, const double* lengths, size_t count) override;


    //--------------------------------------------------------------------------
    // Path
    //--------------------------------------------------------------------------

    void beginPath() override;
    void moveTo(const Vec2d& point) override;
    void lineTo(const Vec2d& point) override;
    void curveTo(
        const Vec2d& control1,
        const Vec2d& control2,
        const Vec2d& point
    ) override;
    void closePath() override;

    void addRectPath(const Rectd& rect) override;
    void addEllipsePath(const Rectd& rect) override;
    void addCirclePath(const Vec2d& center, double radius) override;
    void addRingPath(
        const Vec2d& center,
        double outer_radius,
        double inner_radius
    ) override;


    void fillPath() override;
    void fillPathEvenOdd() override;

    void fillRect(const Rectd& rect) override;
    void fillCircle(const Vec2d& center, double radius) override;
    void fillEllipse(const Rectd& rect) override;

    void strokePath() override;
    void strokeRect(const Rectd& rect) override;
    void strokeCircle(const Vec2d& center, double radius) override;
    void strokeEllipse(const Rectd& rect) override;

    void fillGradient(
        Gradient* gradient,
        const Vec2d& start_pos,
        const Vec2d& end_pos,
        bool draw_before,
        bool draw_after) override;


    //--------------------------------------------------------------------------
    //  Transform.
    //--------------------------------------------------------------------------

    void translate(double x, double y) override;
    void scale(double x, double y) override;
    void rotate(double radians) override;
    void rotateDegrees(double degrees) override;


    //--------------------------------------------------------------------------
    //  Helper
    //--------------------------------------------------------------------------

    void setContext(CGContextRef context) noexcept {
        context_ = context;
    }

    [[nodiscard]] auto context() const noexcept -> CGContextRef {
        return context_;
    }

    void setTextMatrix(double a, double b, double c, double d, double tx, double ty);

    CGColorRef createCGColor(const Color& color) const;

    void createGraphicContext(Image& image);


    //--------------------------------------------------------------------------
    //  Text
    //--------------------------------------------------------------------------

    [[nodiscard]] CFObject<CTLineRef> createTextLine(
        const String& text,
        const Font* font,
        const Color& color
    );

    void drawText(
        const String& text,
        const Vec2d& pos,
        const Font* font,
        const Color& color
    ) override;

    double drawTextInRect(
        const String& text,
        const Rectd& rect,
        Alignment alignment,
        const Font* font,
        const Color& color
    ) override;

    [[nodiscard]] std::unique_ptr<TextLayout> createTextLayout(
        const String& text,
        const Font& font
    ) override;

    void drawTextLayout(
        const TextLayout& layout,
        const Vec2d& position,
        const Color& color
    ) override;

    void drawTextLayoutRange(
        const TextLayout& layout,
        const Vec2d& position,
        int32_t start,
        int32_t end,
        const Color& color
    ) override;

private:
    CGContextRef context_ = nullptr;

    CGColorSpaceRef color_space_ = nullptr;
    Vec2d last_point_;
};

} // namespace Grain