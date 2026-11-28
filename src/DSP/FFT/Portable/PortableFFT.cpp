#include "PortableFFT.hpp"

#include <cmath>
#include <numbers>

namespace Grain {

PortableFFT::PortableFFT(std::size_t size) noexcept
    : size_(size),
      bin_count_(size / 2 + 1) {
}


auto PortableFFT::size() const noexcept -> std::size_t {
    return size_;
}


auto PortableFFT::binCount() const noexcept -> std::size_t {
    return bin_count_;
}


auto PortableFFT::type() const noexcept -> FFTBackendType {
    return FFTBackendType::Portable;
}


auto PortableFFT::forward(
    std::span<const float> input,
    std::span<std::complex<float>> output
) noexcept -> Result {
    if (input.size() != size_ || output.size() != bin_count_) {
        return Result::InvalidArgument;
    }

    constexpr double tau = 2.0 * std::numbers::pi_v<double>;

    for (std::size_t k = 0; k < bin_count_; ++k) {
        double real = 0.0;
        double imaginary = 0.0;

        for (std::size_t n = 0; n < size_; ++n) {
            const double angle =
                tau * static_cast<double>(k)
                * static_cast<double>(n)
                / static_cast<double>(size_);

            const double sample = static_cast<double>(input[n]);

            real += sample * std::cos(angle);
            imaginary -= sample * std::sin(angle);
        }

        output[k] = std::complex<float>(
            static_cast<float>(real),
            static_cast<float>(imaginary)
        );
    }

    return Result::Success;
}


auto PortableFFT::inverse(
    std::span<const std::complex<float>> input,
    std::span<float> output
) noexcept -> Result {
    if (input.size() != bin_count_ || output.size() != size_) {
        return Result::InvalidArgument;
    }

    constexpr double tau = 2.0 * std::numbers::pi_v<double>;

    const bool has_nyquist = size_ > 1 && size_ % 2 == 0;
    const std::size_t last_pair_bin =
        has_nyquist ? bin_count_ - 1 : bin_count_;

    for (std::size_t n = 0; n < size_; ++n) {
        double value = static_cast<double>(input[0].real());

        for (std::size_t k = 1; k < last_pair_bin; ++k) {
            const double angle =
                tau * static_cast<double>(k)
                * static_cast<double>(n)
                / static_cast<double>(size_);

            const double real = static_cast<double>(input[k].real());
            const double imaginary = static_cast<double>(input[k].imag());

            value += 2.0 * (
                real * std::cos(angle)
                - imaginary * std::sin(angle)
            );
        }

        if (has_nyquist) {
            const double nyquist =
                static_cast<double>(input[bin_count_ - 1].real());

            value += (n % 2 == 0) ? nyquist : -nyquist;
        }

        output[n] = static_cast<float>(
            value / static_cast<double>(size_)
        );
    }

    return Result::Success;
}

} // namespace Grain