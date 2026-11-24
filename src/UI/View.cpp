#include <grain/UI/View.hpp>
#include <grain/UI/UIElement.hpp>
#include <grain/UI/UIStyles.hpp>
#include <grain/UI/LayoutStyle.hpp>

#include <grain/Platform/Platform.hpp>
#include <grain/Input/Event.hpp>

#include <memory>
#include <ranges>
#include <iostream>
#include <grain/Math/Random.hpp>

namespace Grain {
class View::Impl {
public:
    std::unique_ptr<Platform::View> platformView;

    Impl();
    ~Impl();
};

View::Impl::Impl() = default;
View::Impl::~Impl() = default;


View::View()
    : impl_(std::make_unique<Impl>())
{
}


View::~View() = default;


void View::attach() {
    if (impl_->platformView != nullptr) {
        return;
    }

    impl_->platformView =
        Platform::createView(this);
}


auto View::platformView() -> Platform::View* {
    if (impl_ == nullptr) {
        return nullptr;
    }

    return impl_->platformView.get();
}


void View::updateHoveredComponent(Component* component) {
    if (hovered_component_ == component) {
        return;
    }

    if (hovered_component_ != nullptr) {
        hovered_component_->setHovered(false);
    }

    hovered_component_ = component;

    if (hovered_component_ != nullptr) {
        hovered_component_->setHovered(true);
    }
}


void View::updateFocusedComponent(Component* component) {
    if (focused_component_ == component) {
        return;
    }

    if (focused_component_ != nullptr) {
        focused_component_->setFocused(false);
    }

    focused_component_ = component;

    if (focused_component_ != nullptr) {
        focused_component_->setFocused(true);
    }
}


auto View::hitTest(const Vec2d& pos) -> Component* {

    for (auto& it : std::ranges::reverse_view(children_)) {

        UIElement* child = it.get();

        switch (child->type()) {

        case UIElementType::Component: {
            auto* component = static_cast<Component*>(child);

            if (!component->visible() || !component->enabled()) {
                continue;
            }

            if (component->contains(pos)) {
                return component;
            }

            break;
        }

        case UIElementType::View: {
            auto* view = static_cast<View*>(child);

            // TODO: Implement!
            /*
            if (!view->visible_ || !view->enabled()) {
                continue;
            }
            */

            if (auto* component = view->hitTest(pos)) {
                return component;
            }

            break;
        }
        }
    }

    return nullptr;
}


void View::drawChildren(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    // updateHoveredComponent(hitTest(mousePos()));

    gc.clip(dirty_rect);

    for (auto& child : children_) {
        if (child->type() == UIElementType::Component) {
            auto* component = static_cast<Component*>(child.get());
            if (dirty_rect.intersects(component->bounds())) {
                component->draw(gc, dirty_rect);
            }
        }
        else if (child->type() == UIElementType::View) {
            auto* view = static_cast<View*>(child.get());
            if (dirty_rect.intersects(view->bounds())) {
                view->draw(gc, dirty_rect);
            }
        }
    }
}


void View::drawContent(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    (void)gc;
    (void)dirty_rect;
}


void View::draw(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    auto rect = dirty_rect;
    if (rect.y < 0) {
        rect.height += rect.y;
        rect.y = 0;
    }

    Random random;

    gc.setFillColor({random.real(), random.real(), random.real(), 1});
    gc.fillRect(layout_result_.bounds);

    drawContent(gc, dirty_rect);
    drawChildren(gc, dirty_rect);
}


void View::layoutSizing() {
    const auto& element = style_->element;

    Sized new_size = {0, 0};

    int32_t i = 0;
    for (const auto& child : children_) {

        const auto& child_size = child->layout_result_.preferred_size;

        if (style_->direction == LayoutDirection::TopToBottom) {
            new_size.width = std::max(new_size.width, child_size.width);
            new_size.height += child_size.height;
        }
        else {
            new_size.width += child_size.width;
            new_size.height = std::max(new_size.height, child_size.height);
        }
    }

    if (children_.size() > 1) {

        const double gaps = (children_.size() - 1) * element.child_gap;

        if (style_->direction == LayoutDirection::TopToBottom) {
            new_size.height += gaps;
        }
        else {
            new_size.width += gaps;
        }
    }

    layout_result_.intrinsic_size = new_size;

    layoutPreferredSizing();
}


void View::layoutPreferredSizing() {

    const auto& element = style_->element;
    const auto& sizing = element.sizing;

    const double width =
        layout_result_.intrinsic_size.width +
        element.padding.left + element.padding.right;

    const double height =
        layout_result_.intrinsic_size.height +
        element.padding.top + element.padding.bottom;

    layout_result_.preferred_size.width = std::clamp(
        width, sizing.width.min_value, sizing.width.max_value
    );

    layout_result_.preferred_size.height = std::clamp(
        height, sizing.height.min_value, sizing.height.max_value
    );
}


void View::layoutGrow() {

    const auto& element = style_->element;
    const bool vertical = style_->direction == LayoutDirection::TopToBottom;

    const double available_space =
        vertical
            ? layout_result_.allocated_size.height
                - element.padding.top
                - element.padding.bottom
            : layout_result_.allocated_size.width
                - element.padding.left
                - element.padding.right;

    double used_space = 0.0;
    double total_grow_weight = 0.0;

    // Distribute space among this View's direct children.

    // Calculate space already occupied by non-growing children
    // and the total Grow weight.
    for (const auto& child : children_) {

        const auto& sizing = child->element().sizing;
        const auto& axis = vertical ? sizing.height : sizing.width;
        const auto& size = child->layout_result_.allocated_size;

        if (axis.type == LayoutSizingType::Grow) {
            total_grow_weight += axis.value;
        }
        else {
            used_space += vertical ? size.height : size.width;
        }
    }

    // Account for gaps between children.
    if (children_.size() > 1) {
        used_space += (children_.size() - 1) * element.child_gap;
    }

    // Calculate the remaining space.
    const double remaining_space = std::max(0.0, available_space - used_space);

    if (total_grow_weight > 0.0) {
        // Distribute remaining space among growing children.
        for (auto& child : children_) {

            const auto& sizing = child->element().sizing;
            const auto& axis = vertical ? sizing.height : sizing.width;

            if (axis.type != LayoutSizingType::Grow) {
                continue;
            }

            const double weight = axis.value;

            if (weight <= 0.0) {
                continue;
            }

            const double allocated = remaining_space * (weight / total_grow_weight);

            if (vertical) {
                child->layout_result_.allocated_size.height =
                    std::clamp(
                        allocated,
                        axis.min_value,
                        axis.max_value
                    );
            }
            else {
                child->layout_result_.allocated_size.width =
                    std::clamp(
                        allocated,
                        axis.min_value,
                        axis.max_value
                    );
            }
        }
    }

    // Allocate the cross axis.
    //
    // In a vertical layout, this is width.
    // In a horizontal layout, this is height.

    const double cross_available_space =
        vertical
            ? layout_result_.allocated_size.width
                - element.padding.left
                - element.padding.right
            : layout_result_.allocated_size.height
                - element.padding.top
                - element.padding.bottom;

    for (auto& child : children_) {

        const auto& sizing = child->element().sizing;

        const auto& axis =
            vertical ? sizing.width : sizing.height;

        if (axis.type != LayoutSizingType::Grow) {
            continue;
        }

        const double allocated = std::clamp(
            cross_available_space,
            axis.min_value,
            axis.max_value
        );

        if (vertical) {
            child->layout_result_.allocated_size.width = allocated;
        }
        else {
            child->layout_result_.allocated_size.height = allocated;
        }
    }

    // Recursively distribute space inside child Views.
    for (auto& child : children_) {
        if (child->type() == UIElementType::View) {
            static_cast<View&>(*child).layoutGrow();
        }
    }
}


auto View::layoutPositioning(const Rectd& bounds)
    -> LayoutResult&
{
    layout_result_.bounds = bounds;

    positionChildren({
        bounds.x,
        bounds.y
    });

    return layout_result_;
}


void View::positionChildren(const Vec2d& position) {

    const auto& element = style_->element;

    const bool vertical =
        style_->direction == LayoutDirection::TopToBottom;

    Vec2d cursor{
        position.x + element.padding.left,
        position.y + element.padding.top
    };

    for (auto& child : children_) {

        const auto& allocated =
            child->layout_result_.allocated_size;

        const Rectd child_bounds{
            cursor.x,
            cursor.y,
            allocated.width,
            allocated.height
        };

        auto& result =
            child->layoutPositioning(child_bounds);

        if (vertical) {
            cursor.y += result.bounds.height + element.child_gap;
        }
        else {
            cursor.x += result.bounds.width + element.child_gap;
        }
    }
}


void View::handleMouseEvent(const Event& event) {

    mouse_pos_ = event.mouse_pos_;

    switch (event.type_) {

        case EventType::MouseMove: {
            if (mouse_capture_component_ != nullptr) {
                mouse_capture_component_->handleEvent(event);
            }
            else {
                updateHoveredComponent(hitTest(event.mouse_pos_));
            }

            break;
        }

        case EventType::MouseButtonDown: {
            Component* component = hitTest(event.mouse_pos_);
            updateHoveredComponent(component);

            if (component != nullptr &&
                component->handleEvent(event))
            {
                mouse_capture_component_ = component;
            }

            break;
        }

        case EventType::MouseButtonUp: {
            Component* component = mouse_capture_component_;
            if (component != nullptr) {
                // Release capture before dispatching the event.
                mouse_capture_component_ = nullptr;
                component->handleEvent(event);
            }
            else {
                component = hitTest(event.mouse_pos_);

                if (component != nullptr) {
                    component->handleEvent(event);
                }
            }

            updateHoveredComponent(
                hitTest(event.mouse_pos_)
            );

            break;
        }

        case EventType::MouseWheel: {
            Component* component = hitTest(event.mouse_pos_);
            if (component != nullptr) {
                component->handleEvent(event);
            }

            break;
        }

        default:
            break;
    }
}


void View::handleKeyEvent(const Event& event) {
    switch (event.type_) {
        case EventType::KeyDown: {
            if (focused_component_ != nullptr) {
            }
            break;
        }

        default:
            break;
    }
}


void View::handleEvent(const Event& event) {
    switch (event.type_) {
        default: {
            if (focused_component_ != nullptr &&
                focused_component_->visible() &&
                focused_component_->enabled()) {
                focused_component_->handleEvent(event);
                }
            break;
        }
    }
}


void View::requestRedraw() {
    if (impl_->platformView == nullptr) {
        return;
    }

    impl_->platformView->requestRedraw();
}


void View::requestRedraw(const Rectd& dirty_rect) {
    if (impl_->platformView == nullptr) {
        return;
    }

    impl_->platformView->requestRedraw(dirty_rect);
}


auto View::queryMousePos() const -> Vec2d {
    if (impl_ == nullptr || impl_->platformView == nullptr) {
        return {};
    }

    return impl_->platformView->mousePos();
}


} // namespace Grain