
#include "PortableFFT.hpp"

#include <cmath>
#include <numbers>

namespace Grain {

namespace {

auto isPowerOfTwo(std::size_t value) noexcept -> bool {
    return value != 0 && (value & (value - 1)) == 0;
}

} // namespace


PortableFFT::PortableFFT(std::size_t size)
    : size_(size),
      bin_count_(size / 2 + 1),
      use_radix2_(isPowerOfTwo(size)),
      twiddles_(size),
      bit_reversal_(use_radix2_ ? size : 0),
      workspace_(use_radix2_ ? size : 0) {

    constexpr double tau = 2.0 * std::numbers::pi_v<double>;

    //--------------------------------------------------------------------------
    //  Precompute twiddle factors.
    //--------------------------------------------------------------------------

    for (std::size_t i = 0; i < size_; ++i) {
        const double angle =
            tau * static_cast<double>(i) / static_cast<double>(size_);

        twiddles_[i] = std::complex<double>(
            std::cos(angle),
            -std::sin(angle)
        );
    }

    //--------------------------------------------------------------------------
    //  Precompute bit-reversal permutation.
    //--------------------------------------------------------------------------

    if (use_radix2_) {
        std::size_t bit_count = 0;

        for (std::size_t value = size_; value > 1; value >>= 1) {
            ++bit_count;
        }

        for (std::size_t i = 0; i < size_; ++i) {
            std::size_t value = i;
            std::size_t reversed = 0;

            for (std::size_t bit = 0; bit < bit_count; ++bit) {
                reversed = (reversed << 1) | (value & 1);
                value >>= 1;
            }

            bit_reversal_[i] = reversed;
        }
    }
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


//-------------------------------------------------------------------------------
//  Forward transform.
//-------------------------------------------------------------------------------

auto PortableFFT::forward(
    std::span<const float> input,
    std::span<std::complex<float>> output
) noexcept -> Result {
    if (input.size() != size_ || output.size() != bin_count_) {
        return Result::InvalidArgument;
    }

    if (use_radix2_) {
        return forwardRadix2(input, output);
    }

    return forwardDirect(input, output);
}


auto PortableFFT::forwardRadix2(
    std::span<const float> input,
    std::span<std::complex<float>> output
) noexcept -> Result {

    //--------------------------------------------------------------------------
    //  Bit-reversed input.
    //--------------------------------------------------------------------------

    for (std::size_t i = 0; i < size_; ++i) {
        workspace_[i] = std::complex<double>(
            static_cast<double>(input[bit_reversal_[i]]),
            0.0
        );
    }

    //--------------------------------------------------------------------------
    //  Iterative Cooley-Tukey butterflies.
    //--------------------------------------------------------------------------

    for (std::size_t length = 2; length <= size_;) {
        const std::size_t half_length = length / 2;
        const std::size_t twiddle_stride = size_ / length;

        for (std::size_t start = 0; start < size_; start += length) {
            for (std::size_t j = 0; j < half_length; ++j) {
                const std::complex<double>& twiddle =
                    twiddles_[j * twiddle_stride];

                const std::complex<double> even =
                    workspace_[start + j];

                const std::complex<double> odd =
                    workspace_[start + j + half_length] * twiddle;

                workspace_[start + j] = even + odd;
                workspace_[start + j + half_length] = even - odd;
            }
        }

        if (length == size_) {
            break;
        }

        length *= 2;
    }

    //--------------------------------------------------------------------------
    //  Return the non-redundant real spectrum.
    //--------------------------------------------------------------------------

    for (std::size_t k = 0; k < bin_count_; ++k) {
        output[k] = std::complex<float>(
            static_cast<float>(workspace_[k].real()),
            static_cast<float>(workspace_[k].imag())
        );
    }

    return Result::Success;
}


auto PortableFFT::forwardDirect(
    std::span<const float> input,
    std::span<std::complex<float>> output
) noexcept -> Result {

    for (std::size_t k = 0; k < bin_count_; ++k) {
        double real = 0.0;
        double imaginary = 0.0;

        for (std::size_t n = 0; n < size_; ++n) {
            const std::size_t twiddle_index = (k * n) % size_;
            const std::complex<double>& twiddle = twiddles_[twiddle_index];

            const double sample = static_cast<double>(input[n]);

            real += sample * twiddle.real();
            imaginary += sample * twiddle.imag();
        }

        output[k] = std::complex<float>(
            static_cast<float>(real),
            static_cast<float>(imaginary)
        );
    }

    return Result::Success;
}


//-------------------------------------------------------------------------------
//  Inverse transform.
//-------------------------------------------------------------------------------

auto PortableFFT::inverse(
    std::span<const std::complex<float>> input,
    std::span<float> output
) noexcept -> Result {
    if (input.size() != bin_count_ || output.size() != size_) {
        return Result::InvalidArgument;
    }

    if (use_radix2_) {
        return inverseRadix2(input, output);
    }

    return inverseDirect(input, output);
}


auto PortableFFT::inverseRadix2(
    std::span<const std::complex<float>> input,
    std::span<float> output
) noexcept -> Result {

    //--------------------------------------------------------------------------
    //  Reconstruct the full Hermitian spectrum in bit-reversed order.
    //--------------------------------------------------------------------------

    const std::size_t nyquist_bin = size_ / 2;

    for (std::size_t i = 0; i < size_; ++i) {
        const std::size_t source_index = bit_reversal_[i];

        if (source_index == 0 || source_index == nyquist_bin) {
            workspace_[i] = std::complex<double>(
                static_cast<double>(input[source_index].real()),
                0.0
            );
        }
        else if (source_index < bin_count_) {
            workspace_[i] = std::complex<double>(
                static_cast<double>(input[source_index].real()),
                static_cast<double>(input[source_index].imag())
            );
        }
        else {
            const std::size_t mirrored_index = size_ - source_index;

            workspace_[i] = std::complex<double>(
                static_cast<double>(input[mirrored_index].real()),
                -static_cast<double>(input[mirrored_index].imag())
            );
        }
    }

    //--------------------------------------------------------------------------
    //  Inverse Cooley-Tukey butterflies.
    //--------------------------------------------------------------------------

    for (std::size_t length = 2; length <= size_;) {
        const std::size_t half_length = length / 2;
        const std::size_t twiddle_stride = size_ / length;

        for (std::size_t start = 0; start < size_; start += length) {
            for (std::size_t j = 0; j < half_length; ++j) {
                const std::complex<double>& forward_twiddle =
                    twiddles_[j * twiddle_stride];

                const std::complex<double> inverse_twiddle(
                    forward_twiddle.real(),
                    -forward_twiddle.imag()
                );

                const std::complex<double> even =
                    workspace_[start + j];

                const std::complex<double> odd =
                    workspace_[start + j + half_length] * inverse_twiddle;

                workspace_[start + j] = even + odd;
                workspace_[start + j + half_length] = even - odd;
            }
        }

        if (length == size_) {
            break;
        }

        length *= 2;
    }

    //--------------------------------------------------------------------------
    //  Normalize and return real samples.
    //--------------------------------------------------------------------------

    const double normalization = 1.0 / static_cast<double>(size_);

    for (std::size_t i = 0; i < size_; ++i) {
        output[i] = static_cast<float>(
            workspace_[i].real() * normalization
        );
    }

    return Result::Success;
}


auto PortableFFT::inverseDirect(
    std::span<const std::complex<float>> input,
    std::span<float> output
) noexcept -> Result {

    const bool has_nyquist = size_ > 1 && size_ % 2 == 0;
    const std::size_t last_pair_bin =
        has_nyquist ? bin_count_ - 1 : bin_count_;

    for (std::size_t n = 0; n < size_; ++n) {
        double value = static_cast<double>(input[0].real());

        for (std::size_t k = 1; k < last_pair_bin; ++k) {
            const std::size_t twiddle_index = (k * n) % size_;
            const std::complex<double>& twiddle = twiddles_[twiddle_index];

            const double real = static_cast<double>(input[k].real());
            const double imaginary = static_cast<double>(input[k].imag());

            value += 2.0 * (
                real * twiddle.real()
                + imaginary * twiddle.imag()
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
