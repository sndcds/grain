
#pragma once

#include <algorithm>
#include <cmath>

#include <grain/Color/Color.hpp>


namespace Grain {

/**
 * @brief HSV color representation.
 *
 * Hue        : 0.0 ... 1.0 (cyclic)
 * Saturation : 0.0 ... 1.0
 * Value      : 0.0 ... 1.0
 *
 * Hue is normalized to the interval [0.0, 1.0).
 */
class HSVColor {

public:
    float hue = 0.0f;
    float saturation = 0.0f;
    float value = 0.0f;

    HSVColor() = default;

    constexpr HSVColor(
        float hue,
        float saturation,
        float value
    ) noexcept
        : hue(hue - std::floor(hue)),
          saturation(std::clamp(saturation, 0.0f, 1.0f)),
          value(std::clamp(value, 0.0f, 1.0f)) {}

    [[nodiscard]]
    constexpr auto operator == (
        const HSVColor&
    ) const -> bool = default;


    [[nodiscard]]
    static auto fromColor(
        const Color& color
    ) noexcept -> HSVColor;

    [[nodiscard]]
    auto toColor(
        float alpha = 1.0f
    ) const noexcept -> Color;


    [[nodiscard]]
    auto isSame(
        const HSVColor& other,
        float tolerance = 0.0001f
    ) const noexcept -> bool;

    [[nodiscard]]
    static auto lerp(
        const HSVColor& from,
        const HSVColor& to,
        float t
        ) noexcept -> HSVColor;

};

} // namespace Grain
