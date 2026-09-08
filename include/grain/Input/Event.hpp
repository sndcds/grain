#pragma once

#include <cstdint>

namespace Grain {

using WindowId = std::uint64_t;

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
    EventType type = EventType::None;

    WindowId windowId = 0;

    // Keyboard
    Key key = Key::Unknown;
    KeyModifiers modifiers = KeyModifiers::None;

    // Text input.
    char32_t character = U'\0';

    // Mouse position.
    double mouseX = 0.0;
    double mouseY = 0.0;

    // Mouse movement.
    double deltaX = 0.0;
    double deltaY = 0.0;

    // Mouse button.
    MouseButton mouseButton = MouseButton::Left;

    // Mouse wheel.
    double wheelX = 0.0;
    double wheelY = 0.0;

    // Window dimensions.
    int width = 0;
    int height = 0;
};

} // namespace Grain