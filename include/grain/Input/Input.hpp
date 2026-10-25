#pragma once

#include <grain/Input/Event.hpp>
#include <grain/Math/Vec2.hpp>

namespace Grain {

class Input {
public:
    Input() = default;

    Input(const Input&) = delete;

    auto operator = (const Input&) -> Input& = delete;

    Input(Input&&) = delete;

    auto operator = (Input&&) -> Input& = delete;

    ~Input() = default;

    [[nodiscard]] auto isKeyDown(KeyCode key) const -> bool;
    [[nodiscard]] auto wasKeyPressed(KeyCode key) const -> bool;
    [[nodiscard]] auto wasKeyReleased(KeyCode key) const -> bool;
    [[nodiscard]] auto isMouseButtonDown(MouseButton button) const -> bool;
    [[nodiscard]] auto wasMouseButtonPressed(MouseButton button) const -> bool;
    [[nodiscard]] auto wasMouseButtonReleased(MouseButton button) const -> bool;

    [[nodiscard]] auto mousePosition() const -> Vec2d;
    [[nodiscard]] auto mouseDelta() const -> Vec2d;

    void beginFrame();

    void processEvent(const Event& event);

private:
    static constexpr int KeyCount = 256;
    static constexpr int MouseButtonCount = 4;

    static auto keyIndex(KeyCode key) -> int;
    static auto mouseButtonIndex(MouseButton button) -> int;

    std::array<bool, KeyCount> keys_down_{};
    std::array<bool, KeyCount> keys_pressed_{};
    std::array<bool, KeyCount> keys_released_{};

    std::array<bool, MouseButtonCount> mouse_buttons_down_{};
    std::array<bool, MouseButtonCount> mouse_buttons_pressed_{};
    std::array<bool, MouseButtonCount> mouse_buttons_released_{};

    Vec2d mouse_pos_;
    Vec2d mouse_delta_;
    Vec2d mouse_wheel_delta_;
};

} // namespace Grain