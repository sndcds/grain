#pragma once

#include <grain/Graphics/GraphicContext.hpp>

#include <cairo/cairo.h>

namespace Grain::Platform {

class CairoGraphicContext final : public Grain::GraphicContext {
public:
    explicit CairoGraphicContext(cairo_t* context) noexcept;

    void save() override;

    void restore() override;

    void setFillColor(Grain::Color color) override;

    void setStrokeColor(Grain::Color color) override;

    void setLineWidth(double width) override;

    void fillRect(const Grain::Rectd& rect) override;

    void strokeRect(const Grain::Rectd& rect) override;

    void translate(double x, double y) override;

    void scale(double x, double y) override;

    void rotate(double radians) override;

private:
    cairo_t* context_ = nullptr;
};

}