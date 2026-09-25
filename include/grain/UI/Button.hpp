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

    void setEnabled(bool enabled) noexcept;
    [[nodiscard]] bool enabled() const noexcept;

    void setPressed(bool pressed) noexcept;
    [[nodiscard]] bool pressed() const noexcept;

private:
    std::string title_;
    Action action_;

    bool enabled_ = true;
    bool pressed_ = false;
};

}