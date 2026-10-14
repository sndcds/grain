#pragma once

#include "grain/Platform/View.hpp"

#include <grain/Geometry/Rect.hpp>
#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Input/Event.hpp>

namespace Grain {

class View;

class Component {
public:
    explicit Component(View* view = nullptr)
        : view_(view) {
    }

    virtual ~Component() = default;

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    Component(Component&&) = delete;
    Component& operator=(Component&&) = delete;

    [[nodiscard]] View* view() const noexcept {
        return view_;
    }

    void setView(View* view) noexcept {
        view_ = view;
    }

    [[nodiscard]] const Rectd& bounds() const noexcept {
        return bounds_;
    }

    void setBounds(const Rectd& bounds) noexcept {
        if (bounds_ != bounds) {
            bounds_ = bounds;
            invalidate();
        }
    }

    virtual bool contains(const Vec2d& pos) const noexcept;


    [[nodiscard]] bool enabled() const noexcept;
    void setEnabled(bool enabled) noexcept;

    [[nodiscard]] bool hovered() const noexcept;
    void setHovered(bool hovered) noexcept;

    [[nodiscard]] bool focused() const noexcept;
    void setFocused(bool hovered) noexcept;

    [[nodiscard]] bool pressed() const noexcept;
    void setPressed(bool pressed) noexcept;

    [[nodiscard]] bool visible() const noexcept;
    void setVisible(bool visible) noexcept;

    virtual void draw(GraphicContext& gc) = 0;

    virtual bool handleEvent(const Event& event) {
        (void)event;
        return false;
    }

protected:
    void invalidate() noexcept;

    Rectd bounds_{};
    View* view_ = nullptr;

    bool enabled_ = true;
    bool hovered_ = false;
    bool focused_ = false;
    bool pressed_ = false;
    bool visible_ = true;
};

}