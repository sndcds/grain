
#pragma once

#include <grain/Core/Result.hpp>
#include <grain/DSP/FFT/FFT.hpp>

#include <cstddef>
#include <span>
#include <vector>

namespace Grain {

struct ConvolutionConfiguration {
    /// Number of samples processed per partition.
    std::size_t partitionSize = 1024;

    /// FFT backend used for the convolution.
    FFTBackendType backend = FFTBackendType::Automatic;
};


class Convolution {
public:
    /**
     * Computes the full linear convolution of two real-valued signals.
     *
     * The result length is:
     * input.size() + impulseResponse.size() - 1
     *
     * The output vector is replaced with the result.
     *
     * Both input signals must contain at least one sample.
     */
    [[nodiscard]]
    static auto process(
        std::span<const float> input,
        std::span<const float> impulseResponse,
        std::vector<float>& output,
        const ConvolutionConfiguration& configuration = {}
    ) noexcept -> Result;
};

} // namespace Grain
