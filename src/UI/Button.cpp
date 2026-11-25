#include "grain/UI/Button.hpp"

#include "grain/Graphics/GraphicContext.hpp"
#include "../../include/grain/UI/View.hpp"

#include <iostream>

namespace Grain {

Button::Button(std::string title)
    : title_(std::move(title))
{
}


void Button::draw(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    const auto bounds = layout_result_.bounds;
    static Font* font;
    if (font == nullptr) {
        font = new Grain::Font("SFPro-Regular", 16.0f);
    }

    auto b = bounds;
    b.inset(0);
    double radius = bounds.height / 2.5;

    float alpha = 1;

    if (pressed_) {
        gc.setFillColor({0.9, 0.6, 0.6, 1});
    }
    else if (hovered_) {
        gc.setFillColor({1, 0.8, 0.8, 1});
    }
    else {
        gc.setFillColor({1, 1, 1, 0.4});
    }

    gc.addRoundRectPath(b, radius);
    gc.fillPath();
    // gc.setStrokeSize(1);
    // gc.addRoundRectPath(b, radius);
    gc.setStrokeColor({0, 0, 0, 1});
    // gc.strokePath();


    gc.drawTextInRect(
        title_.c_str(),
        bounds,
        Grain::Alignment::Center,
        font);
}


void Button::setTitle(std::string title) {
    if (title_ == title) {
        return;
    }

    title_ = std::move(title);
    invalidate();
}


auto Button::title() const noexcept -> const std::string& {
    return title_;
}


void Button::setAction(Action action) {
    action_ = std::move(action);
}


void Button::click() {
    if (enabled_) {
        clicked.emit();
    }
}


auto Button::handleEvent(const Event& event) -> bool {

    switch (event.type_) {

        case EventType::MouseButtonDown: {
            if (event.mouse_button_ != MouseButton::Left) {
                return false;
            }

            if (!contains(event.mouse_pos_)) {
                return false;
            }

            setPressed(true);
            return true;
        }

        case EventType::MouseButtonUp: {
            if (event.mouse_button_ != MouseButton::Left) {
                return false;
            }

            if (!pressed()) {
                return false;
            }

            const bool should_click =
                contains(event.mouse_pos_);

            setPressed(false);

            if (should_click) {
                click();
            }

            return true;
        }

        default:
            return false;
    }
}


void Button::layoutSizing() {

    static Font* font;
    if (font == nullptr) {
        font = new Font("SFPro-Regular", 16.0f);
    }

    layout_result_.intrinsic_size = font->textDimension(title_.c_str());

    layoutPreferredSizing();
}


}