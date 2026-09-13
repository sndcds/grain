#include "CairoGraphicContext.hpp"

#include <numbers>

namespace Grain::Platform {
namespace {

void setSourceColor(cairo_t* context, const Grain::Color& color) {
    cairo_set_source_rgba(
        context,
        static_cast<double>(color.red),
        static_cast<double>(color.green),
        static_cast<double>(color.blue),
        static_cast<double>(color.alpha)
    );
}

} // namespace

CairoGraphicContext::CairoGraphicContext(cairo_t* context) noexcept
    : context_(context) {
}

void CairoGraphicContext::save() {
    if (context_ == nullptr) {
        return;
    }

    // Cairo has one source, so keep Grain's fill and stroke colors separately.
    stateStack_.push_back(state_);
    cairo_save(context_);
}

void CairoGraphicContext::restore() {
    if (context_ == nullptr || stateStack_.empty()) {
        return;
    }

    cairo_restore(context_);
    state_ = stateStack_.back();
    stateStack_.pop_back();
}

void CairoGraphicContext::setFillColor(const Grain::Color& color) {
    if (context_ == nullptr) {
        return;
    }

    GraphicContext::setFillColor(color);
}

void CairoGraphicContext::setStrokeColor(const Grain::Color& color) {
    if (context_ == nullptr) {
        return;
    }

    GraphicContext::setStrokeColor(color);
}

void CairoGraphicContext::setLineWidth(double width) {
    if (context_ == nullptr) {
        return;
    }

    GraphicContext::setLineWidth(width);
    cairo_set_line_width(context_, width);
}

void CairoGraphicContext::beginPath() noexcept {
    if (context_ == nullptr) {
        return;
    }

    cairo_new_path(context_);
}

void CairoGraphicContext::moveTo(const Grain::Vec2d& point) noexcept {
    if (context_ == nullptr) {
        return;
    }

    cairo_move_to(context_, point.x, point.y);
}

void CairoGraphicContext::lineTo(const Grain::Vec2d& point) noexcept {
    if (context_ == nullptr) {
        return;
    }

    cairo_line_to(context_, point.x, point.y);
}

void CairoGraphicContext::curveTo(
    const Grain::Vec2d& control1,
    const Grain::Vec2d& control2,
    const Grain::Vec2d& point
) noexcept {
    if (context_ == nullptr) {
        return;
    }

    cairo_curve_to(context_, control1.x, control1.y, control2.x, control2.y,
                   point.x, point.y);
}

void CairoGraphicContext::closePath() noexcept {
    if (context_ == nullptr) {
        return;
    }

    cairo_close_path(context_);
}

void CairoGraphicContext::fillRect(const Grain::Rectd& rect) {
    if (context_ == nullptr) {
        return;
    }

    // Rectangle drawing replaces the current path, as in Core Graphics.
    beginPath();
    cairo_rectangle(context_, rect.x, rect.y, rect.width, rect.height);
    setSourceColor(context_, state_.fillColor);
    cairo_fill(context_);
}

void CairoGraphicContext::strokeRect(const Grain::Rectd& rect) {
    if (context_ == nullptr) {
        return;
    }

    beginPath();
    cairo_rectangle(context_, rect.x, rect.y, rect.width, rect.height);
    setSourceColor(context_, state_.strokeColor);
    cairo_stroke(context_);
}

void CairoGraphicContext::fillPath(const Grain::GraphicPath& path) {
    if (context_ == nullptr) {
        return;
    }

    addPath(path);
    setSourceColor(context_, state_.fillColor);
    cairo_fill(context_);
}

void CairoGraphicContext::strokePath(const Grain::GraphicPath& path) {
    if (context_ == nullptr) {
        return;
    }

    addPath(path);
    setSourceColor(context_, state_.strokeColor);
    cairo_stroke(context_);
}

void CairoGraphicContext::translate(double x, double y) {
    if (context_ == nullptr) {
        return;
    }

    cairo_translate(context_, x, y);
}

void CairoGraphicContext::scale(double x, double y) {
    if (context_ == nullptr) {
        return;
    }

    cairo_scale(context_, x, y);
}

void CairoGraphicContext::rotate(double radians) {
    if (context_ == nullptr) {
        return;
    }

    cairo_rotate(context_, radians);
}

void CairoGraphicContext::rotateDegrees(double degrees) {
    rotate(degrees * std::numbers::pi / 180.0);
}

} // namespace Grain::Platform
