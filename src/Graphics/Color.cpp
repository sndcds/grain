#include <grain/Graphics/Color.hpp>

#include <cmath>
#include <algorithm>

namespace Grain {

bool Color::isSame(
    const Color& other,
    float tolerance) const noexcept {

    return std::abs(red - other.red) <= tolerance &&
           std::abs(green - other.green) <= tolerance &&
           std::abs(blue - other.blue) <= tolerance &&
           std::abs(alpha - other.alpha) <= tolerance;
}


Color Color::lerp(
    const Color& a,
    const Color& b,
    float t) noexcept {

    return Color(
        a.red   + t * (b.red   - a.red),
        a.green + t * (b.green - a.green),
        a.blue  + t * (b.blue  - a.blue),
        a.alpha + t * (b.alpha - a.alpha)
    );
}


uint32_t Color::rgba32() const noexcept {

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
