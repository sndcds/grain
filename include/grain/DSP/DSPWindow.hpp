#pragma once

#include <grain/Core/Result.hpp>
#include <cstdint>

namespace Grain {

class DSPWindow {

public:
    enum class WindowType : int8_t {
        Kaiser, First = 0,
        Sinc,       // Lanczos
        Sine,       // Hanning if Beta = 2
        Hanning,
        Hamming,
        Blackman,
        FlatTop,
        BlackmanHarris,
        BlackmanNuttall,
        Nuttall,
        KaiserBessel,
        Trapezoid,
        Gauss,
        Last
    };

    static auto hanningWindow(float t) -> float;
    static auto hanningLeft(float t) -> float;
    static auto hanningWindowSymmetric(int32_t window_size, float* out_data) -> bool;
    static auto hanningWindowPeriodic(int32_t window_size, float* out_data) -> bool;

    static auto window(
        int32_t window_size,
        WindowType window_type,
        float alpha,
        float beta,
        bool unity_gain,
        float* out_data
    ) -> Result;
};


} // End of namespace Grain
