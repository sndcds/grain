#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>

namespace Grain::Math {

inline constexpr double kPi = std::numbers::pi;
inline constexpr double kTau = std::numbers::pi * 2.0;
inline constexpr double kEpsilon = std::numeric_limits<double>::epsilon();
inline constexpr double kEpsilonFloat = std::numeric_limits<float>::epsilon();


//------------------------------------------------------------------------------
// Angle
//------------------------------------------------------------------------------

[[nodiscard]] constexpr auto degreesToRadians(double degrees) -> double {
    return degrees * kPi / 180.0;
}

[[nodiscard]] constexpr auto radiansToDegrees(double radians) -> double {
    return radians * 180.0 / kPi;
}


//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

[[nodiscard]] constexpr auto lerp(double a, double b, double t) -> double {
    return a + (t * (b - a));
}

[[nodiscard]] constexpr auto lerp(float a, float b, float t) -> float {
    return a + (t * (b - a));
}

[[nodiscard]] constexpr auto inverseLerp(double a, double b, double value) -> double {
    return a != b ? (value - a) / (b - a) : 0;
}

[[nodiscard]] constexpr auto inverseLerp(float a, float b, float value) -> float {
    return a != b ? (value - a) / (b - a) : 0;
}


//------------------------------------------------------------------------------
// Remapping
//------------------------------------------------------------------------------

[[nodiscard]] constexpr auto remap(
    double in_min,
    double in_max,
    double out_min,
    double out_max,
    double value
) -> double {
    return in_min != in_max
        ? lerp(
            out_min,
            out_max,
            inverseLerp(in_min, in_max, value)
        )
        : out_min;
}

[[nodiscard]] constexpr auto remap(
    float in_min,
    float in_max,
    float out_min,
    float out_max,
    float value
) -> float {
    return in_min != in_max
        ? lerp(
            out_min,
            out_max,
            inverseLerp(in_min, in_max, value)
        )
        : out_min;
}

[[nodiscard]] constexpr auto normalize(
    double min,
    double max,
    double value
) -> double {
    return inverseLerp(min, max, value);
}

[[nodiscard]] constexpr auto normalize(
    float min,
    float max,
    float value
) -> float {
    return inverseLerp(min, max, value);
}

[[nodiscard]] constexpr auto remapClamped(
    double in_min,
    double in_max,
    double out_min,
    double out_max,
    double value
) -> double {
    const double result = remap(in_min, in_max, out_min, out_max, value);

    const double lower = std::min(out_min, out_max);
    const double upper = std::max(out_min, out_max);

    return std::clamp(result, lower, upper);
}

[[nodiscard]] constexpr auto remapClamped(
    float in_min,
    float in_max,
    float out_min,
    float out_max,
    float value
) -> float {
    const float result = remap(in_min, in_max, out_min, out_max, value);

    const float lower = std::min(out_min, out_max);
    const float upper = std::max(out_min, out_max);

    return std::clamp(result, lower, upper);
}


//------------------------------------------------------------------------------
// Step functions
//------------------------------------------------------------------------------

[[nodiscard]] constexpr auto unitStep(
    double threshold,
    double value
) -> double {
    return value < threshold ? 0.0 : 1.0;
}

[[nodiscard]] constexpr auto unitStep(
    float threshold,
    float value
) -> float {
    return value < threshold ? 0.0f : 1.0f;
}

[[nodiscard]] constexpr auto unitStep(
    double threshold,
    double range,
    double value
) -> double {
    if (value < threshold - range) {
        return 0;
    }

    if (value > threshold + range) {
        return 1;
    }

    return normalize(
        threshold - range,
        threshold + range,
        value
    );
}

[[nodiscard]] constexpr auto unitStep(
    float threshold,
    float range,
    float value
) -> float {
    if (value < threshold - range) {
        return 0;
    }

    if (value > threshold + range) {
        return 1;
    }

    return normalize(
        threshold - range,
        threshold + range,
        value
    );
}


//------------------------------------------------------------------------------
// Smooth interpolation
//------------------------------------------------------------------------------

[[nodiscard]] constexpr auto smoothStep(double t) -> double {
    return t * t * (3.0 - 2.0 * t);
}

[[nodiscard]] constexpr auto smoothStep(float t) -> float {
    return t * t * (3.0f - (2.0f * t));
}

[[nodiscard]] constexpr auto smoothStep(double a, double b, double t) -> double {
    return lerp(a, b, smoothStep(t));
}

[[nodiscard]] constexpr auto smoothStep(float a, float b, float t) -> float {
    return lerp(a, b, smoothStep(t));
}

[[nodiscard]] constexpr auto smootherStep(double t) -> double {
    return t * t * t * (t * ((6.0 * t) - 15.0) + 10.0);
}

[[nodiscard]] constexpr auto smootherStep(float t) -> float {
    return t * t * t * (t * ((6.0f * t) - 15.0f) + 10.0f);
}

[[nodiscard]] constexpr auto smootherStep(double a, double b, double t ) -> double {
    return lerp(a, b, smootherStep(t));
}

[[nodiscard]] constexpr auto smootherStep(float a, float b, float t) -> float {
    return lerp(a, b, smootherStep(t));
}


//------------------------------------------------------------------------------
// Wrapping
//------------------------------------------------------------------------------

[[nodiscard]] inline auto wrap(double value) -> double {
    return value - std::floor(value);
}

[[nodiscard]] inline auto wrap(double value, double min, double max) -> double {
    const double range = max - min;

    return min +
        std::fmod(
            std::fmod(value - min, range) + range,
            range
        );
}

[[nodiscard]] inline auto pingPong(double value) -> double {
    constexpr double period = 2.0;

    double x = std::fmod(value, period);

    if (x < 0.0) {
        x += period;
    }

    return 1.0 - std::abs(x - 1.0);
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------


static void buildPowLookUpTable(float power, int32_t resolution, float* out_lut) noexcept {
    if (out_lut && resolution >= 2) {
        float f = 1.0f / static_cast<float>(resolution - 1);
        for (int32_t i = 0; i < resolution; i++) {
            out_lut[i] = std::pow(f * static_cast<float>(i), power);
        }
    }
}

[[nodiscard]] static auto gaussKernel(double x, double sigma_sqr) noexcept -> double {
    // TODO: Check formula!
    double p = std::pow(M_E, -((x * x) / (sigma_sqr * 2)));
    return (1.0 / (std::sqrt(std::numbers::pi * 2 * sigma_sqr))) * p;
}

[[nodiscard]] static auto sinc(double x) noexcept -> double {
    return x != 0 ? std::sin(x) / x : 1;
    // TODO: Check formula!
    // return x == 0.0 ? 1.0 : std::sin(std::numbers::pi * x) / (std::numbers::pi * x);
}

[[nodiscard]] static auto bessel(double x) noexcept -> double {
    double sum = 1.0;

    for (int32_t i = 1; i < 10; i++) {
        double x_to_i_power = std::pow(x / 2.0, static_cast<double>(i));
        double factorial = 1.0;
        for (int32_t j = 1; j <= i; j++) {
            factorial *= j;
        }
        sum += std::pow(x_to_i_power / factorial, 2.0);
    }

    return sum;
}


//------------------------------------------------------------------------------
//  Ratios
//------------------------------------------------------------------------------

[[nodiscard]] inline auto quotient(double dividend, double divisor) -> double {
    return divisor <= kEpsilon
        ? std::numeric_limits<double>::quiet_NaN()
        : dividend / divisor;
}

[[nodiscard]] inline auto percent(double part, double full) -> double {
    return full <= kEpsilon
        ? std::numeric_limits<double>::quiet_NaN()
        : part / full * 100.0;
}

[[nodiscard]] inline auto xPerY(double x, double y) -> double {
    return y <= kEpsilon
        ? std::numeric_limits<double>::quiet_NaN()
        : x / y;
}

[[nodiscard]] inline auto xPerYInt(double x, double y) -> int64_t {
    if (y <= kEpsilon) {
        return 0;
    }

    return static_cast<int64_t>(std::round(x / y));
}


//------------------------------------------------------------------------------
//  Integer / discrete math
//------------------------------------------------------------------------------

auto greatestCommonDivisor(int64_t a, int64_t b ) -> int64_t;

auto factorial(int32_t n) -> int64_t;

auto sumN(int32_t n) -> int64_t;

auto roundToNearestPowerOfTwo(double value) -> int64_t;

auto stepsToReachAtOrAfterInt(int64_t value, int64_t step, int64_t target ) -> int64_t;

auto stepsToReachAtOrAfter(double value, double step, double target) -> int64_t;


//------------------------------------------------------------------------------
//  Powers of two
//------------------------------------------------------------------------------

auto nextLog2(int64_t value) -> int32_t;

auto isPowerOfTwo(int64_t value) -> bool;

auto log2IfPowerOfTwo(int64_t value) -> int32_t;

auto nextPowerOfTwo(int64_t value) -> int64_t;


//------------------------------------------------------------------------------
// Trigonometry
//------------------------------------------------------------------------------

[[nodiscard]] inline auto secant(double angle) -> double {
    const double cosine = std::cos(degreesToRadians(angle));

    return cosine != 0.0
        ? 1.0 / cosine
        : std::numeric_limits<double>::quiet_NaN();
}

} // namespace Grain::Math