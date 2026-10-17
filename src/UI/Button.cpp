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
    static Font* font;
    if (font == nullptr) {
        font = new Grain::Font("SFPro-Regular", 15.0f);
    }

    auto b = bounds_;
    b.inset(.5);
    double radius = bounds_.height / 2.5;

    float alpha = 1;
    if (hovered_) {
        gc.setFillColor({1, .8, .8, 1});
    }
    else {
        gc.setFillColor({1, 1, 1, .4});
        alpha = 0.4;
    }

    gc.addRoundRectPath(b, radius);
    gc.fillPath();
    // gc.setStrokeSize(1);
    // gc.addRoundRectPath(b, radius);
    gc.setStrokeColor({0, 0, 0, 1});
    // gc.strokePath();


    gc.drawTextInRect(
        "kulturbytes",
        bounds_,
        Grain::Alignment::Center,
        font,
        Color(0, 0, 0, alpha));
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

bool Button::handleEvent(const Event& event) {
    if (event.type_ == Grain::EventType::MouseButtonDown) {
        if (bounds_.contains(event.mouse_pos_)) {
            std::cout << "Button::handleEvent" << std::endl;
            click();
        }
    }
    else if (event.type_ == Grain::EventType::MouseMove) {
        std::cout << "MouseMove: " << event.mouse_delta_ << std::endl;
    }
    return false;
}


}