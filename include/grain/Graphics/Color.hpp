#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Grain {

/**
 * @brief RGBA color.
 *
 * Color is the standard color representation used by Grain.
 *
 * All components are floating-point values normally in the range
 * 0.0 to 1.0:
 *
 *   red   = 0.0 ... 1.0
 *   green = 0.0 ... 1.0
 *   blue  = 0.0 ... 1.0
 *   alpha = 0.0 ... 1.0
 *
 * Color uses straight (unpremultiplied) alpha.
 */
class Color {
public:
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
    float alpha = 1.0f;

public:
    constexpr Color() = default;

    constexpr Color(
        float red,
        float green,
        float blue,
        float alpha = 1.0f)
        : red(red),
          green(green),
          blue(blue),
          alpha(alpha) {}

    constexpr explicit Color(
        float grey,
        float alpha = 1.0f)
        : red(grey),
          green(grey),
          blue(grey),
          alpha(alpha) {}

    [[nodiscard]]
    static constexpr Color black() {
        return Color(0.0f, 0.0f, 0.0f);
    }

    [[nodiscard]]
    static constexpr Color white() {
        return Color(1.0f, 1.0f, 1.0f);
    }

    [[nodiscard]]
    static constexpr Color transparent() {
        return Color(0.0f, 0.0f, 0.0f, 0.0f);
    }

    //--------------------------------------------------------------------------
    //  Approximate black-body color, based on the common temperature-to-RGB
    //  approximation. Temperature is expressed in kelvin.
    //--------------------------------------------------------------------------
    static Color kelvinColor(float kelvin) {
        const float temperature = std::clamp(kelvin, 1000.0f, 40000.0f);
        const float t = temperature / 100.0f;

        float red;
        float green;
        float blue;

        if (t <= 66.0f) {
            red = 255.0f;
            green = 99.4708025861f * std::log(t)
                  - 161.1195681661f;

            if (t <= 19.0f) {
                blue = 0.0f;
            }
            else {
                blue = 138.5177312231f * std::log(t - 10.0f)
                     - 305.0447927307f;
            }
        }
        else {
            red = 329.698727446f * std::pow(t - 60.0f, -0.1332047592f);
            green = 288.1221695283f * std::pow(t - 60.0f, -0.0755148492f);
            blue = 255.0f;
        }

        return Color(
            std::clamp(red, 0.0f, 1.0f),
            std::clamp(green, 0.0f, 1.0f),
            std::clamp(blue, 0.0f, 1.0f),
            1.0f
        );
    }


    // Test for exact equality.
    [[nodiscard]]
    constexpr bool operator==(const Color&) const = default;

    // Test for approximate equality.
    [[nodiscard]]
    bool isSame(const Color& other, float tolerance = 0.0001f) const;

    [[nodiscard]]
    static Color lerp(const Color& a, const Color& b, float t);

    [[nodiscard]]
    static Color mixboxLerp(const Color& a, const Color& b, float t);

    // Convert to packed RGBA 32-bit representation.
    [[nodiscard]]
    uint32_t rgba32() const;

    // Create a color from packed RGBA 32-bit representation.
    [[nodiscard]]
    static constexpr Color fromRGBA32(
        uint32_t value) {

        return Color(
            static_cast<float>((value >> 24) & 0xff) / 255.0f,
            static_cast<float>((value >> 16) & 0xff) / 255.0f,
            static_cast<float>((value >> 8) & 0xff) / 255.0f,
            static_cast<float>(value & 0xff) / 255.0f
        );
    }
};

} // namespace Grain
