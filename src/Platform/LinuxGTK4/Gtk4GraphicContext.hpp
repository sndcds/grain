#pragma once

#include <grain/Graphics/GraphicContext.hpp>

#include <gtk/gtk.h>

namespace Grain::Platform {

class Gtk4GraphicContext final : public GraphicContext {
public:
    // The snapshot is borrowed for the lifetime of this context.
    explicit Gtk4GraphicContext(GtkSnapshot* snapshot) noexcept;
    ~Gtk4GraphicContext() override;

    void save() override;
    void restore() override;

    void beginPath() noexcept override;
    void moveTo(const Vec2d& point) noexcept override;
    void lineTo(const Vec2d& point) noexcept override;
    void curveTo(const Vec2d& control1, const Vec2d& control2, const Vec2d& point) noexcept override;
    void closePath() noexcept override;

    void fillRect(const Rectd& rect) override;
    void strokeRect(const Rectd& rect) override;
    void strokePath() override;

private:
    void rectangle(const Rectd& rect) noexcept;
    void paint(bool stroke);

    GtkSnapshot* snapshot_;
    // Keep points in view coordinates, as Cairo does, so later changes to the
    // transform (including restore) do not move an already constructed path.
    GskPathBuilder* path_;
    bool hasCurrentPoint_ = false;
};

} // namespace Grain::Platform
