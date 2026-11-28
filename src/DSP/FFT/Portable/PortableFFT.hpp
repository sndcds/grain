
#pragma once

#include <grain/DSP/FFT/FFT.hpp>
#include "../FFTBackend.hpp"

#include <complex>
#include <vector>

namespace Grain {

class PortableFFT final : public FFTBackend {
public:
    explicit PortableFFT(std::size_t size);

    [[nodiscard]]
    auto size() const noexcept -> std::size_t override;

    [[nodiscard]]
    auto binCount() const noexcept -> std::size_t override;

    [[nodiscard]]
    auto type() const noexcept -> FFTBackendType override;

    auto forward(
        std::span<const float> input,
        std::span<std::complex<float>> output
    ) noexcept -> Result override;

    auto inverse(
        std::span<const std::complex<float>> input,
        std::span<float> output
    ) noexcept -> Result override;

private:
    auto forwardDirect(
        std::span<const float> input,
        std::span<std::complex<float>> output
    ) noexcept -> Result;

    auto inverseDirect(
        std::span<const std::complex<float>> input,
        std::span<float> output
    ) noexcept -> Result;

    auto forwardRadix2(
        std::span<const float> input,
        std::span<std::complex<float>> output
    ) noexcept -> Result;

    auto inverseRadix2(
        std::span<const std::complex<float>> input,
        std::span<float> output
    ) noexcept -> Result;

    std::size_t size_ = 0;
    std::size_t bin_count_ = 0;

    bool use_radix2_ = false;

    // e^(-i * 2 * pi * index / size), calculated once at construction.
    std::vector<std::complex<double>> twiddles_;

    // Input permutation used by the iterative radix-2 algorithm.
    std::vector<std::size_t> bit_reversal_;

    // Reusable working buffer. No allocation occurs during transforms.
    std::vector<std::complex<double>> workspace_;
};

} // namespace Grain
