#include <algorithm>
#include <grain/Input/Input.hpp>

namespace Grain {

namespace {

constexpr int InvalidIndex = -1;

} // namespace

auto Input::keyIndex(KeyCode key) -> int {
    const auto value = static_cast<int>(key);
    if (value < 0 || value >= KeyCount) {
        return InvalidIndex;
    }

    return value;
}

auto Input::mouseButtonIndex(MouseButton button) -> int {
    const auto value = static_cast<int>(button);
    if (value < 0 || value >= MouseButtonCount) {
        return InvalidIndex;
    }

    return value;
}

void Input::beginFrame() {
    std::ranges::fill(keys_pressed_, false);
    std::ranges::fill(keys_released_, false);

    std::ranges::fill(mouse_buttons_pressed_, false);
    std::ranges::fill(mouse_buttons_released_, false);

    mouse_delta_ = {0.0, 0.0};
    mouse_wheel_delta_ = {0.0, 0.0};
}

void Input::processEvent(const Event& event) {
    switch (event.type_) {
    case EventType::KeyDown: {
        const auto index = keyIndex(event.key_);
        if (index < 0) {
            break;
        }

        const auto i = static_cast<std::size_t>(index);

        if (!keys_down_[i]) {
            keys_pressed_[i] = true;
        }

        keys_down_[i] = true;
        break;
    }

    case EventType::KeyUp: {
        const auto index = keyIndex(event.key_);
        if (index < 0) {
            break;
        }

        const auto i = static_cast<std::size_t>(index);

        if (keys_down_[i]) {
            keys_released_[i] = true;
        }

        keys_down_[i] = false;
        break;
    }

    case EventType::MouseMove:
        mouse_delta_ += event.mouse_delta_;
        mouse_pos_ = event.mouse_pos_;
        break;

    case EventType::MouseButtonDown: {
        const auto index = mouseButtonIndex(event.mouse_button_);
        if (index < 0) {
            break;
        }

        const auto i = static_cast<std::size_t>(index);

        if (!mouse_buttons_down_[i]) {
            mouse_buttons_pressed_[i] = true;
        }

        mouse_buttons_down_[i] = true;
        break;
    }

    case EventType::MouseButtonUp: {
        const auto index = mouseButtonIndex(event.mouse_button_);
        if (index < 0) {
            break;
        }

        const auto i = static_cast<std::size_t>(index);

        if (mouse_buttons_down_[index]) {
            mouse_buttons_released_[index] = true;
        }

        mouse_buttons_down_[index] = false;
        break;
    }

    case EventType::MouseWheel:
        mouse_wheel_delta_ += event.mouse_wheel_;
        break;

    default:
        break;
    }
}

auto Input::isKeyDown(KeyCode key) const -> bool {
    const int index = keyIndex(key);
    return index >= 0 && keys_down_[index];
}

auto Input::wasKeyPressed(KeyCode key) const -> bool {
    const int index = keyIndex(key);
    return index >= 0 && keys_pressed_[index];
}

auto Input::wasKeyReleased(KeyCode key) const -> bool {
    const int index = keyIndex(key);
    return index >= 0 && keys_released_[index];
}

auto Input::isMouseButtonDown(MouseButton button) const -> bool {
    const int index = mouseButtonIndex(button);
    return index >= 0 && mouse_buttons_down_[index];
}

auto Input::wasMouseButtonPressed(MouseButton button) const -> bool {
    const int index = mouseButtonIndex(button);
    return index >= 0 && mouse_buttons_pressed_[index];
}

auto Input::wasMouseButtonReleased(MouseButton button) const -> bool {
    const int index = mouseButtonIndex(button);
    return index >= 0 && mouse_buttons_released_[index];
}

auto Input::mousePosition() const -> Vec2d {
    return mouse_pos_;
}

auto Input::mouseDelta() const -> Vec2d {
    return mouse_delta_;
}

} // namespace Grain
