#pragma once

#include <grain/Graphics/GraphicContext.hpp>

#include <cairo/cairo.h>

namespace Grain::Platform {

class CairoGraphicContext final : public Grain::GraphicContext {
public:
    // The caller retains ownership of the Cairo context.
    explicit CairoGraphicContext(cairo_t* context) noexcept;

    void save() override;

    void restore() override;

    void setFillColor(const Grain::Color& color) override;

    void setStrokeColor(const Grain::Color& color) override;

    void setLineWidth(double width) override;

    void beginPath() noexcept override;

    void moveTo(const Grain::Vec2d& point) noexcept override;

    void lineTo(const Grain::Vec2d& point) noexcept override;

    void curveTo(const Grain::Vec2d& control1, const Grain::Vec2d& control2,
                 const Grain::Vec2d& point) noexcept override;

    void closePath() noexcept override;

    void fillRect(const Grain::Rectd& rect) override;

    void strokeRect(const Grain::Rectd& rect) override;

    void fillPath(const Grain::GraphicPath& path) override;

    void strokePath(const Grain::GraphicPath& path) override;

    void translate(double x, double y) override;

    void scale(double x, double y) override;

    void rotate(double radians) override;

    void rotateDegrees(double degrees) override;

private:
    cairo_t* context_ = nullptr;
};

} // namespace Grain::Platform
