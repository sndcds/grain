#include <grain/UI/UIMenuItem.hpp>

#include <iostream>
#include <grain/Image/Image.hpp>
#include <grain/Text/Font.hpp>
#include <grain/Graphics/GraphicContext.hpp>

#include <string>

namespace Grain {

namespace {

constexpr double CheckColumnWidth = 16.0;
constexpr double CheckColumnGap = 4.0;
constexpr double IconColumnWidth = 18.0;
constexpr double IconColumnGap = 8.0;
constexpr double HorizontalPadding = 8.0;

} // namespace


UIMenuItem::UIMenuItem(
    std::string title,
    const Image* icon,
    bool selected
)
    : Button(std::move(title)),
      icon_(icon),
      selected_(selected)
{
}


auto UIMenuItem::icon() const -> const Image* {
    return icon_;
}


void UIMenuItem::setIcon(const Image* icon) {
    if (icon_ == icon) {
        return;
    }

    icon_ = icon;
    invalidate();
}


auto UIMenuItem::selected() const -> bool {
    return selected_;
}


void UIMenuItem::setSelected(bool selected) {
    std::cout << "setSelected: " << selected_ << std::endl;
    if (selected_ == selected) {
        return;
    }

    selected_ = selected;
    invalidate();
}


auto UIMenuItem::active() const -> bool {
    return active_;
}


void UIMenuItem::setActive(bool active) {
    if (active_ == active) {
        return;
    }

    active_ = active;
    invalidate();
}


void UIMenuItem::draw(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {
    (void)dirty_rect;

    const Rectd item_bounds = bounds();

    if (!visible()) {
        return;
    }

    if (!enabled()) {
        gc.setFillColor({0.92f, 0.92f, 0.92f, 0.5f});
    }
    else if (pressed()) {
        gc.setFillColor({0.72f, 0.72f, 0.72f, 1.0f});
    }
    else if (active_ || hovered()) {
        gc.setFillColor({0.82f, 0.82f, 0.82f, 1.0f});
    }
    else {
        gc.setFillColor({1.0f, 1.0f, 1.0f, 0.0f});
    }

    gc.addRoundRectPath(item_bounds, 5.0);
    gc.fillPath();

    // The checkmark column is always reserved.
    const double check_x =
        item_bounds.x + HorizontalPadding;

    if (selected_) {
        gc.setStrokeColor({0.12f, 0.12f, 0.12f, 1.0f});
        gc.setStrokeWidth(1.8);

        const double center_y =
            item_bounds.y + (item_bounds.height / 2.0);

        gc.beginPath();
        gc.moveTo({check_x + 2.0, center_y});
        gc.lineTo({check_x + 6.0, center_y + 4.0});
        gc.lineTo({check_x + 13.0, center_y - 4.0});
        gc.strokePath();
    }

    // The icon column is also always reserved.
    const double icon_x =
        check_x + CheckColumnWidth + CheckColumnGap;

    if (icon_ != nullptr) {
        constexpr double icon_size = 18.0;

        const Rectd icon_bounds{
            icon_x,
            item_bounds.y + ((item_bounds.height - icon_size) / 2.0),
            icon_size,
            icon_size
        };

        gc.drawImage(*icon_, icon_bounds);
    }

    const double text_left =
        icon_x + IconColumnWidth + IconColumnGap;

    static Font* font = new Font("SFPro-Regular", 16.0f);

    gc.setFillColor(
        enabled()
            ? Color{0.12f, 0.12f, 0.12f, 1.0f}
            : Color{0.55f, 0.55f, 0.55f, 1.0f}
    );

    const Rectd text_bounds{
        text_left,
        item_bounds.y,
        item_bounds.right() - text_left - HorizontalPadding,
        item_bounds.height
    };

    gc.drawTextInRect(
        title(),
        text_bounds,
        Alignment::Left,
        font
    );
}


void UIMenuItem::layoutSizing() {
    Button::layoutSizing();

    layout_result_.intrinsic_size.width +=
        (HorizontalPadding * 2.0) +
        CheckColumnWidth +
        CheckColumnGap +
        IconColumnWidth +
        IconColumnGap;

    layoutPreferredSizing();
}

} // namespace Grain