#pragma once

#include <grain/Core/Result.hpp>

#include <complex>
#include <cstddef>
#include <memory>
#include <span>

namespace Grain {

enum class FFTBackendType {
    Automatic,
    Portable,
    Accelerate,
    FFTW
};


struct FFTConfiguration {
    std::size_t size = 1024;
    FFTBackendType backend = FFTBackendType::Automatic;
};


class FFT {
public:
    [[nodiscard]]
    static auto create(
        const FFTConfiguration& configuration,
        std::unique_ptr<FFT>& output
    ) noexcept -> Result;

    ~FFT();

    FFT(const FFT&) = delete;
    auto operator=(const FFT&) -> FFT& = delete;

    FFT(FFT&&) noexcept;
    auto operator=(FFT&&) noexcept -> FFT&;

    [[nodiscard]]
    auto size() const noexcept -> std::size_t;

    [[nodiscard]]
    auto binCount() const noexcept -> std::size_t;

    [[nodiscard]]
    auto backend() const noexcept -> FFTBackendType;

    auto forward(
        std::span<const float> input,
        std::span<std::complex<float>> output
    ) noexcept -> Result;

    auto inverse(
        std::span<const std::complex<float>> input,
        std::span<float> output
    ) noexcept -> Result;

private:
    class Impl;

    explicit FFT(std::unique_ptr<Impl> impl) noexcept;

    std::unique_ptr<Impl> impl_;
};

} // namespace Grain