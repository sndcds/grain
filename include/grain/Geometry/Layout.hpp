#pragma once

namespace Grain {

enum class Alignment {
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

enum class FitMode {
    Cover,
    Fit,
    Stretch,
    Center
};

} // namespace Grain