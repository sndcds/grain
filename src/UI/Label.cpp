
#include "grain/UI/Label.hpp"

#include "grain/Graphics/GraphicContext.hpp"

#include <iostream>
#include <utility>
#include <grain/UI/View.hpp>

namespace Grain {

Label::Label(std::string text)
    : text_(std::move(text))
{
}

void Label::setText(std::string text) {
    if (text_ == text) {
        return;
    }

    text_ = std::move(text);

    if (auto* parent = parentView()) {
        parent->requestRedraw(bounds_);
    }
}

const std::string& Label::text() const noexcept {
    return text_;
}

void Label::draw(GraphicContext& gc) {

    std::cout << "Label::draw()" << std::endl;

    static Font* font;
    if (font == nullptr) {
        font = new Grain::Font("SFPro-Regular", 15.0f);
    }

    if (!visible_) {
        return;
    }

    gc.setFillColor({1, 0, 0, 1});
    gc.fillRect(bounds_);

    // TODO: Set text color!

    gc.drawTextInRect(
        text_,
        bounds_,
        Alignment::Left,
        font);
}

void Label::doSizing() {
    std::cout << "Label::doSizing()" << std::endl;
}


}
