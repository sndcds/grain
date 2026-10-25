#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Grain::Easing {

enum class Type : uint8_t {
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
    InOutBounce,

    Count
};

inline constexpr std::size_t kTypeCount = static_cast<std::size_t>(Type::Count);

[[nodiscard]] auto evaluate(Type type, double t) -> double;

[[nodiscard]] auto typeName(Type type) -> std::string_view;


//------------------------------------------------------------------------------
// Sine
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInSine(double t) -> double;
[[nodiscard]] auto easeOutSine(double t) -> double;
[[nodiscard]] auto easeInOutSine(double t) -> double;


//------------------------------------------------------------------------------
// Quadratic
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInQuad(double t) -> double;
[[nodiscard]] auto easeOutQuad(double t) -> double;
[[nodiscard]] auto easeInOutQuad(double t) -> double;


//------------------------------------------------------------------------------
// Cubic
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInCubic(double t) -> double;
[[nodiscard]] auto easeOutCubic(double t) -> double;
[[nodiscard]] auto easeInOutCubic(double t) -> double;


//------------------------------------------------------------------------------
// Quartic
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInQuart(double t) -> double;
[[nodiscard]] auto easeOutQuart(double t) -> double;
[[nodiscard]] auto easeInOutQuart(double t) -> double;


//------------------------------------------------------------------------------
// Quintic
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInQuint(double t) -> double;
[[nodiscard]] auto easeOutQuint(double t) -> double;
[[nodiscard]] auto easeInOutQuint(double t) -> double;


//------------------------------------------------------------------------------
// Exponential
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInExpo(double t) -> double;
[[nodiscard]] auto easeOutExpo(double t) -> double;
[[nodiscard]] auto easeInOutExpo(double t) -> double;


//------------------------------------------------------------------------------
// Circular
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInCirc(double t) -> double;
[[nodiscard]] auto easeOutCirc(double t) -> double;
[[nodiscard]] auto easeInOutCirc(double t) -> double;


//------------------------------------------------------------------------------
// Back
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInBack(double t) -> double;
[[nodiscard]] auto easeOutBack(double t) -> double;
[[nodiscard]] auto easeInOutBack(double t) -> double;


//------------------------------------------------------------------------------
// Elastic
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInElastic(double t) -> double;
[[nodiscard]] auto easeOutElastic(double t) -> double;
[[nodiscard]] auto easeInOutElastic(double t) -> double;


//------------------------------------------------------------------------------
// Bounce
//------------------------------------------------------------------------------

[[nodiscard]] auto easeInBounce(double t) -> double;
[[nodiscard]] auto easeOutBounce(double t) -> double;
[[nodiscard]] auto easeInOutBounce(double t) -> double;

}