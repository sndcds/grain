#pragma once

#include <grain/Input/Event.hpp>
#include <grain/Geometry/Vec2.hpp>

namespace Grain {

class Input {
public:
    Input() = default;

    Input(const Input&) = delete;

    Input& operator=(const Input&) = delete;

    Input(Input&&) = delete;

    Input& operator=(Input&&) = delete;

    ~Input() = default;

    [[nodiscard]]
    bool isKeyDown(Key key) const noexcept;

    [[nodiscard]]
    bool wasKeyPressed(Key key) const noexcept;

    [[nodiscard]]
    bool wasKeyReleased(Key key) const noexcept;

    [[nodiscard]]
    bool isMouseButtonDown(MouseButton button) const noexcept;

    [[nodiscard]]
    bool wasMouseButtonPressed(MouseButton button) const noexcept;

    [[nodiscard]]
    bool wasMouseButtonReleased(MouseButton button) const noexcept;

    [[nodiscard]]
    Vec2d mousePosition() const noexcept;

    [[nodiscard]]
    Vec2d mouseDelta() const noexcept;

    void beginFrame() noexcept;

    void processEvent(const Event& event) noexcept;

private:
    static constexpr int KeyCount = 256;
    static constexpr int MouseButtonCount = 4;

    static int keyIndex(Key key) noexcept;

    static int mouseButtonIndex(
        MouseButton button
        ) noexcept;

    bool keys_down_[KeyCount]{};
    bool keys_pressed_[KeyCount]{};
    bool keys_released_[KeyCount]{};

    bool mouse_buttons_down_[MouseButtonCount]{};
    bool mouse_buttons_pressed_[MouseButtonCount]{};
    bool mouse_buttons_released_[MouseButtonCount]{};

    double mouse_x_ = 0.0;
    double mouse_y_ = 0.0;

    double mouse_delta_x_ = 0.0;
    double mouse_delta_y_ = 0.0;

    double wheel_delta_x_ = 0.0;
    double wheel_delta_y_ = 0.0;
};

} // namespace Grain