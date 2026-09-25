#include "grain/UI/Button.hpp"

#include "grain/Graphics/GraphicContext.hpp"
#include "grain/Platform/View.hpp"

namespace Grain {

Button::Button(View* view, std::string title)
    : Component(view),
      title_(std::move(title))
{
}

void Button::draw(GraphicContext& gc) {
    std::cout << "draw button\n";

    gc.setFillColor({1, 0, 0, 0.4});
    gc.fillRect(bounds_);
}

void Button::setTitle(std::string title) {
    title_ = std::move(title);
    view()->requestRedraw();
}

const std::string& Button::title() const noexcept {
    return title_;
}

void Button::setAction(Action action) {
    action_ = std::move(action);
}

void Button::click() {
    if (enabled_ && action_) {
        action_();
    }
}

void Button::setEnabled(bool enabled) noexcept {
    if (enabled_ == enabled) {
        return;
    }

    enabled_ = enabled;

    if (view()) {
        view()->requestRedraw();
    }
}

bool Button::enabled() const noexcept {
    return enabled_;
}

void Button::setPressed(bool pressed) noexcept {
    pressed_ = pressed;

    if (view()) {
        view()->requestRedraw();
    }
}

bool Button::pressed() const noexcept {
    return pressed_;
}

}