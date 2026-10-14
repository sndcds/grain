
#include "grain/UI/Label.hpp"

#include "grain/Graphics/GraphicContext.hpp"

#include <utility>

namespace Grain {

Label::Label(View* view, std::string text)
    : Component(view),
      text_(std::move(text))
{
}

void Label::setText(std::string text) {
    if (text_ == text) {
        return;
    }

    text_ = std::move(text);
    view()->requestRedraw();
}

const std::string& Label::text() const noexcept {
    return text_;
}

void Label::draw(GraphicContext& gc) {
    static Font* font;
    if (font == nullptr) {
        font = new Grain::Font("SFPro-Regular", 15.0f);
    }

    if (!visible_) {
        return;
    }

    gc.drawTextInRect(
        text_,
        bounds_,
        Alignment::Left,
        font,
        enabled_ ? Color(1, 0, 0, 1) : Color(.1, .1, .1, 1));
}

}
