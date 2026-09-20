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

[[nodiscard]]
constexpr double degreesToRadians(double degrees) noexcept {
    return degrees * kPi / 180.0;
}

[[nodiscard]]
constexpr double radiansToDegrees(double radians) noexcept {
    return radians * 180.0 / kPi;
}


//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

[[nodiscard]]
constexpr double lerp(double a, double b, double t) noexcept {
    return a + t * (b - a);
}

[[nodiscard]]
constexpr float lerp(float a, float b, float t) noexcept {
    return a + t * (b - a);
}

[[nodiscard]]
constexpr double inverseLerp(double a, double b, double value) noexcept {
    return a != b ? (value - a) / (b - a) : 0.0;
}

[[nodiscard]]
constexpr float inverseLerp(float a, float b, float value) noexcept {
    return a != b ? (value - a) / (b - a) : 0.0f;
}


//------------------------------------------------------------------------------
// Remapping
//------------------------------------------------------------------------------

[[nodiscard]]
constexpr double remap(
    double inputMin,
    double inputMax,
    double outputMin,
    double outputMax,
    double value
) noexcept
{
    return inputMin != inputMax
        ? lerp(
            outputMin,
            outputMax,
            inverseLerp(inputMin, inputMax, value)
        )
        : outputMin;
}

[[nodiscard]]
constexpr float remap(
    float inputMin,
    float inputMax,
    float outputMin,
    float outputMax,
    float value
) noexcept
{
    return inputMin != inputMax
        ? lerp(
            outputMin,
            outputMax,
            inverseLerp(inputMin, inputMax, value)
        )
        : outputMin;
}

[[nodiscard]]
constexpr double normalize(
    double min,
    double max,
    double value
) noexcept
{
    return inverseLerp(min, max, value);
}

[[nodiscard]]
constexpr float normalize(
    float min,
    float max,
    float value
) noexcept
{
    return inverseLerp(min, max, value);
}

[[nodiscard]]
constexpr double remapClamped(
    double inputMin,
    double inputMax,
    double outputMin,
    double outputMax,
    double value
) noexcept
{
    const double result =
        remap(inputMin, inputMax, outputMin, outputMax, value);

    const double lower = std::min(outputMin, outputMax);
    const double upper = std::max(outputMin, outputMax);

    return std::clamp(result, lower, upper);
}

[[nodiscard]]
constexpr float remapClamped(
    float inputMin,
    float inputMax,
    float outputMin,
    float outputMax,
    float value
) noexcept
{
    const float result =
        remap(inputMin, inputMax, outputMin, outputMax, value);

    const float lower = std::min(outputMin, outputMax);
    const float upper = std::max(outputMin, outputMax);

    return std::clamp(result, lower, upper);
}


//------------------------------------------------------------------------------
// Step functions
//------------------------------------------------------------------------------

[[nodiscard]]
constexpr double unitStep(
    double threshold,
    double value
) noexcept
{
    return value < threshold ? 0.0 : 1.0;
}

[[nodiscard]]
constexpr float unitStep(
    float threshold,
    float value
) noexcept
{
    return value < threshold ? 0.0f : 1.0f;
}

[[nodiscard]]
constexpr double unitStep(
    double threshold,
    double range,
    double value
) noexcept
{
    if (value < threshold - range)
        return 0.0;

    if (value > threshold + range)
        return 1.0;

    return normalize(
        threshold - range,
        threshold + range,
        value
    );
}

[[nodiscard]]
constexpr float unitStep(
    float threshold,
    float range,
    float value
) noexcept
{
    if (value < threshold - range)
        return 0.0f;

    if (value > threshold + range)
        return 1.0f;

    return normalize(
        threshold - range,
        threshold + range,
        value
    );
}


//------------------------------------------------------------------------------
// Smooth interpolation
//------------------------------------------------------------------------------

[[nodiscard]]
constexpr double smoothStep(double t) noexcept {
    return t * t * (3.0 - 2.0 * t);
}

