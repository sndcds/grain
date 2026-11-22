#pragma once

#include <map>
#include <string>


namespace Grain {

enum class LayoutDirection : uint8_t {
    LeftToRight,
    TopToBottom
};


enum class LayoutAlignment : uint8_t {
    Start,
    Center,
    End
};


enum class LayoutSizingType : uint8_t {
    Fit,
    Grow,
    Fixed,
    Percent
};

struct LayoutSizingAxis {
    LayoutSizingType type = LayoutSizingType::Fit;
    double value = 0.0;
    double min_value = 0.0;
    double max_value = std::numeric_limits<double>::max();
};

struct LayoutSizing {
    LayoutSizingAxis width;
    LayoutSizingAxis height;
};


struct LayoutPadding {
    double left = 0.0;
    double right = 0.0;
    double top = 0.0;
    double bottom = 0.0;
};


struct LayoutElementStyle {
    LayoutPadding padding;
    double child_gap = 0.0;
    LayoutSizing sizing;
};

struct LayoutViewStyle {
    LayoutElementStyle element;
    LayoutDirection direction = LayoutDirection::TopToBottom;
    LayoutAlignment child_horizontal_alignment = LayoutAlignment::Start;
    LayoutAlignment child_vertical_alignment = LayoutAlignment::Start;

    // ClipConfig clip;
    // ScrollConfig scroll;
};

struct LayoutComponentStyle {
    LayoutElementStyle element;

    // FontStyle font;
    // ColorStyle color;
    // BorderStyle border;
};

}