#include "grain/UI/Button.hpp"

#include "grain/Graphics/GraphicContext.hpp"
#include "../../include/grain/UI/View.hpp"

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
        title_.c_str(),
        bounds,
        Grain::Alignment::Center,
        font);
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
        if (layout_result_.bounds.contains(event.mouse_pos_)) {
            click();
        }
    }
    else if (event.type_ == Grain::EventType::MouseMove) {
    }
    return false;
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