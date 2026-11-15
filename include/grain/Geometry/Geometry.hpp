#pragma once

namespace Grain {

enum class Alignment : int8_t {
    No,
    TopLeft,
    Left,
    BottomLeft,
    Top,
    Center,
    Bottom,
    TopRight,
    Right,
    BottomRight
};

enum class Edge : int8_t {
    Top = 0,
    Right,
    Bottom,
    Left
};

enum class FitMode : int8_t {
    Cover,
    Fit,
    Stretch,
    Center
};

} // namespace Grain