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

    [[nodiscard]] View* view() const {
        return view_;
    }

    void setView(View* view) {
        view_ = view;
    }

    virtual void setBounds(const Rectd& bounds);

    [[nodiscard]] const Rectd& bounds() const {
        return bounds_;
    }

    virtual bool contains(const Vec2d& pos) const;


    [[nodiscard]] bool enabled() const;

    virtual void setEnabled(bool enabled);

    [[nodiscard]] bool hovered() const { return hovered_; }

    void setHovered(bool hovered);

    [[nodiscard]] bool focused() const { return focused_; }

    void setFocused(bool hovered);

    [[nodiscard]] bool pressed() const { return pressed_; }

    void setPressed(bool pressed);

    [[nodiscard]] bool visible() const { return visible_; }

    void setVisible(bool visible);

    [[nodiscard]] bool editable() const;

    virtual void setEditable(bool editable);

    [[nodiscard]] bool numberMode() const;

    virtual void setNumberMode(bool number_mode);


    virtual void draw(GraphicContext& gc) = 0;

    virtual bool handleEvent(const Event& event) {
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
    bool editable_ = true;
    bool number_mode_ = false;
};

}