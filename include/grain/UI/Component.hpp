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

    auto operator = (const Component&) -> Component& = delete;

    Component(Component&&) = delete;

    auto operator = (Component&&) -> Component& = delete;

    [[nodiscard]] auto view() const -> View* {
        return view_;
    }

    void setView(View* view) {
        view_ = view;
    }

    virtual void setBounds(const Rectd& bounds);

    [[nodiscard]] auto bounds() const -> const Rectd& {
        return bounds_;
    }

    [[nodiscard]] virtual auto contains(const Vec2d& pos) const -> bool;


    [[nodiscard]] auto enabled() const -> bool;
    virtual void setEnabled(bool enabled);

    [[nodiscard]] auto hovered() const -> bool { return hovered_; }
    void setHovered(bool hovered);

    [[nodiscard]] auto focused() const -> bool { return focused_; }
    void setFocused(bool hovered);

    [[nodiscard]] auto pressed() const -> bool { return pressed_; }
    void setPressed(bool pressed);

    [[nodiscard]] auto visible() const -> bool { return visible_; }
    void setVisible(bool visible);

    [[nodiscard]] auto editable() const -> bool { return editable_; }
    virtual void setEditable(bool editable);

     [[nodiscard]] auto numberMode() const -> bool { return number_mode_; }
    virtual void setNumberMode(bool number_mode);


    virtual void draw(GraphicContext& gc) = 0;

    virtual auto handleEvent(const Event& event) -> bool {
        (void)event;
        return false;
    }

protected:
    void invalidate();

    Rectd bounds_{};
    View* view_ = nullptr;

    Component* receiver_component_ = nullptr;

    bool can_get_focus_ = false;
    bool enabled_ = true;
    bool hovered_ = false;
    bool focused_ = false;
    bool pressed_ = false;
    bool visible_ = true;
    bool editable_ = false;
    bool number_mode_ = false;
};

}