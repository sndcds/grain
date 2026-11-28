
#include <grain/DSP/Partials.hpp>

#include <algorithm>
#include <cmath>
#include <new>
#include <stdexcept>

namespace Grain {

Partials::Partials(std::size_t fftSize)
    : fft_size_(fftSize),
      bins_(fftSize / 2 + 1, std::complex<float>{0.0f, 0.0f})
{
}


auto Partials::create(
    std::size_t fftSize,
    std::unique_ptr<Partials>& output
) noexcept -> Result
{
    output.reset();

    if (fftSize == 0) {
        return Result::InvalidArgument;
    }

    try {
        output.reset(new Partials(fftSize));
    }
    catch (const std::bad_alloc&) {
        return Result::OutOfMemory;
    }
    catch (const std::length_error&) {
        return Result::InvalidArgument;
    }
    catch (...) {
        return Result::Error;
    }

    return Result::Success;
}


auto Partials::fftSize() const noexcept -> std::size_t {
    return fft_size_;
}


auto Partials::binCount() const noexcept -> std::size_t {
    return bins_.size();
}


auto Partials::hasNyquistBin() const noexcept -> bool {
    return fft_size_ % 2 == 0;
}


auto Partials::dc() const noexcept -> float {
    return bins_.empty() ? 0.0f : bins_[0].real();
}


auto Partials::nyquist() const noexcept -> float {
    if (!hasNyquistBin()) {
        return 0.0f;
    }

    return bins_.back().real();
}


auto Partials::magnitude(std::size_t binIndex) const noexcept -> float {
    if (binIndex >= bins_.size()) {
        return 0.0f;
    }

    return std::abs(bins_[binIndex]);
}


auto Partials::phase(std::size_t binIndex) const noexcept -> float {
    if (binIndex >= bins_.size()) {
        return 0.0f;
    }

    return std::arg(bins_[binIndex]);
}


auto Partials::spectrum() const noexcept
    -> std::span<const std::complex<float>>
{
    return bins_;
}


auto Partials::mutSpectrum() noexcept
    -> std::span<std::complex<float>>
{
    return bins_;
}


auto Partials::setBin(
    std::size_t binIndex,
    std::complex<float> value
) noexcept -> Result
{
    if (binIndex >= bins_.size()) {
        return Result::InvalidArgument;
    }

    bins_[binIndex] = value;
    return Result::Success;
}


auto Partials::setMagnitude(
    std::size_t binIndex,
    float magnitudeValue
) noexcept -> Result
{
    if (binIndex >= bins_.size() || magnitudeValue < 0.0f) {
        return Result::InvalidArgument;
    }

    const std::complex<float> current = bins_[binIndex];
    const float currentMagnitude = std::abs(current);

    if (currentMagnitude > 0.0f) {
        bins_[binIndex] = current * (magnitudeValue / currentMagnitude);
    }
    else {
        bins_[binIndex] = {magnitudeValue, 0.0f};
    }

    return Result::Success;
}


auto Partials::setPhase(
    std::size_t binIndex,
    float phaseValue
) noexcept -> Result
{
    if (binIndex >= bins_.size()) {
        return Result::InvalidArgument;
    }

    const float currentMagnitude = std::abs(bins_[binIndex]);

    bins_[binIndex] = std::polar(currentMagnitude, phaseValue);

    return Result::Success;
}


auto Partials::setFromSpectrum(
    std::span<const std::complex<float>> spectrumData
) noexcept -> Result
{
    if (spectrumData.size() != bins_.size()) {
        return Result::InvalidArgument;
    }

    std::copy(spectrumData.begin(), spectrumData.end(), bins_.begin());

    return Result::Success;
}


auto Partials::copyToSpectrum(
    std::span<std::complex<float>> spectrumData
) const noexcept -> Result
{
    if (spectrumData.size() != bins_.size()) {
        return Result::InvalidArgument;
    }

    std::copy(bins_.begin(), bins_.end(), spectrumData.begin());

    return Result::Success;
}


void Partials::clear() noexcept {
    std::fill(
        bins_.begin(),
        bins_.end(),
        std::complex<float>{0.0f, 0.0f}
    );
}

} // namespace Grain
