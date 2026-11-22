#include <grain/UI/Component.hpp>
#include <grain/UI/View.hpp>
#include <grain/UI/LayoutEngine.hpp>

#include <iostream>
#include <grain/UI/LayoutStyle.hpp>

namespace Grain {


bool Component::contains(const Vec2d& point) const {
    const auto& rect = bounds();
    return rect.contains(point);
}


bool Component::enabled() const {
    return enabled_;
}


void Component::setEnabled(bool enabled) {
    if (enabled_ != enabled) {
        enabled_ = enabled;
        invalidate();
    }
}


void Component::setHovered(bool hovered) {
    if (hovered_ != hovered) {
        hovered_ = hovered;
        invalidate();
    }
}


void Component::setFocused(bool focused) {
    if (focused_ != focused) {
        focused_ = focused;
        invalidate();
    }
}


void Component::setPressed(bool pressed) {
    if (pressed_ != pressed) {
        pressed_ = pressed;
        invalidate();
    }
}


void Component::setVisible(bool visible) {
    if (visible_ != visible) {
        visible_ = visible;
        invalidate();
    }
}


void Component::setEditable(bool editable) {
    if (editable_ != editable) {
        editable_ = editable;
        invalidate();
    }
}


void Component::setNumberMode(bool number_mode) {
    if (number_mode_ != number_mode) {
        number_mode_ = number_mode;
        invalidate();
    }
}


void Component::invalidate() {
    if (view_ != nullptr) {
        view_->requestRedraw(layout_result_.bounds);
    }
}


void Component::draw(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    // Default component drawing
}


void Component::layoutSizing() {

    layout_result_.intrinsic_size = {0, 0, 0, 0};
}


void Component::layoutPreferredSizing() {

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


void Component::layoutGrow() {

}


auto Component::layoutPositioning(const Vec2d& position)
    -> LayoutResult&
{
    layout_result_.bounds.width =
        layout_result_.preferred_size.width;

    layout_result_.bounds.height =
        layout_result_.preferred_size.height;

    layout_result_.bounds.setPosition(position);

    return layout_result_;
}


}