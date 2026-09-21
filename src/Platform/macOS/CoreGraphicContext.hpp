#pragma once

#include <CoreGraphics/CoreGraphics.h>

#include <grain/Graphics/GraphicContext.hpp>

namespace Grain {

class CoreGraphicContext final : public GraphicContext {
public:
    explicit CoreGraphicContext(CGContextRef context);

    ~CoreGraphicContext() override = default;

    void save() override;

    void restore() override;

    void setFillColor(const Color& color) override;

    void setStrokeColor(const Color& color) override;

    void setLineWidth(double width) override;

    void beginPath() noexcept override;
    void moveTo(const Vec2d& point) noexcept override;
    void lineTo(const Vec2d& point) noexcept override;
    void curveTo(const Vec2d& control1, const Vec2d& control2, const Vec2d& point) noexcept override;
    void closePath() noexcept  override;

    void fillRect(const Rectd& rect) override;

    void strokeRect(const Rectd& rect) override;

    void fillPath(const GraphicPath& path) override;

    void strokePath(const GraphicPath& path) override;

    void drawText(const String& text, const Vec2d& pos, const Font* font, const Color& color) override;

    void translate(double x, double y) override;

    void scale(double x, double y) override;

    void rotate(double radians) override;

    void rotateDegrees(double degrees) override;


    //--------------------------------------------------------------------------
    //  Helper
    //--------------------------------------------------------------------------

    void setTextMatrix(double a, double b, double c, double d, double tx, double ty);

    static CGColorRef createCGColor(const Grain::Color& color);

private:
    CGContextRef context_ = nullptr;
    Vec2d lastPoint_;
};

} // namespace Grain