[[nodiscard]]
constexpr float smoothStep(float t) noexcept {
    return t * t * (3.0f - 2.0f * t);
}

[[nodiscard]]
constexpr double smoothStep(
    double a,
    double b,
    double t
) noexcept
{
    return lerp(a, b, smoothStep(t));
}

[[nodiscard]]
constexpr float smoothStep(
    float a,
    float b,
    float t
) noexcept
{
    return lerp(a, b, smoothStep(t));
}

[[nodiscard]]
constexpr double smootherStep(double t) noexcept {
    return t * t * t *
        (t * (6.0 * t - 15.0) + 10.0);
}

[[nodiscard]]
constexpr float smootherStep(float t) noexcept {
    return t * t * t *
        (t * (6.0f * t - 15.0f) + 10.0f);
}

[[nodiscard]]
constexpr double smootherStep(
    double a,
    double b,
    double t
) noexcept
{
    return lerp(a, b, smootherStep(t));
}

[[nodiscard]]
constexpr float smootherStep(
    float a,
    float b,
    float t
) noexcept
{
    return lerp(a, b, smootherStep(t));
}


//------------------------------------------------------------------------------
// Wrapping
//------------------------------------------------------------------------------

[[nodiscard]]
inline double wrap(double value) noexcept {
    return value - std::floor(value);
}

[[nodiscard]]
inline double wrap(
    double value,
    double min,
    double max
) noexcept
{
    const double range = max - min;

    return min +
        std::fmod(
            std::fmod(value - min, range) + range,
            range
        );
}

[[nodiscard]]
inline double pingPong(double value) noexcept
{
    constexpr double period = 2.0;

    double x = std::fmod(value, period);

    if (x < 0.0)
        x += period;

    return 1.0 - std::abs(x - 1.0);
}


//------------------------------------------------------------------------------
// Ratios
//------------------------------------------------------------------------------

[[nodiscard]]
inline double quotient(
    double dividend,
    double divisor
) noexcept
{
    return divisor <= kEpsilon
        ? std::numeric_limits<double>::quiet_NaN()
        : dividend / divisor;
}

[[nodiscard]]
inline double percent(
    double part,
    double full
) noexcept
{
    return full <= kEpsilon
        ? std::numeric_limits<double>::quiet_NaN()
        : part / full * 100.0;
}

[[nodiscard]]
inline double xPerY(
    double x,
    double y
) noexcept
{
    return y <= kEpsilon
        ? std::numeric_limits<double>::quiet_NaN()
        : x / y;
}

[[nodiscard]]
inline int64_t xPerYInt(
    double x,
    double y
) noexcept
{
    if (y <= kEpsilon)
        return 0;

    return static_cast<int64_t>(std::round(x / y));
}


//------------------------------------------------------------------------------
// Integer / discrete math
//------------------------------------------------------------------------------

int64_t greatestCommonDivisor(
    int64_t a,
    int64_t b
) noexcept;

int64_t factorial(int32_t n) noexcept;

int64_t sumN(int32_t n) noexcept;

int64_t roundToNearestPowerOfTwo(double value) noexcept;

int64_t stepsToReachAtOrAfterInt(
    int64_t value,
    int64_t step,
    int64_t target
) noexcept;

int64_t stepsToReachAtOrAfter(
    double value,
    double step,
    double target
) noexcept;


//------------------------------------------------------------------------------
// Powers of two
//------------------------------------------------------------------------------

int32_t nextLog2(int64_t value) noexcept;

bool isPowerOfTwo(int64_t value) noexcept;

int32_t log2IfPowerOfTwo(int64_t value) noexcept;

int64_t nextPowerOfTwo(int64_t value) noexcept;


//------------------------------------------------------------------------------
// Trigonometry
//------------------------------------------------------------------------------

[[nodiscard]]
inline double secant(double angle) noexcept {
    const double cosine = std::cos(degreesToRadians(angle));

    return cosine != 0.0
        ? 1.0 / cosine
        : std::numeric_limits<double>::quiet_NaN();
}

} // namespace Grain::Math