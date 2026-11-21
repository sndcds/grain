#pragma once

#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Input/Event.hpp>


namespace Grain {

class View;

enum class UIElementType {
    View,
    Component
};

class UIElement {
public:
    UIElement() = default;
    UIElement(const UIElement&) = delete;
    virtual ~UIElement() = default;

    auto operator = (const UIElement&) -> UIElement& = delete;

    [[nodiscard]]
    virtual auto type() const -> UIElementType = 0;

    [[nodiscard]]
    auto parent() const -> UIElement* {
        return parent_;
    }

    [[nodiscard]]
    auto parentView() const -> View*;

    virtual void draw(
        GraphicContext& gc,
        const Rectd& dirty_rect
    ) = 0;

    virtual void doSizing() = 0;


protected:
    UIElement* parent_ = nullptr;

    friend class View;
};

}