#include "CairoGraphicContext.hpp"

namespace Grain::Platform {

CairoGraphicContext::CairoGraphicContext(
    cairo_t* context
    ) noexcept
    : context_(context) {
}

void CairoGraphicContext::save() {
    cairo_save(context_);
}

void CairoGraphicContext::restore() {
    cairo_restore(context_);
}

void CairoGraphicContext::setFillColor(
    Grain::Color color
    ) {
    cairo_set_source_rgba(
        context_,
        static_cast<double>(color.red),
        static_cast<double>(color.green),
        static_cast<double>(color.blue),
        static_cast<double>(color.alpha)
        );
}

void CairoGraphicContext::setStrokeColor(
    Grain::Color color
    ) {
    cairo_set_source_rgba(
        context_,
        static_cast<double>(color.red),
        static_cast<double>(color.green),
        static_cast<double>(color.blue),
        static_cast<double>(color.alpha)
        );
}

void CairoGraphicContext::setLineWidth(
    double width
    ) {
    cairo_set_line_width(
        context_,
        width
        );
}

void CairoGraphicContext::fillRect(
    const Grain::Rectd& rect
    ) {
    cairo_rectangle(
        context_,
        rect.x,
        rect.y,
        rect.width,
        rect.height
        );

    cairo_fill(context_);
}

void CairoGraphicContext::strokeRect(
    const Grain::Rectd& rect
    ) {
    cairo_rectangle(
        context_,
        rect.x,
        rect.y,
        rect.width,
        rect.height
        );

    cairo_stroke(context_);
}

void CairoGraphicContext::translate(
    double x,
    double y
    ) {
    cairo_translate(
        context_,
        x,
        y
        );
}

void CairoGraphicContext::scale(
    double x,
    double y
    ) {
    cairo_scale(
        context_,
        x,
        y
        );
}

void CairoGraphicContext::rotate(
    double radians
    ) {
    cairo_rotate(
        context_,
        radians
        );
}

} // namespace Grain::Platform