
#pragma once

#include <grain/UI/Component.hpp>
#include <grain/Color/Color.hpp>

#include <string>

namespace Grain {

class Label : public Component {
public:
    explicit Label(std::string text = {});

    void draw(
        GraphicContext& gc,
        const Rectd& dirty_rect
    ) override;

    void layoutSizing() override;

    void setText(std::string text);
    [[nodiscard]] auto text() const noexcept -> const std::string&;

private:
    std::string text_;
};

}
