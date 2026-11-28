#include <grain/UI/MenuPopup.hpp>

#include <grain/UI/View.hpp>
#include <grain/UI/UIMenuItem.hpp>
#include <grain/UI/UIMenuSeparator.hpp>
#include <grain/Platform/Window.hpp>

#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace Grain {

namespace {

class MenuPopupView final : public View {
public:
    using ActivateHandler = std::function<void(std::size_t)>;
    using EscapeHandler = std::function<void()>;

    void setActivateHandler(ActivateHandler handler) {
        activate_handler_ = std::move(handler);
    }

    void setEscapeHandler(EscapeHandler handler) {
        escape_handler_ = std::move(handler);
    }

    auto addMenuItem(
        const PopupItem& item,
        const LayoutComponentStyle& item_style,
        const LayoutComponentStyle& separator_style
    ) -> UIMenuItem* {
        if (item.type == PopupItemType::Separator) {
            auto& separator = addChild<UIMenuSeparator>();
            separator.setStyle(separator_style);
            buttons_.push_back(nullptr);
            return nullptr;
        }

        const bool initially_selected =
            hasStatus(item.status, PopupItemStatus::Selected);

        auto& button = addChild<UIMenuItem>(
            item.label,
            item.icon,
            initially_selected
        );

        button.setStyle(item_style);
        button.setEnabled(
            !hasStatus(item.status, PopupItemStatus::Disabled)
        );

        const std::size_t index = buttons_.size();

        button.clicked.connect([this, index]() {
            activate(index);
        });

        buttons_.push_back(&button);

        return &button;
    }

    void initializeActiveItem() {
        for (std::size_t i = 0; i < buttons_.size(); ++i) {
            if (isNavigable(i)) {
                setActiveIndex(i);
                return;
            }
        }
    }

    void handleKeyEvent(const Event& event) override {
        if (event.type_ != EventType::KeyDown) {
            return;
        }

        switch (event.key_) {
            case KeyCode::Up:
                moveActive(-1);
                return;

            case KeyCode::Down:
                moveActive(1);
                return;

            case KeyCode::Enter:
            case KeyCode::CarriageReturn:
            case KeyCode::Space:
                activateActive();
                return;

            case KeyCode::Escape:
                if (escape_handler_) {
                    escape_handler_();
                }
                return;

            default:
                break;
        }

        View::handleKeyEvent(event);
    }

private:
    std::vector<UIMenuItem*> buttons_;
    std::size_t active_index_ = static_cast<std::size_t>(-1);

    ActivateHandler activate_handler_;
    EscapeHandler escape_handler_;

    [[nodiscard]]
    auto isNavigable(std::size_t index) const -> bool {
        return index < buttons_.size() &&
               buttons_[index] != nullptr &&
               buttons_[index]->enabled();
    }

    void setActiveIndex(std::size_t index) {
        if (!isNavigable(index)) {
            return;
        }

        if (active_index_ < buttons_.size() &&
            buttons_[active_index_] != nullptr) {
            buttons_[active_index_]->setActive(false);
        }

        active_index_ = index;
        buttons_[active_index_]->setActive(true);
    }

    void moveActive(int direction) {
        if (buttons_.empty()) {
            return;
        }

        const std::size_t count = buttons_.size();

        std::size_t index = active_index_;

        for (std::size_t attempt = 0; attempt < count; ++attempt) {
            if (direction > 0) {
                index = index == static_cast<std::size_t>(-1)
                    ? 0
                    : (index + 1) % count;
            }
            else {
                index = index == static_cast<std::size_t>(-1)
                    ? count - 1
                    : (index == 0 ? count - 1 : index - 1);
            }

            if (isNavigable(index)) {
                setActiveIndex(index);
                return;
            }
        }
    }

    void activate(std::size_t index) {
        if (!isNavigable(index)) {
            return;
        }

        setActiveIndex(index);

        if (activate_handler_) {
            activate_handler_(index);
        }
    }

    void activateActive() {
        if (isNavigable(active_index_)) {
            buttons_[active_index_]->click();
        }
    }
};

} // namespace


class MenuPopup::Impl {
public:
    MenuPopup* owner_ = nullptr;

    // These styles must outlive the View and its child Components.
    MenuPopupLayout layout_;

    std::vector<PopupItem> items_;
    std::vector<UIMenuItem*> buttons_;

    // Destroy Popup before destroying layout_ and items_.
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
        auto content = std::make_unique<MenuPopupView>();

        buttons_.reserve(items_.size());

        for (const PopupItem& item : items_) {
            UIMenuItem* button = content->addMenuItem(
                item,
                layout_.item,
                layout_.separator
            );

            buttons_.push_back(button);
        }

        content->setActivateHandler(
            [this](std::size_t index) {
                activateItem(index);
            }
        );

        content->setEscapeHandler([this]() {
            if (popup_) {
                popup_->hide();
            }
        });

        // content->initializeActiveItem();

        popup_ = std::make_unique<Popup>(
            parent,
            std::move(content),
            anchor,
            size,
            options
        );

        popup_->setContentStyle(layout_.view);
    }

    void activateItem(std::size_t index) {
        if (index >= items_.size()) {
            return;
        }

        PopupItem& item = items_[index];

        if (item.type == PopupItemType::Separator ||
            hasStatus(item.status, PopupItemStatus::Disabled)) {
            return;
        }

        const bool selectable =
            hasStatus(item.status, PopupItemStatus::Selectable);

        if (selectable) {
            const bool was_selected =
                hasStatus(item.status, PopupItemStatus::Selected);

            const bool is_selected = !was_selected;

            if (is_selected) {
                item.status = item.status | PopupItemStatus::Selected;
            }
            else {
                item.status = static_cast<PopupItemStatus>(
                    static_cast<std::uint8_t>(item.status) &
                    ~static_cast<std::uint8_t>(PopupItemStatus::Selected)
                );
            }

            if (buttons_[index] != nullptr) {
                buttons_[index]->setSelected(is_selected);
            }
        }

        owner_->selected.emit(item.tag);

        if (popup_) {
            popup_->hide();
        }
    }
};


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