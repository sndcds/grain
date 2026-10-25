#pragma once

#include <grain/UI/Component.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Graphics/Color.hpp>

#include <functional>
#include <string>

namespace Grain {

class Button : public Component {
public:
    using Action = std::function<void()>;

    explicit Button(View* view, std::string title = {});

    void draw(GraphicContext& gc) override;

    void setTitle(std::string title);
    [[nodiscard]] auto title() const noexcept -> const std::string&;

    void setAction(Action action);
    void click();

    auto handleEvent(const Event& event) -> bool override;


private:
    std::string title_;
    Action action_;
};

}