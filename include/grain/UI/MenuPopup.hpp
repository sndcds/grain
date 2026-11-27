
#pragma once

#include <grain/UI/Popup.hpp>
#include <grain/UI/LayoutStyle.hpp>
#include <grain/Events/Emitter.hpp>

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace Grain {

class Image;
class Window;


//------------------------------------------------------------------------------
//  Popup item type
//------------------------------------------------------------------------------

enum class PopupItemType : std::uint8_t {
    Action,
    Separator
};


//------------------------------------------------------------------------------
//  Popup item status
//------------------------------------------------------------------------------

enum class PopupItemStatus : std::uint8_t {
    None       = 0,
    Disabled   = 1 << 0,
    Selectable = 1 << 1,
    Selected   = 1 << 2
};

constexpr auto operator|(
    PopupItemStatus lhs,
    PopupItemStatus rhs
) -> PopupItemStatus {
    return static_cast<PopupItemStatus>(
        static_cast<std::uint8_t>(lhs) |
        static_cast<std::uint8_t>(rhs)
    );
}

constexpr auto operator&(
    PopupItemStatus lhs,
    PopupItemStatus rhs
) -> PopupItemStatus {
    return static_cast<PopupItemStatus>(
        static_cast<std::uint8_t>(lhs) &
        static_cast<std::uint8_t>(rhs)
    );
}

constexpr auto hasStatus(
    PopupItemStatus status,
    PopupItemStatus flag
) -> bool {
    return (status & flag) != PopupItemStatus::None;
}

//------------------------------------------------------------------------------
//  Popup item definition
//------------------------------------------------------------------------------

struct PopupItem {
    std::string label;

    // Non-owning pointer. The Image must outlive the MenuPopup.
    const Image* icon = nullptr;

    PopupItemType type = PopupItemType::Action;
    PopupItemStatus status = PopupItemStatus::None;

    std::uint64_t tag = 0;
};

//------------------------------------------------------------------------------
//  Menu popup layout
//------------------------------------------------------------------------------

struct MenuPopupLayout {
    LayoutViewStyle view;
    LayoutComponentStyle item;
    LayoutComponentStyle separator;

    MenuPopupLayout() {
        view.direction = LayoutDirection::TopToBottom;
        view.element.padding = {8.0, 8.0, 6.0, 6.0};
        view.element.child_gap = 2.0;

        item.element.sizing.width = {
            LayoutSizingType::Grow,
            1.0,
            0.0,
            std::numeric_limits<double>::max()
        };

        item.element.sizing.height = {
            LayoutSizingType::Fixed,
            32.0,
            32.0,
            32.0
        };

        item.element.padding = {8.0, 8.0, 0.0, 0.0};

        separator.element.sizing.width = {
            LayoutSizingType::Grow,
            1.0,
            0.0,
            std::numeric_limits<double>::max()
        };

        separator.element.sizing.height = {
            LayoutSizingType::Fixed,
            9.0,
            9.0,
            9.0
        };
    }
};

//------------------------------------------------------------------------------
//  Menu popup
//------------------------------------------------------------------------------

class MenuPopup {
public:
    MenuPopup(
        Window& parent,
        std::vector<PopupItem> items,
        const Rectd& anchor,
        const Sized& size,
        const MenuPopupLayout& layout = {},
        const PopupOptions& options = {}
    );

    MenuPopup(const MenuPopup&) = delete;
    auto operator=(const MenuPopup&) -> MenuPopup& = delete;

    MenuPopup(MenuPopup&&) = delete;
    auto operator=(MenuPopup&&) -> MenuPopup& = delete;

    ~MenuPopup();

    void show();
    void hide();

    void reposition(const Rectd& anchor);
    void resize(const Sized& size);

    [[nodiscard]]
    auto isVisible() const -> bool;

    // Emits the tag of the activated item.
    Grain::Events::Emitter<std::uint64_t> selected;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain
