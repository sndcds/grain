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

    void setEnabled(bool enabled);

    [[nodiscard]] bool hovered() const;

    void setHovered(bool hovered);

    [[nodiscard]] bool focused() const;

    void setFocused(bool hovered);

    [[nodiscard]] bool pressed() const;

    void setPressed(bool pressed);

    [[nodiscard]] bool visible() const;

    void setVisible(bool visible);

    virtual void draw(GraphicContext& gc) = 0;

    virtual bool handleEvent(const Event& event) {
        (void)event;
        return false;
    }

protected:
    void invalidate();

    Rectd bounds_{};
    View* view_ = nullptr;

    bool enabled_ = true;
    bool hovered_ = false;
    bool focused_ = false;
    bool pressed_ = false;
    bool visible_ = true;
};

}