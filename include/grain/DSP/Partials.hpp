
#pragma once

#include <grain/Core/Result.hpp>

#include <complex>
#include <cstddef>
#include <memory>
#include <span>
#include <vector>

namespace Grain {

/**
 * Stores the non-redundant complex spectrum of a real-valued signal.
 *
 * The bin layout matches FFT:
 * - Bin 0 is DC.
 * - Bins 1..N/2 contain positive frequencies.
 * - For even FFT sizes, the final bin is Nyquist.
 * - For odd FFT sizes, the final bin is below Nyquist.
 *
 * The number of stored bins is fftSize() / 2 + 1.
 *
 * Magnitudes are absolute values of complex bins.
 * Phases are in radians.
 *
 * Partials does not perform FFT operations. It is a platform-independent
 * container that can exchange its spectrum directly with Grain::FFT.
 */
class Partials {
public:
    [[nodiscard]]
    static auto create(
        std::size_t fftSize,
        std::unique_ptr<Partials>& output
    ) noexcept -> Result;

    ~Partials() = default;

    Partials(const Partials&) = default;
    auto operator=(const Partials&) -> Partials& = default;

    Partials(Partials&&) noexcept = default;
    auto operator=(Partials&&) noexcept -> Partials& = default;

    [[nodiscard]]
    auto fftSize() const noexcept -> std::size_t;

    [[nodiscard]]
    auto binCount() const noexcept -> std::size_t;

    [[nodiscard]]
    auto hasNyquistBin() const noexcept -> bool;

    [[nodiscard]]
    auto dc() const noexcept -> float;

    [[nodiscard]]
    auto nyquist() const noexcept -> float;

    [[nodiscard]]
    auto magnitude(std::size_t binIndex) const noexcept -> float;

    [[nodiscard]]
    auto phase(std::size_t binIndex) const noexcept -> float;

    [[nodiscard]]
    auto spectrum() const noexcept -> std::span<const std::complex<float>>;

    [[nodiscard]]
    auto mutSpectrum() noexcept -> std::span<std::complex<float>>;

    auto setBin(
        std::size_t binIndex,
        std::complex<float> value
    ) noexcept -> Result;

    auto setMagnitude(
        std::size_t binIndex,
        float magnitude
    ) noexcept -> Result;

    auto setPhase(
        std::size_t binIndex,
        float phase
    ) noexcept -> Result;

    auto setFromSpectrum(
        std::span<const std::complex<float>> spectrum
    ) noexcept -> Result;

    auto copyToSpectrum(
        std::span<std::complex<float>> spectrum
    ) const noexcept -> Result;

    void clear() noexcept;

private:
    explicit Partials(std::size_t fftSize);

    std::size_t fft_size_ = 0;
    std::vector<std::complex<float>> bins_;
};

} // namespace Grain
