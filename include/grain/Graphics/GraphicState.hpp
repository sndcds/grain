#pragma once

#include <grain/Graphics/Color.hpp>
#include <grain/Math/Mat3.hpp>

namespace Grain {

enum class StrokeJoinStyle : int8_t {
    Miter,
    Round,
    Bevel
};

enum class StrokeCapStyle : int8_t {
    Butt,
    Round,
    Square
};

enum class FillWindingRule : int8_t {
    NoneZero = 0,
    EvenOdd
};

enum class ColorBlendMode : int8_t {
    Undefined = -1,
    Normal = 0,
    Multiply = 1,
    Screen = 2,
    Overlay = 3,
    Darken = 4,
    Lighten = 5,
    ColorDodge = 6,
    ColorBurn = 7,
    SoftLight = 8,
    HardLight = 9,
    Difference = 10,
    Exclusion = 11,
    Hue = 12,
    Saturation = 13,
    Color = 14,
    Luminosity = 15,
    Clear = 16,
    Copy = 17,
    SourceIn = 18,
    SourceOut = 19,
    SourceAtop = 20,
    DestinationOver = 21,
    DestinationIn = 22,
    DestinationOut = 23,
    DestinationAtop = 24,
    XOR = 25,
    PlusDarker = 26,
    PlusLighter = 27,

    First = 0,
    Last = PlusLighter
};

struct GraphicState {
    Color fillColor = Color::black();
    Color strokeColor = Color::black();

    double lineWidth = 1.0;

    StrokeJoinStyle lineJoin = StrokeJoinStyle::Miter;
    StrokeCapStyle lineCap = StrokeCapStyle::Butt;

    double miterLimit = 10.0;

    ColorBlendMode blendMode = ColorBlendMode::Normal;

    float alpha = 1.0f;

    Mat3<double> transform;
};

} // namespace Grain