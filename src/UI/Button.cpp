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
    if (!font) {
        font = new Grain::Font("SFPro-Regular", 15.0f);
    }

    auto b = bounds_;
    b.inset(.5);
    double radius = bounds_.height / 2.5;
    gc.setFillColor({1, 1, 1, 1});
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
        {0, 0, 0, 1});
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

}