#include <grain/Graphics/Color.hpp>

#include <cmath>
#include <algorithm>

#include "../../external/mixbox/mixbox.hpp"


namespace Grain {

bool Color::isSame(const Color& other, float tolerance) const {
    return std::abs(red - other.red) <= tolerance &&
           std::abs(green - other.green) <= tolerance &&
           std::abs(blue - other.blue) <= tolerance &&
           std::abs(alpha - other.alpha) <= tolerance;
}


Color Color::lerp(const Color& a, const Color& b, float t) {
    return Color(
        a.red   + t * (b.red   - a.red),
        a.green + t * (b.green - a.green),
        a.blue  + t * (b.blue  - a.blue),
        a.alpha + t * (b.alpha - a.alpha)
    );
}


Color Color::mixboxLerp(const Color& a, const Color& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);

    float red;
    float green;
    float blue;

    mixbox_lerp_float(
        a.red, a.green, a.blue,
        b.red, b.green, b.blue,
        t,
        &red, &green, &blue
    );

    const float alpha = a.alpha + (b.alpha - a.alpha) * t;

    return Color(
        std::clamp(red, 0.0f, 1.0f),
        std::clamp(green, 0.0f, 1.0f),
        std::clamp(blue, 0.0f, 1.0f),
        std::clamp(alpha, 0.0f, 1.0f)
    );
}


uint32_t Color::rgba32() const {

    auto toByte = [](float value) -> uint32_t {
        value = std::clamp(value, 0.0f, 1.0f);
        return static_cast<uint32_t>(value * 255.0f + 0.5f);
    };

    return (toByte(red)   << 24) |
           (toByte(green) << 16) |
           (toByte(blue)  << 8)  |
           toByte(alpha);
}

} // namespace Grain
