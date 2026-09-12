#pragma once

#include <grain/Input/Event.hpp>
#include <grain/Math/Vec2.hpp>

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

    bool keysDown_[KeyCount]{};
    bool keysPressed_[KeyCount]{};
    bool keysReleased_[KeyCount]{};

    bool mouseButtonsDown_[MouseButtonCount]{};
    bool mouseButtonsPressed_[MouseButtonCount]{};
    bool mouseButtonsReleased_[MouseButtonCount]{};

    double mouseX_ = 0.0;
    double mouseY_ = 0.0;

    double mouseDeltaX_ = 0.0;
    double mouseDeltaY_ = 0.0;

    double wheelDeltaX_ = 0.0;
    double wheelDeltaY_ = 0.0;
};

} // namespace Grain