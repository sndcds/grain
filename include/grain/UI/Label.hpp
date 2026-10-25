
#pragma once

#include <grain/UI/Component.hpp>
#include <grain/Graphics/Color.hpp>

#include <string>

namespace Grain {

class Label : public Component {
public:
    explicit Label(View* view, std::string text = {});

    void draw(GraphicContext& gc) override;

    void setText(std::string text);
    [[nodiscard]] auto text() const noexcept -> const std::string&;

private:
    std::string text_;
};

}
