#include <grain/Input/Input.hpp>

namespace Grain {

namespace {

constexpr int InvalidIndex = -1;

} // namespace

int Input::keyIndex(Key key) noexcept {
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
        keys_pressed_[index] = false;
        keys_released_[index] = false;
    }

    for (int index = 0; index < MouseButtonCount; ++index) {
        mouse_buttons_pressed_[index] = false;
        mouse_buttons_released_[index] = false;
    }

    mouse_delta_x_ = 0.0;
    mouse_delta_y_ = 0.0;
    wheel_delta_x_ = 0.0;
    wheel_delta_y_ = 0.0;
}

void Input::processEvent(const Event& event) noexcept {
    switch (event.type) {
    case EventType::KeyDown: {
        const int index = keyIndex(event.key);

        if (index < 0) {
            break;
        }

        if (!keys_down_[index]) {
            keys_pressed_[index] = true;
        }

        keys_down_[index] = true;
        break;
    }

    case EventType::KeyUp: {
        const int index = keyIndex(event.key);

        if (index < 0) {
            break;
        }

        if (keys_down_[index]) {
            keys_released_[index] = true;
        }

        keys_down_[index] = false;
        break;
    }

    case EventType::MouseMove:
        mouse_delta_x_ += event.delta_x;
        mouse_delta_y_ += event.delta_y;

        mouse_x_ = event.mouse_x;
        mouse_y_ = event.mouse_y;
        break;

    case EventType::MouseButtonDown: {
        const int index = mouseButtonIndex(event.mouse_button);

        if (index < 0) {
            break;
        }

        if (!mouse_buttons_down_[index]) {
            mouse_buttons_pressed_[index] = true;
        }

        mouse_buttons_down_[index] = true;
        break;
    }

    case EventType::MouseButtonUp: {
        const int index = mouseButtonIndex(event.mouse_button);

        if (index < 0) {
            break;
        }

        if (mouse_buttons_down_[index]) {
            mouse_buttons_released_[index] = true;
        }

        mouse_buttons_down_[index] = false;
        break;
    }

    case EventType::MouseWheel:
        wheel_delta_x_ += event.wheel_x;
        wheel_delta_y_ += event.wheel_y;
        break;

    default:
        break;
    }
}

bool Input::isKeyDown(Key key) const noexcept {
    const int index = keyIndex(key);

    return index >= 0 && keys_down_[index];
}

bool Input::wasKeyPressed(Key key) const noexcept {
    const int index = keyIndex(key);

    return index >= 0 && keys_pressed_[index];
}

bool Input::wasKeyReleased(Key key) const noexcept {
    const int index = keyIndex(key);

    return index >= 0 && keys_released_[index];
}

bool Input::isMouseButtonDown(MouseButton button) const noexcept {
    const int index = mouseButtonIndex(button);

    return index >= 0 && mouse_buttons_down_[index];
}

bool Input::wasMouseButtonPressed(MouseButton button) const noexcept {
    const int index = mouseButtonIndex(button);

    return index >= 0 && mouse_buttons_pressed_[index];
}

bool Input::wasMouseButtonReleased(MouseButton button) const noexcept {
    const int index = mouseButtonIndex(button);

    return index >= 0 && mouse_buttons_released_[index];
}

Vec2d Input::mousePosition() const noexcept {
    return Vec2d(mouse_x_, mouse_y_);
}

Vec2d Input::mouseDelta() const noexcept {
    return Vec2d(mouse_delta_x_, mouse_delta_y_);
}

} // namespace Grain
