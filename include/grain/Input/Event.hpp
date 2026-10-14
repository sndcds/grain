#pragma once

#include <grain/Math/Vec2.hpp>

#include <cstdint>
#include <functional>

namespace Grain {

enum class EventType {
    None,

    Quit,

    WindowClose,
    WindowResize,
    WindowFocusGained,
    WindowFocusLost,

    KeyDown,
    KeyUp,
    TextInput,

    MouseMove,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel
};

enum class Key {
    Unknown,

    Escape,
    Enter,
    Tab,
    Backspace,
    Space,

    Left,
    Right,
    Up,
    Down,

    Home,
    End,
    PageUp,
    PageDown,
    Insert,
    Delete,

    Shift,
    Control,
    Alt,
    Super,

    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12
};

enum class MouseButton {
    None,
    Left,
    Right,
    Middle,
    Other
};

enum class KeyModifiers : std::uint8_t {
    None = 0,
    Shift = 1 << 0,
    Control = 1 << 1,
    Alt = 1 << 2,
    Super = 1 << 3
};

constexpr KeyModifiers operator|(
    KeyModifiers lhs,
    KeyModifiers rhs
    ) noexcept {
    return static_cast<KeyModifiers>(
        static_cast<std::uint8_t>(lhs) |
        static_cast<std::uint8_t>(rhs)
    );
}

constexpr KeyModifiers operator&(
    KeyModifiers lhs,
    KeyModifiers rhs
    ) noexcept {
    return static_cast<KeyModifiers>(
        static_cast<std::uint8_t>(lhs) &
        static_cast<std::uint8_t>(rhs)
    );
}

constexpr bool hasModifier(
    KeyModifiers modifiers,
    KeyModifiers modifier
    ) noexcept {
    return (modifiers & modifier) == modifier;
}

struct Event {
    EventType type_ = EventType::None;

    uint64_t window_id_ = 0;

    // Keyboard
    Key key_ = Key::Unknown;
    KeyModifiers modifiers_ = KeyModifiers::None;

    // Text input
    char32_t character_ = U'\0';

    // Mouse
    Vec2d mouse_pos_{};
    Vec2d mouse_delta_{};
    Vec2d mouse_wheel_{};
    MouseButton mouse_button_ = MouseButton::Left;

    // Window dimensions.
    int width_ = 0;
    int height_ = 0;
};

using EventHandler = std::function<void(const Event&)>;

} // namespace Grain