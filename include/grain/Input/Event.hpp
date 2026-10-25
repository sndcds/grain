#pragma once

#include <grain/Math/Vec2.hpp>

#include <cstdint>
#include <functional>

namespace Grain {

enum class EventType : uint32_t {
    None,
    Quit,

    // Window lifecycle and state.
    WindowCloseRequested,
    WindowWillClose,
    WindowResize,
    WindowFocusGained,
    WindowFocusLost,

    // Keyboard.
    KeyDown,
    KeyUp,
    ModifiersChanged,

    // Mouse.
    MouseMove,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel,
};


enum class KeyCode : uint32_t {
    Unknown,

    Escape,
    Enter,
    CarriageReturn,
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

enum class MouseButton : uint8_t {
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
    Super = 1 << 3,
    Command = 1 << 4
};

constexpr auto operator | (
    KeyModifiers lhs,
    KeyModifiers rhs
    ) -> KeyModifiers {
    return static_cast<KeyModifiers>(
        static_cast<std::uint8_t>(lhs) |
        static_cast<std::uint8_t>(rhs)
    );
}

constexpr auto operator & (
    KeyModifiers lhs,
    KeyModifiers rhs
    ) -> KeyModifiers {
    return static_cast<KeyModifiers>(
        static_cast<std::uint8_t>(lhs) &
        static_cast<std::uint8_t>(rhs)
    );
}

struct Event {
    EventType type_ = EventType::None;

    uint64_t window_id_ = 0;

    // Keyboard
    KeyCode key_ = KeyCode::Unknown;
    KeyModifiers modifiers_ = KeyModifiers::None;

    // Text input
    String text_;
    char32_t character_ = U'\0';
    bool is_repeat_ = false;

    // Mouse
    Vec2d mouse_pos_;
    Vec2d mouse_delta_;
    Vec2d mouse_wheel_;
    MouseButton mouse_button_ = MouseButton::Left;
    int32_t mouse_click_count_ = 0;

    // Window dimensions.
    int32_t width_ = 0;
    int32_t height_ = 0;

    [[nodiscard]] constexpr auto hasKeyModifier(KeyModifiers modifier) const -> bool {
        return (modifiers_ & modifier) == modifier;
    }

    [[nodiscard]] constexpr auto hasKeyModifierOnly(KeyModifiers modifier) const -> bool {
        return modifiers_ == modifier;
    }
};

using EventHandler = std::function<void(const Event&)>;

} // namespace Grain