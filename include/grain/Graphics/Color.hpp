#pragma once

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
        constexpr Color() noexcept = default;

        constexpr Color(
            float red,
            float green,
            float blue,
            float alpha = 1.0f) noexcept
            : red(red),
              green(green),
              blue(blue),
              alpha(alpha) {}

        /**
         * @brief Create a grey color.
         */
        constexpr explicit Color(
            float grey,
            float alpha = 1.0f) noexcept
            : red(grey),
              green(grey),
              blue(grey),
              alpha(alpha) {}

        /**
         * @brief Create black.
         */
        [[nodiscard]]
        static constexpr Color black() noexcept {
            return Color(0.0f, 0.0f, 0.0f);
        }

        /**
         * @brief Create white.
         */
        [[nodiscard]]
        static constexpr Color white() noexcept {
            return Color(1.0f, 1.0f, 1.0f);
        }

        /**
         * @brief Create a fully transparent color.
         *
         * RGB values are zero because the color uses straight alpha.
         */
        [[nodiscard]]
        static constexpr Color transparent() noexcept {
            return Color(0.0f, 0.0f, 0.0f, 0.0f);
        }

        /**
         * @brief Test for exact equality.
         */
        [[nodiscard]]
        constexpr bool operator==(const Color&) const noexcept = default;

        /**
         * @brief Test for approximate equality.
         */
        [[nodiscard]]
        bool isSame(
            const Color& other,
            float tolerance = 0.0001f) const noexcept;

        /**
         * @brief Linearly interpolate between two colors.
         *
         * t = 0 returns a.
         * t = 1 returns b.
         *
         * The RGB and alpha components are interpolated independently.
         */
        [[nodiscard]]
        static Color lerp(
            const Color& a,
            const Color& b,
            float t) noexcept;

        /**
         * @brief Convert to packed RGBA 32-bit representation.
         *
         * The byte order is:
         *
         *   0xRRGGBBAA
         */
        [[nodiscard]]
        uint32_t rgba32() const noexcept;

        /**
         * @brief Create a color from packed RGBA 32-bit representation.
         *
         * The byte order is:
         *
         *   0xRRGGBBAA
         */
        [[nodiscard]]
        static constexpr Color fromRGBA32(
            uint32_t value) noexcept {

            return Color(
                static_cast<float>((value >> 24) & 0xff) / 255.0f,
                static_cast<float>((value >> 16) & 0xff) / 255.0f,
                static_cast<float>((value >> 8) & 0xff) / 255.0f,
                static_cast<float>(value & 0xff) / 255.0f
            );
        }
    };

} // namespace Grain
