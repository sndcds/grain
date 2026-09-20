#pragma once

#include <cstddef>
#include <string_view>

namespace Grain::Easing {

enum class Type {
    InSine,
    OutSine,
    InOutSine,

    InQuad,
    OutQuad,
    InOutQuad,

    InCubic,
    OutCubic,
    InOutCubic,

    InQuart,
    OutQuart,
    InOutQuart,

    InQuint,
    OutQuint,
    InOutQuint,

    InExpo,
    OutExpo,
    InOutExpo,

    InCirc,
    OutCirc,
    InOutCirc,

    InBack,
    OutBack,
    InOutBack,

    InElastic,
    OutElastic,
    InOutElastic,

    InBounce,
    OutBounce,
    InOutBounce
};

inline constexpr std::size_t typeCount = 30;

[[nodiscard]]
double evaluate(Type type, double t) noexcept;

[[nodiscard]]
std::string_view typeName(Type type) noexcept;


//------------------------------------------------------------------------------
// Sine
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInSine(double t) noexcept;

[[nodiscard]]
double easeOutSine(double t) noexcept;

[[nodiscard]]
double easeInOutSine(double t) noexcept;


//------------------------------------------------------------------------------
// Quadratic
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInQuad(double t) noexcept;

[[nodiscard]]
double easeOutQuad(double t) noexcept;

[[nodiscard]]
double easeInOutQuad(double t) noexcept;


//------------------------------------------------------------------------------
// Cubic
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInCubic(double t) noexcept;

[[nodiscard]]
double easeOutCubic(double t) noexcept;

[[nodiscard]]
double easeInOutCubic(double t) noexcept;


//------------------------------------------------------------------------------
// Quartic
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInQuart(double t) noexcept;

[[nodiscard]]
double easeOutQuart(double t) noexcept;

[[nodiscard]]
double easeInOutQuart(double t) noexcept;


//------------------------------------------------------------------------------
// Quintic
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInQuint(double t) noexcept;

[[nodiscard]]
double easeOutQuint(double t) noexcept;

[[nodiscard]]
double easeInOutQuint(double t) noexcept;


//------------------------------------------------------------------------------
// Exponential
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInExpo(double t) noexcept;

[[nodiscard]]
double easeOutExpo(double t) noexcept;

[[nodiscard]]
double easeInOutExpo(double t) noexcept;


//------------------------------------------------------------------------------
// Circular
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInCirc(double t) noexcept;

[[nodiscard]]
double easeOutCirc(double t) noexcept;

[[nodiscard]]
double easeInOutCirc(double t) noexcept;


//------------------------------------------------------------------------------
// Back
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInBack(double t) noexcept;

[[nodiscard]]
double easeOutBack(double t) noexcept;

[[nodiscard]]
double easeInOutBack(double t) noexcept;


//------------------------------------------------------------------------------
// Elastic
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInElastic(double t) noexcept;

[[nodiscard]]
double easeOutElastic(double t) noexcept;

[[nodiscard]]
double easeInOutElastic(double t) noexcept;


//------------------------------------------------------------------------------
// Bounce
//------------------------------------------------------------------------------

[[nodiscard]]
double easeInBounce(double t) noexcept;

[[nodiscard]]
double easeOutBounce(double t) noexcept;

[[nodiscard]]
double easeInOutBounce(double t) noexcept;

}