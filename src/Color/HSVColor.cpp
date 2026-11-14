#include <algorithm>
#include <cmath>

#include <grain/Color/HSVColor.hpp>
#include <grain/Color/Color.hpp>


namespace Grain {


[[nodiscard]]
auto HSVColor::fromColor(
    const Color& color
) noexcept -> HSVColor {

    const float r = std::clamp(color.red,   0.0f, 1.0f);
    const float g = std::clamp(color.green, 0.0f, 1.0f);
    const float b = std::clamp(color.blue,  0.0f, 1.0f);

    const float maximum = std::max({r, g, b});
    const float minimum = std::min({r, g, b});

    const float delta = maximum - minimum;

    float h = 0.0f;
    float s = 0.0f;

    if (delta > 0.0f) {

        if (maximum == r) {
            h = (g - b) / delta;

            if (h < 0.0f) {
                h += 6.0f;
            }
        }
        else if (maximum == g) {
            h = (b - r) / delta + 2.0f;
        }
        else {
            h = (r - g) / delta + 4.0f;
        }

        h /= 6.0f;

        s = maximum > 0.0f
            ? delta / maximum
            : 0.0f;
    }

    return {h, s, maximum};
}


[[nodiscard]]
auto HSVColor::toColor(
    float alpha
) const noexcept -> Color {

    const float h = hue * 6.0f;

    const float c = value * saturation;

    const float x = c * (
        1.0f - std::abs(
            std::fmod(h, 2.0f) - 1.0f
        )
    );

    const float m = value - c;

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    if (h < 1.0f) {
        r = c;
        g = x;
    }
    else if (h < 2.0f) {
        r = x;
        g = c;
    }
    else if (h < 3.0f) {
        g = c;
        b = x;
    }
    else if (h < 4.0f) {
        g = x;
        b = c;
    }
    else if (h < 5.0f) {
        r = x;
        b = c;
    }
    else {
        r = c;
        b = x;
    }

    return {
        r + m,
        g + m,
        b + m,
        std::clamp(alpha, 0.0f, 1.0f)
    };
}


[[nodiscard]]
auto HSVColor::isSame(
    const HSVColor& other,
    float tolerance
) const noexcept -> bool {

    tolerance = std::max(tolerance, 0.0f);

    // Hue is irrelevant for achromatic colors.
    if (
        saturation <= tolerance &&
        other.saturation <= tolerance
    ) {
        return
            std::abs(value - other.value) <= tolerance;
    }

    const float hueDifference = std::abs(
        hue - other.hue
    );

    const float circularHueDifference = std::min(
        hueDifference,
        1.0f - hueDifference
    );

    return
        circularHueDifference <= tolerance &&
        std::abs(saturation - other.saturation) <= tolerance &&
        std::abs(value - other.value) <= tolerance;
}


[[nodiscard]]
auto HSVColor::lerp(
    const HSVColor& from,
    const HSVColor& to,
    float t
) noexcept -> HSVColor {

    t = std::clamp(t, 0.0f, 1.0f);

    // Shortest path around the hue circle.
    float hueDelta = to.hue - from.hue;

    if (hueDelta > 0.5f) {
        hueDelta -= 1.0f;
    }
    else if (hueDelta < -0.5f) {
        hueDelta += 1.0f;
    }

    float h = from.hue + hueDelta * t;

    // Normalize hue to [0.0, 1.0).
    h -= std::floor(h);

    // Linear interpolation of saturation and value.
    const float s = std::lerp(
        from.saturation,
        to.saturation,
        t
    );

    const float v = std::lerp(
        from.value,
        to.value,
        t
    );

    return {h, s, v};
}


} // namespace Grain
