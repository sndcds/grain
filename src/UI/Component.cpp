#include "grain/UI/Component.hpp"

#include "grain/Platform/View.hpp"

namespace Grain {

void Component::setEnabled(bool enabled) noexcept {
    if (enabled_ == enabled) {
        return;
    }

    enabled_ = enabled;

    if (view_) {
        view_->requestRedraw();
    }
}

bool Component::enabled() const noexcept {
    return enabled_;
}

void Component::setPressed(bool pressed) noexcept {
    pressed_ = pressed;

    if (view_) {
        view_->requestRedraw();
    }
}

bool Component::pressed() const noexcept {
    return pressed_;
}


void Component::invalidate() noexcept {
    view_->requestRedraw();
}

}