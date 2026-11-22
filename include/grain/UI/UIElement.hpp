#pragma once

#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Input/Event.hpp>
#include <grain/UI/LayoutResult.hpp>

#include "LayoutStyle.hpp"


namespace Grain {

class View;
class LayoutEngine;
struct LayoutElementStyle;


enum class UIElementType : uint8_t {
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

    virtual void layoutSizing() = 0;
    virtual void layoutPreferredSizing() = 0;
    virtual void layoutGrow() = 0;

    virtual auto layoutPositioning(const Rectd& bounds) -> LayoutResult& = 0;

    virtual auto element() -> LayoutElementStyle = 0;

protected:
    UIElement* parent_ = nullptr;
    LayoutResult layout_result_{};

    friend class View;
    friend class LayoutEngine;
};

}
