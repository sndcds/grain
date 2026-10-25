#pragma once

#include <cstdint>

namespace Grain {

enum class EaseMode : int32_t {
    InSine = 0,
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

    First = InSine,
    Last = InOutBounce
};

} // namespace Grain