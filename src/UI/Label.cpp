
#include <grain/UI/Label.hpp>

#include <grain/Graphics/GraphicContext.hpp>
#include <grain/UI/UIStyles.hpp>

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
        parent->requestRedraw(layout_result_.bounds);
    }
}

const std::string& Label::text() const noexcept {
    return text_;
}


void Label::draw(GraphicContext& gc,const Rectd& dirty_rect) {

    static Font* font;
    if (font == nullptr) {
        font = new Font("SFPro-Regular", 15.0f);
    }

    if (!visible_) {
        return;
    }

    gc.setFillColor({1, 1, 1, .8f});
    gc.fillRect(layout_result_.bounds);

    // TODO: Set text color!

    gc.drawTextInRect(
        text_,
        layout_result_.bounds,
        Alignment::Center,
        font);
}


void Label::layoutSizing() {

    static Font* font;
    if (font == nullptr) {
        font = new Font("SFPro-Regular", 16.0f);
    }

    layout_result_.intrinsic_size = font->textDimension(text_.c_str());

    layoutPreferredSizing();
}


}
