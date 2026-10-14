#include "grain/UI/Component.hpp"

#include "grain/Platform/View.hpp"

namespace Grain {
bool Component::contains(const Vec2d& point) const noexcept {
    const auto& rect = bounds();
    return rect.contains(point);
}


bool Component::enabled() const noexcept {
    return enabled_;
}


void Component::setEnabled(bool enabled) noexcept {
    if (enabled_ != enabled) {
        enabled_ = enabled;
        invalidate();
    }
}


bool Component::hovered() const noexcept {
    return hovered_;
}


void Component::setHovered(bool hovered) noexcept {
    if (hovered_ != hovered) {
        hovered_ = hovered;
        invalidate();
    }
}


void Component::setFocused(bool focused) noexcept {
    if (focused_ != focused) {
        focused_ = focused;
        invalidate();
    }
}

bool Component::focused() const noexcept {
    return focused_;
}


bool Component::pressed() const noexcept {
    return pressed_;
}


void Component::setPressed(bool pressed) noexcept {
    if (pressed_ != pressed) {
        pressed_ = pressed;
        invalidate();
    }
}


bool Component::visible() const noexcept {
    return visible_;
}

void Component::setVisible(bool visible) noexcept {
    if (visible_ != visible) {
        visible_ = visible;
        invalidate();
    }
}


void Component::invalidate() noexcept {
    if (view_) {
        view_->requestRedraw();
    }
}

}