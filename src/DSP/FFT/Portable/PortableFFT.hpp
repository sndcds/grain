
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
    std::size_t size_ = 0;
    std::size_t bin_count_ = 0;

    // e^(-i * 2 * pi * index / size), calculated once at construction.
    std::vector<std::complex<double>> twiddles_;
};

} // namespace Grain
