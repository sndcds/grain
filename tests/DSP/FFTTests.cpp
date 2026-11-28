#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/DSP/FFT/FFT.hpp>

#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <memory>
#include <numbers>
#include <vector>

namespace Grain::DSP::Tests {

using Catch::Matchers::WithinAbs;

constexpr float kTolerance = 1.0e-4f;


TEST_CASE("FFT creation", "[DSP][FFT]")
{
    SECTION("creates an automatic backend")
    {
        std::unique_ptr<FFT> fft;

        REQUIRE(
            FFT::create(FFTConfiguration{8}, fft)
            == Result::Success
        );

        REQUIRE(fft != nullptr);
        REQUIRE(fft->size() == 8);
        REQUIRE(fft->binCount() == 5);

#if defined(__APPLE__)
        REQUIRE(fft->backend() == FFTBackendType::Accelerate);
#else
        REQUIRE(fft->backend() == FFTBackendType::Portable);
#endif
    }

    SECTION("creates an arbitrary transform size")
    {
        std::unique_ptr<FFT> fft;

        REQUIRE(
            FFT::create(FFTConfiguration{7}, fft)
            == Result::Success
        );

        REQUIRE(fft != nullptr);
        REQUIRE(fft->size() == 7);
        REQUIRE(fft->binCount() == 4);

        // The current Accelerate implementation supports even sizes only.
        // Automatic selection should therefore fall back to Portable.
        REQUIRE(fft->backend() == FFTBackendType::Portable);
    }

    SECTION("rejects a zero transform size")
    {
        std::unique_ptr<FFT> fft;

        REQUIRE(
            FFT::create(FFTConfiguration{0}, fft)
            == Result::InvalidArgument
        );

        REQUIRE(fft == nullptr);
    }

    SECTION("rejects unavailable optimized backends")
    {
        std::unique_ptr<FFT> fft;

#if !defined(__APPLE__)
        REQUIRE(
            FFT::create(
                FFTConfiguration{8, FFTBackendType::Accelerate},
                fft
            )
            == Result::Unsupported
        );

        REQUIRE(fft == nullptr);
#endif

        REQUIRE(
            FFT::create(
                FFTConfiguration{8, FFTBackendType::FFTW},
                fft
            )
            == Result::Unsupported
        );

        REQUIRE(fft == nullptr);
    }
}


TEST_CASE("FFT forward transform", "[DSP][FFT]") {
    constexpr std::size_t size = 8;

    std::unique_ptr<FFT> fft;

    REQUIRE(
        FFT::create(FFTConfiguration{size}, fft)
        == Result::Success
    );

    std::array<float, size> input{};
    std::array<std::complex<float>, size / 2 + 1> output{};

    SECTION("an impulse has a flat spectrum") {
        input[0] = 1.0f;

        REQUIRE(fft->forward(input, output) == Result::Success);

        for (const std::complex<float>& bin : output) {
            REQUIRE_THAT(bin.real(), WithinAbs(1.0f, kTolerance));
            REQUIRE_THAT(bin.imag(), WithinAbs(0.0f, kTolerance));
        }
    }

    SECTION("a constant signal produces only a DC component") {
        input.fill(1.0f);

        REQUIRE(fft->forward(input, output) == Result::Success);

        REQUIRE_THAT(
            output[0].real(),
            WithinAbs(static_cast<float>(size), kTolerance)
        );

        REQUIRE_THAT(output[0].imag(), WithinAbs(0.0f, kTolerance));

        for (std::size_t i = 1; i < output.size(); ++i) {
            REQUIRE_THAT(output[i].real(), WithinAbs(0.0f, kTolerance));
            REQUIRE_THAT(output[i].imag(), WithinAbs(0.0f, kTolerance));
        }
    }

    SECTION("a bin-centered sine produces the expected imaginary component") {
        constexpr std::size_t target_bin = 2;

        for (std::size_t n = 0; n < size; ++n) {
            const double phase =
                2.0 * std::numbers::pi_v<double>
                * static_cast<double>(target_bin)
                * static_cast<double>(n)
                / static_cast<double>(size);

            input[n] = static_cast<float>(std::sin(phase));
        }

        REQUIRE(fft->forward(input, output) == Result::Success);

        REQUIRE_THAT(
            output[target_bin].real(),
            WithinAbs(0.0f, kTolerance)
        );

        REQUIRE_THAT(
            output[target_bin].imag(),
            WithinAbs(-static_cast<float>(size) / 2.0f, kTolerance)
        );
    }

    SECTION("an alternating signal produces a Nyquist component") {
        for (std::size_t n = 0; n < size; ++n) {
            input[n] = n % 2 == 0 ? 1.0f : -1.0f;
        }

        REQUIRE(fft->forward(input, output) == Result::Success);

        REQUIRE_THAT(
            output.back().real(),
            WithinAbs(static_cast<float>(size), kTolerance)
        );

        REQUIRE_THAT(output.back().imag(), WithinAbs(0.0f, kTolerance));
    }

    SECTION("rejects incorrect input and output sizes") {
        std::array<float, size - 1> short_input{};
        std::array<std::complex<float>, size / 2> short_output{};

        REQUIRE(
            fft->forward(short_input, output)
            == Result::InvalidArgument
        );

        REQUIRE(
            fft->forward(input, short_output)
            == Result::InvalidArgument
        );
    }
}


TEST_CASE("FFT inverse transform", "[DSP][FFT]") {
    SECTION("round trip with an even transform size") {
        constexpr std::size_t size = 8;

        std::unique_ptr<FFT> fft;

        REQUIRE(
            FFT::create(FFTConfiguration{size}, fft)
            == Result::Success
        );

        std::array<float, size> input = {
            0.25f, -0.5f, 0.75f, 1.0f,
            -0.25f, 0.5f, -0.75f, 0.125f
        };

        std::array<std::complex<float>, size / 2 + 1> spectrum{};
        std::array<float, size> output{};

        REQUIRE(fft->forward(input, spectrum) == Result::Success);
        REQUIRE(fft->inverse(spectrum, output) == Result::Success);

        for (std::size_t i = 0; i < size; ++i) {
            REQUIRE_THAT(output[i], WithinAbs(input[i], kTolerance));
        }
    }

    SECTION("round trip with an odd transform size") {
        constexpr std::size_t size = 7;

        std::unique_ptr<FFT> fft;

        REQUIRE(
            FFT::create(FFTConfiguration{size}, fft)
            == Result::Success
        );

        std::array<float, size> input = {
            0.25f, -0.5f, 0.75f, 1.0f,
            -0.25f, 0.5f, -0.75f
        };

        std::array<std::complex<float>, size / 2 + 1> spectrum{};
        std::array<float, size> output{};

        REQUIRE(fft->forward(input, spectrum) == Result::Success);
        REQUIRE(fft->inverse(spectrum, output) == Result::Success);

        for (std::size_t i = 0; i < size; ++i) {
            REQUIRE_THAT(output[i], WithinAbs(input[i], kTolerance));
        }
    }

    SECTION("round trip with a single sample") {
        std::unique_ptr<FFT> fft;

        REQUIRE(
            FFT::create(FFTConfiguration{1}, fft)
            == Result::Success
        );

        const std::array<float, 1> input = { 0.75f };
        std::array<std::complex<float>, 1> spectrum{};
        std::array<float, 1> output{};

        REQUIRE(fft->forward(input, spectrum) == Result::Success);
        REQUIRE(fft->inverse(spectrum, output) == Result::Success);

        REQUIRE_THAT(output[0], WithinAbs(input[0], kTolerance));
    }

    SECTION("rejects incorrect input and output sizes") {
        constexpr std::size_t size = 8;

        std::unique_ptr<FFT> fft;

        REQUIRE(
            FFT::create(FFTConfiguration{size}, fft)
            == Result::Success
        );

        std::array<std::complex<float>, size / 2> short_input{};
        std::array<float, size - 1> short_output{};

        std::array<std::complex<float>, size / 2 + 1> input{};
        std::array<float, size> output{};

        REQUIRE(
            fft->inverse(short_input, output)
            == Result::InvalidArgument
        );

        REQUIRE(
            fft->inverse(input, short_output)
            == Result::InvalidArgument
        );
    }
}

TEST_CASE("FFT Accelerate matches portable backend", "[DSP][FFT]") {
#if defined(__APPLE__)
    constexpr std::size_t size = 16;

    std::unique_ptr<FFT> portable;
    std::unique_ptr<FFT> accelerate;

    REQUIRE(
        FFT::create(
            FFTConfiguration{size, FFTBackendType::Portable},
            portable
        ) == Result::Success
    );

    REQUIRE(
        FFT::create(
            FFTConfiguration{size, FFTBackendType::Accelerate},
            accelerate
        ) == Result::Success
    );

    std::array<float, size> input{};
    std::array<std::complex<float>, size / 2 + 1> portable_output{};
    std::array<std::complex<float>, size / 2 + 1> accelerate_output{};

    for (std::size_t i = 0; i < size; ++i) {
        input[i] = static_cast<float>(i % 5) / 5.0f;
    }

    REQUIRE(
        portable->forward(input, portable_output) == Result::Success
    );

    REQUIRE(
        accelerate->forward(input, accelerate_output) == Result::Success
    );

    for (std::size_t i = 0; i < portable_output.size(); ++i) {
        REQUIRE_THAT(
            accelerate_output[i].real(),
            WithinAbs(portable_output[i].real(), kTolerance)
        );

        REQUIRE_THAT(
            accelerate_output[i].imag(),
            WithinAbs(portable_output[i].imag(), kTolerance)
        );
    }
#endif
}

} // namespace Grain::DSP::Tests