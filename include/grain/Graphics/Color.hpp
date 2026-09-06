#pragma once

namespace Grain {

struct Color {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
    float alpha = 1.0f;

    static constexpr Color black() noexcept {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

    static constexpr Color white() noexcept {
        return {1.0f, 1.0f, 1.0f, 1.0f};
    }

    static constexpr Color redColor() noexcept {
        return {1.0f, 0.0f, 0.0f, 1.0f};
    }

    static constexpr Color greenColor() noexcept {
        return {0.0f, 1.0f, 0.0f, 1.0f};
    }

    static constexpr Color blueColor() noexcept {
        return {0.0f, 0.0f, 1.0f, 1.0f};
    }
};

} // namespace Grain