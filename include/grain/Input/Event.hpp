#pragma once

#include <cstdint>

#include <grain/Geometry/Vec2.hpp>

namespace Grain {

enum class EventType {
    None,

    Quit,

    WindowClose,
    WindowResize,
    WindowMove,
    WindowFocus,
    WindowBlur,

    KeyDown,
    KeyUp,
    TextInput,

    MouseMove,
    MouseDown,
    MouseUp,
    MouseWheel,

    MouseEnter,
    MouseLeave
};

enum class MouseButton {
    Left,
    Middle,
    Right,
    X1,
    X2
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

struct Modifiers {
    bool shift = false;
    bool control = false;
    bool alt = false;
    bool super = false;
};

struct Event {
    EventType type = EventType::None;

    Vec2d position{};
    Vec2d delta{};

    Key key = Key::Unknown;
    MouseButton button = MouseButton::Left;

    Modifiers modifiers{};

    double wheelX = 0.0;
    double wheelY = 0.0;

    char32_t character = U'\0';

    int width = 0;
    int height = 0;

    bool repeat = false;
};

} // namespace Grain