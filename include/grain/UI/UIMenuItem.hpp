#pragma once

#include <grain/UI/Button.hpp>

namespace Grain {

class Image;

class UIMenuItem final : public Button {
public:
    explicit UIMenuItem(
        std::string title = {},
        const Image* icon = nullptr,
        bool selected = false
    );

    [[nodiscard]]
    auto icon() const -> const Image*;

    void setIcon(const Image* icon);

    [[nodiscard]]
    auto selected() const -> bool;

    void setSelected(bool selected);

    [[nodiscard]]
    auto active() const -> bool;

    void setActive(bool active);

    void draw(
        GraphicContext& gc,
        const Rectd& dirty_rect
    ) override;

    void layoutSizing() override;

private:
    // Non-owning. The Image must outlive this component.
    const Image* icon_ = nullptr;

    bool selected_ = false;
    bool active_ = false;
};

} // namespace Grain