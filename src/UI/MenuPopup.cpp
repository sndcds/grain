
#include <grain/UI/MenuPopup.hpp>

#include <grain/UI/Button.hpp>
#include <grain/UI/View.hpp>
#include <grain/Platform/Window.hpp>
#include <grain/Image/Image.hpp>
#include <grain/Text/Font.hpp>
#include <grain/Graphics/GraphicContext.hpp>

#include <memory>
#include <utility>

namespace Grain {

namespace {

class MenuPopupButton final : public Button {
public:
    MenuPopupButton(
        std::string title,
        const Image* icon,
        bool selected
    )
        : Button(std::move(title)),
          icon_(icon),
          selected_(selected)
    {
    }


    [[nodiscard]]
    auto selected() const -> bool {
        return selected_;
    }

    void setSelected(bool selected) {
        if (selected_ == selected) {
            return;
        }

        selected_ = selected;
        invalidate();
    }

    void draw(
        GraphicContext& gc,
        const Rectd& dirty_rect
    ) override {
        (void)dirty_rect;

        const Rectd item_bounds = bounds();

        if (!visible()) {
            return;
        }

        if (!enabled()) {
            gc.setFillColor({0.92f, 0.92f, 0.92f, 1.0f});
        }
        else if (pressed()) {
            gc.setFillColor({0.78f, 0.82f, 0.90f, 1.0f});
        }
        else if (hovered()) {
            gc.setFillColor({0.86f, 0.89f, 0.94f, 1.0f});
        }
        else if (selected_) {
            gc.setFillColor({0.88f, 0.91f, 0.97f, 1.0f});
        }
        else {
            gc.setFillColor({1.0f, 1.0f, 1.0f, 0.0f});
        }

        gc.addRoundRectPath(
            item_bounds,
            5.0
        );
        gc.fillPath();

        double text_left = item_bounds.x + 8.0;

        if (icon_ != nullptr) {
            constexpr double icon_size = 18.0;

            const Rectd icon_bounds{
                item_bounds.x + 8.0,
                item_bounds.y + (item_bounds.height - icon_size) / 2.0,
                icon_size,
                icon_size
            };

            gc.drawImage(*icon_, icon_bounds);

            text_left += icon_size + 8.0;
        }

        static Font* font = new Font("SFPro-Regular", 16.0f);

        gc.setFillColor(
            enabled()
                ? Color{0.12f, 0.12f, 0.12f, 1.0f}
                : Color{0.55f, 0.55f, 0.55f, 1.0f}
        );

        const Rectd text_bounds{
            text_left,
            item_bounds.y,
            item_bounds.right() - text_left - 8.0,
            item_bounds.height
        };

        gc.drawTextInRect(
            title(),
            text_bounds,
            Alignment::Center,
            font
        );
    }

    void layoutSizing() override {
        Button::layoutSizing();

        if (icon_ != nullptr) {
            layout_result_.intrinsic_size.width += 26.0;
        }
    }

private:
    const Image* icon_ = nullptr;
    bool selectable_ = false;
    bool selected_ = false;
};

} // namespace


//------------------------------------------------------------------------------
//  MenuPopup::Impl
//------------------------------------------------------------------------------

class MenuPopup::Impl {
public:
    MenuPopup* owner_ = nullptr;

    // These styles must outlive the View and its child Components.
    MenuPopupLayout layout_;

    std::vector<PopupItem> items_;

    // Destroy the Popup before destroying layout_ and items_.
    std::unique_ptr<Popup> popup_;

    Impl(
        MenuPopup& owner,
        Window& parent,
        std::vector<PopupItem> items,
        const Rectd& anchor,
        const Sized& size,
        const MenuPopupLayout& layout,
        const PopupOptions& options
    )
        : owner_(&owner),
          layout_(layout),
          items_(std::move(items))
    {
        auto content = std::make_unique<View>();

        for (const PopupItem& item : items_) {
            const bool disabled =
                hasStatus(item.status, PopupItemStatus::Disabled);

            const bool selectable =
                hasStatus(item.status, PopupItemStatus::Selectable);

            const bool initially_selected =
                hasStatus(item.status, PopupItemStatus::Selected);

            auto& button = content->addChild<MenuPopupButton>(
                item.label,
                item.icon,
                initially_selected
            );

            button.setStyle(layout_.item);
            button.setEnabled(!disabled);

            if (!disabled) {
                const std::uint64_t tag = item.tag;

                MenuPopupButton* button_ptr = &button;

                button.clicked.connect(
                    [this, button_ptr, tag, selectable]() {
                        if (selectable) {
                            button_ptr->setSelected(!button_ptr->selected());
                        }

                        owner_->selected.emit(tag);

                        if (popup_) {
                            popup_->hide();
                        }
                    }
                );
            }
        }

        popup_ = std::make_unique<Popup>(
            parent,
            std::move(content),
            anchor,
            size,
            options
        );

        popup_->setContentStyle(layout_.view);
    }
};


//------------------------------------------------------------------------------
//  MenuPopup
//------------------------------------------------------------------------------

MenuPopup::MenuPopup(
    Window& parent,
    std::vector<PopupItem> items,
    const Rectd& anchor,
    const Sized& size,
    const MenuPopupLayout& layout,
    const PopupOptions& options
)
    : impl_(std::make_unique<Impl>(
        *this,
        parent,
        std::move(items),
        anchor,
        size,
        layout,
        options
    ))
{
}


MenuPopup::~MenuPopup() = default;


void MenuPopup::show() {
    if (impl_ && impl_->popup_) {
        impl_->popup_->show();
    }
}


void MenuPopup::hide() {
    if (impl_ && impl_->popup_) {
        impl_->popup_->hide();
    }
}


void MenuPopup::reposition(const Rectd& anchor) {
    if (impl_ && impl_->popup_) {
        impl_->popup_->reposition(anchor);
    }
}


void MenuPopup::resize(const Sized& size) {
    if (impl_ && impl_->popup_) {
        impl_->popup_->resize(size);
    }
}


auto MenuPopup::isVisible() const -> bool {
    return impl_ &&
           impl_->popup_ &&
           impl_->popup_->isVisible();
}

} // namespace Grain
