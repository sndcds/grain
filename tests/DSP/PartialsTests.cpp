
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/DSP/FFT/FFT.hpp>
#include <grain/DSP/Partials.hpp>

#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <memory>
#include <numbers>

namespace Grain::DSP::Tests {

using Catch::Matchers::WithinAbs;

constexpr float kTolerance = 1.0e-5f;


TEST_CASE("Partials creation", "[DSP][Partials]")
{
    SECTION("creates an even-sized spectrum")
    {
        std::unique_ptr<Partials> partials;

        REQUIRE(Partials::create(8, partials) == Result::Success);

        REQUIRE(partials != nullptr);
        REQUIRE(partials->fftSize() == 8);
        REQUIRE(partials->binCount() == 5);
        REQUIRE(partials->hasNyquistBin());
    }

    SECTION("creates an odd-sized spectrum")
    {
        std::unique_ptr<Partials> partials;

        REQUIRE(Partials::create(7, partials) == Result::Success);

        REQUIRE(partials != nullptr);
        REQUIRE(partials->fftSize() == 7);
        REQUIRE(partials->binCount() == 4);
        REQUIRE_FALSE(partials->hasNyquistBin());
    }

    SECTION("rejects a zero transform size")
    {
        std::unique_ptr<Partials> partials;

        REQUIRE(Partials::create(0, partials) == Result::InvalidArgument);
        REQUIRE(partials == nullptr);
    }
}


TEST_CASE("Partials spectrum access", "[DSP][Partials]")
{
    std::unique_ptr<Partials> partials;

    REQUIRE(Partials::create(8, partials) == Result::Success);

    SECTION("starts with all bins cleared")
    {
        REQUIRE(partials->dc() == 0.0f);

        for (const std::complex<float>& bin : partials->spectrum()) {
            REQUIRE(bin.real() == 0.0f);
            REQUIRE(bin.imag() == 0.0f);
        }
    }

    SECTION("sets and reads a complex bin")
    {
        REQUIRE(partials->setBin(2, {3.0f, 4.0f}) == Result::Success);

        REQUIRE_THAT(partials->magnitude(2), WithinAbs(5.0f, kTolerance));
        REQUIRE_THAT(
            partials->phase(2),
            WithinAbs(std::atan2(4.0f, 3.0f), kTolerance)
        );
    }

    SECTION("sets DC")
    {
        REQUIRE(partials->setBin(0, {2.5f, 0.0f}) == Result::Success);

        REQUIRE_THAT(partials->dc(), WithinAbs(2.5f, kTolerance));
    }

    SECTION("sets magnitude while preserving phase")
    {
        constexpr float initialPhase = 0.75f;

        REQUIRE(
            partials->setBin(
                2,
                std::polar(2.0f, initialPhase)
            )
            == Result::Success
        );

        REQUIRE(partials->setMagnitude(2, 5.0f) == Result::Success);

        REQUIRE_THAT(partials->magnitude(2), WithinAbs(5.0f, kTolerance));
        REQUIRE_THAT(partials->phase(2), WithinAbs(initialPhase, kTolerance));
    }

    SECTION("sets phase while preserving magnitude")
    {
        REQUIRE(partials->setBin(2, {3.0f, 4.0f}) == Result::Success);
        REQUIRE(partials->setPhase(2, 1.25f) == Result::Success);

        REQUIRE_THAT(partials->magnitude(2), WithinAbs(5.0f, kTolerance));
        REQUIRE_THAT(partials->phase(2), WithinAbs(1.25f, kTolerance));
    }

    SECTION("clears all bins")
    {
        REQUIRE(partials->setBin(0, {1.0f, 0.0f}) == Result::Success);
        REQUIRE(partials->setBin(2, {3.0f, 4.0f}) == Result::Success);

        partials->clear();

        for (const std::complex<float>& bin : partials->spectrum()) {
            REQUIRE(bin.real() == 0.0f);
            REQUIRE(bin.imag() == 0.0f);
        }
    }
}


TEST_CASE("Partials spectrum conversion", "[DSP][Partials]")
{
    std::unique_ptr<Partials> partials;

    REQUIRE(Partials::create(8, partials) == Result::Success);

    SECTION("copies a spectrum into Partials")
    {
        const std::array<std::complex<float>, 5> input = {
            std::complex<float>{8.0f, 0.0f},
            std::complex<float>{1.0f, 2.0f},
            std::complex<float>{3.0f, 4.0f},
            std::complex<float>{5.0f, 6.0f},
            std::complex<float>{7.0f, 0.0f}
        };

        REQUIRE(partials->setFromSpectrum(input) == Result::Success);

        REQUIRE(partials->spectrum().size() == input.size());

        for (std::size_t i = 0; i < input.size(); ++i) {
            REQUIRE(partials->spectrum()[i] == input[i]);
        }
    }

    SECTION("copies a Partials spectrum into an output array")
    {
        const std::array<std::complex<float>, 5> input = {
            std::complex<float>{8.0f, 0.0f},
            std::complex<float>{1.0f, 2.0f},
            std::complex<float>{3.0f, 4.0f},
            std::complex<float>{5.0f, 6.0f},
            std::complex<float>{7.0f, 0.0f}
        };

        REQUIRE(partials->setFromSpectrum(input) == Result::Success);

        std::array<std::complex<float>, 5> output{};

        REQUIRE(partials->copyToSpectrum(output) == Result::Success);
        REQUIRE(output == input);
    }

    SECTION("rejects incorrect spectrum sizes")
    {
        const std::array<std::complex<float>, 4> shortSpectrum{};

        REQUIRE(
            partials->setFromSpectrum(shortSpectrum)
            == Result::InvalidArgument
        );

        REQUIRE(
            partials->copyToSpectrum(
                std::span<std::complex<float>>{}
            )
            == Result::InvalidArgument
        );
    }
}


TEST_CASE("Partials validates bin access", "[DSP][Partials]")
{
    std::unique_ptr<Partials> partials;

    REQUIRE(Partials::create(8, partials) == Result::Success);

    SECTION("rejects an out-of-range bin")
    {
        REQUIRE(partials->setBin(5, {1.0f, 0.0f}) == Result::InvalidArgument);
        REQUIRE(partials->setMagnitude(5, 1.0f) == Result::InvalidArgument);
        REQUIRE(partials->setPhase(5, 1.0f) == Result::InvalidArgument);

        REQUIRE(partials->magnitude(5) == 0.0f);
        REQUIRE(partials->phase(5) == 0.0f);
    }

    SECTION("rejects a negative magnitude")
    {
        REQUIRE(partials->setMagnitude(1, -1.0f) == Result::InvalidArgument);
    }
}


TEST_CASE("Partials integrates with FFT forward transform", "[DSP][Partials]")
{
    constexpr std::size_t fftSize = 64;

    FFTConfiguration configuration;
    configuration.size = fftSize;
    configuration.backend = FFTBackendType::Portable;

    std::unique_ptr<FFT> fft;
    std::unique_ptr<Partials> partials;

    REQUIRE(FFT::create(configuration, fft) == Result::Success);
    REQUIRE(Partials::create(fftSize, partials) == Result::Success);

    std::array<float, fftSize> samples{};
    samples[0] = 1.0f;

    REQUIRE(
        fft->forward(samples, partials->mutSpectrum())
        == Result::Success
    );

    for (const std::complex<float>& bin : partials->spectrum()) {
        REQUIRE(std::abs(bin - std::complex<float>{1.0f, 0.0f}) < 1.0e-5f);
    }
}


} // namespace Grain::DSP::Tests
