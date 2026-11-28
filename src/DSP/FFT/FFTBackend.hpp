#pragma once

#include <grain/Core/Result.hpp>
#include <grain/DSP/FFT/FFT.hpp>

namespace Grain {

class FFTBackend {
public:
    virtual ~FFTBackend() = default;

    [[nodiscard]]
    virtual auto size() const noexcept -> std::size_t = 0;

    [[nodiscard]]
    virtual auto binCount() const noexcept -> std::size_t = 0;

    [[nodiscard]]
    virtual auto type() const noexcept -> FFTBackendType = 0;

    virtual auto forward(
        std::span<const float> input,
        std::span<std::complex<float>> output
    ) noexcept -> Result = 0;

    virtual auto inverse(
        std::span<const std::complex<float>> input,
        std::span<float> output
    ) noexcept -> Result = 0;
};

} // namespace Grain