#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Math/Vec2.hpp>

#include <cstdint>
#include <vector>

namespace Grain {

struct FontPathElement {
    enum class Type : uint8_t {
        MoveTo,
        LineTo,
        QuadraticTo,
        CubicTo,
        Close
    };

    Type type = Type::MoveTo;
    Vec2d points[3]{};
};

struct FontPath {
    std::vector<FontPathElement> elements;
    Rectd bounds;
};

} // namespace Grain