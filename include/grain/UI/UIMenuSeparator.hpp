#pragma once

#include <grain/UI/Component.hpp>

namespace Grain {

class UIMenuSeparator final : public Component {
public:
    void draw(
        GraphicContext& gc,
        const Rectd& dirty_rect
    ) override;

    void layoutSizing() override;
};

} // namespace Grain