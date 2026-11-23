
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

    gc.setFillColor({1, 1, 1, 1});
    gc.fillRect(layout_result_.bounds);

    // TODO: Set text color!

    gc.drawTextInRect(
        text_,
        layout_result_.bounds,
        Alignment::Center,
        font);


    gc.save();
    gc.translate(layout_result_.bounds.x, layout_result_.bounds.y);
    gc.setFillColor({0, 0, 0, .8});

    gc.beginPath();
    gc.setBlendMode(ColorBlendMode::Normal);

    GraphicPath camera{16};
    camera.addPoint(13.997, 4);
    camera.addArcAsBezier({2, 2}, 0, false, true, {15.757, 5.05});
    camera.addPoint(16.243, 5.95);
    camera.addArcAsBezier({2, 2}, 0, false, false, {18.003, 7});
    camera.addPoint(20, 7);
    camera.addArcAsBezier({2, 2}, 0, false, true, {22, 9});
    camera.addPoint(22, 18);
    camera.addArcAsBezier({2, 2}, 0, false, true, {20, 20});
    camera.addPoint(4, 20);
    camera.addArcAsBezier({2, 2}, 0, false, true, {2, 18});
    camera.addPoint(2, 9);
    camera.addArcAsBezier({2, 2}, 0, false, true, {4, 7});
    camera.addPoint(5.997, 7);
    camera.addArcAsBezier({2, 2}, 0, false, false, {7.756, 5.952});
    camera.addPoint(8.245, 5.048);
    camera.addArcAsBezier({2, 2}, 0, false, true, {10.004, 4});
    camera.setClosed(true);

    gc.addPath(camera);

    GraphicPath lens{8};
    lens.addPoint(12, 10);
    lens.addArcAsBezier({3, 3}, 0, true, true, {12, 16});
    lens.addArcAsBezier({3, 3}, 0, true, true, {12, 10});
    lens.setClosed(true);

    gc.addPath(lens);

    gc.fillStrokedPath(1.3, StrokeCapStyle::Round, StrokeJoinStyle::Round, 10.0f);
    gc.strokePath();

    gc.restore();
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
