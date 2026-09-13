#include "Gtk4GraphicContext.hpp"

#include <cmath>
#include <numbers>

namespace Grain::Platform {
namespace {

GdkRGBA nativeColor(const Color& color) noexcept {
    return {color.red, color.green, color.blue, color.alpha};
}

struct PathTransform {
    GskPathBuilder* builder;
    Mat3d matrix;
};

gboolean transformPath(GskPathOperation op, const graphene_point_t* points,
                       gsize count, float, gpointer data) noexcept {
    auto& target = *static_cast<PathTransform*>(data);
    Vec2d p[4];
    for (gsize i = 0; i < count; ++i) {
        p[i] = target.matrix.transform({points[i].x, points[i].y});
    }
    switch (op) {
    case GSK_PATH_MOVE:
        gsk_path_builder_move_to(target.builder, static_cast<float>(p[0].x), static_cast<float>(p[0].y));
        break;
    case GSK_PATH_LINE:
        gsk_path_builder_line_to(target.builder, static_cast<float>(p[1].x), static_cast<float>(p[1].y));
        break;
    case GSK_PATH_QUAD:
        gsk_path_builder_quad_to(target.builder, static_cast<float>(p[1].x), static_cast<float>(p[1].y),
                                static_cast<float>(p[2].x), static_cast<float>(p[2].y));
        break;
    case GSK_PATH_CUBIC:
        gsk_path_builder_cubic_to(target.builder, static_cast<float>(p[1].x), static_cast<float>(p[1].y),
                                 static_cast<float>(p[2].x), static_cast<float>(p[2].y),
                                 static_cast<float>(p[3].x), static_cast<float>(p[3].y));
        break;
    case GSK_PATH_CLOSE:
        gsk_path_builder_close(target.builder);
        break;
    case GSK_PATH_CONIC:
        // foreach is asked to emit only lines, quadratics and cubics.
        g_assert_not_reached();
    }
    return TRUE;
}

} // namespace

Gtk4GraphicContext::Gtk4GraphicContext(GtkSnapshot* snapshot) noexcept
    : snapshot_(snapshot), path_(gsk_path_builder_new()) {
}

Gtk4GraphicContext::~Gtk4GraphicContext() {
    gsk_path_builder_unref(path_);
}

void Gtk4GraphicContext::save() {
    stateStack_.push_back(state_);
}

void Gtk4GraphicContext::restore() {
    if (!stateStack_.empty()) {
        state_ = stateStack_.back();
        stateStack_.pop_back();
    }
}

void Gtk4GraphicContext::beginPath() noexcept {
    gsk_path_builder_unref(path_);
    path_ = gsk_path_builder_new();
    hasCurrentPoint_ = false;
}

void Gtk4GraphicContext::moveTo(const Vec2d& point) noexcept {
    const auto p = state_.transform.transform(point);
    gsk_path_builder_move_to(path_, static_cast<float>(p.x), static_cast<float>(p.y));
    hasCurrentPoint_ = true;
}

void Gtk4GraphicContext::lineTo(const Vec2d& point) noexcept {
    if (!hasCurrentPoint_) {
        moveTo(point);
        return;
    }
    const auto p = state_.transform.transform(point);
    gsk_path_builder_line_to(path_, static_cast<float>(p.x), static_cast<float>(p.y));
}

void Gtk4GraphicContext::curveTo(const Vec2d& control1, const Vec2d& control2, const Vec2d& point) noexcept {
    if (!hasCurrentPoint_) {
        moveTo(control1);
    }
    const auto c1 = state_.transform.transform(control1);
    const auto c2 = state_.transform.transform(control2);
    const auto p = state_.transform.transform(point);
    gsk_path_builder_cubic_to(path_, static_cast<float>(c1.x), static_cast<float>(c1.y),
                            static_cast<float>(c2.x), static_cast<float>(c2.y),
                            static_cast<float>(p.x), static_cast<float>(p.y));
}

void Gtk4GraphicContext::closePath() noexcept {
    if (hasCurrentPoint_) {
        gsk_path_builder_close(path_);
    }
}

void Gtk4GraphicContext::rectangle(const Rectd& rect) noexcept {
    beginPath();
    moveTo({rect.x, rect.y});
    lineTo({rect.x + rect.width, rect.y});
    lineTo({rect.x + rect.width, rect.y + rect.height});
    lineTo({rect.x, rect.y + rect.height});
    closePath();
}

void Gtk4GraphicContext::fillRect(const Rectd& rect) {
    rectangle(rect);
    paint(false);
}

void Gtk4GraphicContext::strokeRect(const Rectd& rect) {
    rectangle(rect);
    paint(true);
}

void Gtk4GraphicContext::fillPath(const GraphicPath& path) {
    addPath(path);
    paint(false);
}

void Gtk4GraphicContext::strokePath(const GraphicPath& path) {
    addPath(path);
    paint(true);
}

void Gtk4GraphicContext::paint(bool stroke) {
    g_autoptr(GskPath) path = gsk_path_builder_to_path(path_);
    beginPath();
    if (snapshot_ == nullptr || gsk_path_is_empty(path)) {
        return;
    }
    if (!stroke) {
        const auto color = nativeColor(state_.fillColor);
        gtk_snapshot_append_fill(snapshot_, path, GSK_FILL_RULE_WINDING, &color);
        return;
    }

    // The circular stroke pen depends on M*M^T, not on the rotation or
    // reflection of user coordinates. Factor that matrix as B*B^T with a
    // triangular B (Cholesky), then stroke B^-1(path) under B. This preserves
    // nonuniform scaling and shear without rotating the render node itself.
    const auto& m = state_.transform;
    const double determinant = m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
    if (determinant == 0 || !std::isfinite(determinant) || state_.lineWidth <= 0) {
        return;
    }
    const double sx = std::hypot(m(0, 0), m(0, 1));
    const double sy = std::abs(determinant) / sx;
    const double shear = (m(0, 0) * m(1, 0) + m(0, 1) * m(1, 1)) / sx;
    const Mat3d inverse{
        1 / sx, 0, 0,
        -shear / (sx * sy), 1 / sy, 0,
        0, 0, 1
    };
    g_autoptr(GskPathBuilder) builder = gsk_path_builder_new();
    PathTransform target{builder, inverse};
    gsk_path_foreach(path, static_cast<GskPathForeachFlags>(
        GSK_PATH_FOREACH_ALLOW_QUAD | GSK_PATH_FOREACH_ALLOW_CUBIC), transformPath, &target);
    g_autoptr(GskPath) userPath = gsk_path_builder_to_path(builder);
    g_autoptr(GskStroke) attributes = gsk_stroke_new(static_cast<float>(state_.lineWidth));
    gsk_stroke_set_miter_limit(attributes, static_cast<float>(state_.miterLimit));
    const auto color = nativeColor(state_.strokeColor);
    gtk_snapshot_save(snapshot_);
    g_autoptr(GskTransform) transform = gsk_transform_skew(
        nullptr, 0, static_cast<float>(std::atan(shear / sx) * 180.0 / std::numbers::pi));
    transform = gsk_transform_scale(transform, static_cast<float>(sx), static_cast<float>(sy));
    gtk_snapshot_transform(snapshot_, transform);
    gtk_snapshot_append_stroke(snapshot_, userPath, attributes, &color);
    gtk_snapshot_restore(snapshot_);
}

} // namespace Grain::Platform
