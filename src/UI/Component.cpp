#include "grain/UI/Component.hpp"

#include "grain/Platform/View.hpp"

namespace Grain {

void Component::setBounds(const Rectd& bounds) {
    if (bounds_ != bounds) {
        auto dirty_rect = bounds.united(this->bounds());
        bounds_ = bounds;

        if (view_ != nullptr) {
            view_->requestRedraw(dirty_rect);
        }
    }
}


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
        view_->requestRedraw(bounds_);
    }
}

}