#include <grain/Input/Input.hpp>

namespace Grain {

namespace {

constexpr int InvalidIndex = -1;

} // namespace

int Input::keyIndex(KeyCode key) noexcept {
    const auto value = static_cast<int>(key);

    if (value < 0 || value >= KeyCount) {
        return InvalidIndex;
    }

    return value;
}

int Input::mouseButtonIndex(MouseButton button) noexcept {
    const auto value = static_cast<int>(button);

    if (value < 0 || value >= MouseButtonCount) {
        return InvalidIndex;
    }

    return value;
}

void Input::beginFrame() noexcept {
    for (int index = 0; index < KeyCount; ++index) {
        keysPressed_[index] = false;
        keysReleased_[index] = false;
    }

    for (int index = 0; index < MouseButtonCount; ++index) {
        mouseButtonsPressed_[index] = false;
        mouseButtonsReleased_[index] = false;
    }

    mouse_delta_ = {0.0, 0.0};
    mouse_wheel_delta_ = {0.0, 0.0};
}

void Input::processEvent(const Event& event) noexcept {
    switch (event.type_) {
    case EventType::KeyDown: {
        const int index = keyIndex(event.key_);

        if (index < 0) {
            break;
        }

        if (!keysDown_[index]) {
            keysPressed_[index] = true;
        }

        keysDown_[index] = true;
        break;
    }

    case EventType::KeyUp: {
        const int index = keyIndex(event.key_);

        if (index < 0) {
            break;
        }

        if (keysDown_[index]) {
            keysReleased_[index] = true;
        }

        keysDown_[index] = false;
        break;
    }

    case EventType::MouseMove:
        mouse_delta_ += event.mouse_delta_;
        mouse_pos_ = event.mouse_pos_;
        break;

    case EventType::MouseButtonDown: {
        const int index = mouseButtonIndex(event.mouse_button_);

        if (index < 0) {
            break;
        }

        if (!mouseButtonsDown_[index]) {
            mouseButtonsPressed_[index] = true;
        }

        mouseButtonsDown_[index] = true;
        break;
    }

    case EventType::MouseButtonUp: {
        const int index = mouseButtonIndex(event.mouse_button_);

        if (index < 0) {
            break;
        }

        if (mouseButtonsDown_[index]) {
            mouseButtonsReleased_[index] = true;
        }

        mouseButtonsDown_[index] = false;
        break;
    }

    case EventType::MouseWheel:
        mouse_wheel_delta_ += event.mouse_wheel_;
        break;

    default:
        break;
    }
}

bool Input::isKeyDown(KeyCode key) const noexcept {
    const int index = keyIndex(key);

    return index >= 0 && keysDown_[index];
}

bool Input::wasKeyPressed(KeyCode key) const noexcept {
    const int index = keyIndex(key);

    return index >= 0 && keysPressed_[index];
}

bool Input::wasKeyReleased(KeyCode key) const noexcept {
    const int index = keyIndex(key);

    return index >= 0 && keysReleased_[index];
}

bool Input::isMouseButtonDown(MouseButton button) const noexcept {
    const int index = mouseButtonIndex(button);

    return index >= 0 && mouseButtonsDown_[index];
}

bool Input::wasMouseButtonPressed(MouseButton button) const noexcept {
    const int index = mouseButtonIndex(button);

    return index >= 0 && mouseButtonsPressed_[index];
}

bool Input::wasMouseButtonReleased(MouseButton button) const noexcept {
    const int index = mouseButtonIndex(button);

    return index >= 0 && mouseButtonsReleased_[index];
}

Vec2d Input::mousePosition() const noexcept {
    return mouse_pos_;
}

Vec2d Input::mouseDelta() const noexcept {
    return mouse_delta_;
}

} // namespace Grain
