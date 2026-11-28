
#pragma once

#include <grain/Platform/Popup.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Size.hpp>
#include <grain/UI/LayoutStyle.hpp>

#include <memory>

namespace Grain {

class Window;
class View;


//------------------------------------------------------------------------------
//  A Popup is associated with an existing Window.
//
//  The parent Window must outlive the Popup. Destroy the Popup before
//  destroying its parent Window.
//------------------------------------------------------------------------------
class Popup {
public:
    Popup(
        Window& parent,
        std::unique_ptr<View> content,
        const Rectd& anchor,
        const Sized& size,
        const PopupOptions& options = {}
    );

    Popup(const Popup&) = delete;
    auto operator = (const Popup&) -> Popup& = delete;

    Popup(Popup&&) = delete;
    auto operator = (Popup&&) -> Popup& = delete;

    ~Popup();

    void show();
    void hide();

    void reposition(const Rectd& anchor);
    void resize(const Sized& size);

    void setContentStyle(const LayoutViewStyle& style);

    [[nodiscard]]
    auto isVisible() const -> bool;

    [[nodiscard]]
    auto contentView() -> View&;


private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain
