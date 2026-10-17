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


bool Component::hovered() const {
    return hovered_;
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

bool Component::focused() const {
    return focused_;
}


bool Component::pressed() const {
    return pressed_;
}


void Component::setPressed(bool pressed) {
    if (pressed_ != pressed) {
        pressed_ = pressed;
        invalidate();
    }
}


bool Component::visible() const {
    return visible_;
}

void Component::setVisible(bool visible) {
    if (visible_ != visible) {
        visible_ = visible;
        invalidate();
    }
}


void Component::invalidate() {
    if (view_ != nullptr) {
        view_->requestRedraw(bounds_);
    }
}

}