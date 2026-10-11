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
    [[nodiscard]] const std::string& title() const noexcept;

    void setAction(Action action);
    void click();

    bool handleEvent(const Event& event) override {
        if (event.type == Grain::EventType::MouseButtonDown) {
            if (bounds_.contains(Vec2(event.mouseX, event.mouseY))) {
                std::cout << "Button::handleEvent" << std::endl;
                click();
            }
        }
        return false;
    }


private:
    std::string title_;
    Action action_;
};

}