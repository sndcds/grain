
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/catch_approx.hpp>

#include <grain/DSP/FFT/FFT.hpp>
#include <grain/DSP/Partials.hpp>

#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <memory>
#include <numbers>
#include <limits>

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


TEST_CASE("Partials Nyquist bin for even FFT size", "[DSP][Partials]")
{
    std::unique_ptr<Partials> partials;

    REQUIRE(Partials::create(8, partials) == Result::Success);
    REQUIRE(partials != nullptr);
    REQUIRE(partials->hasNyquistBin());

    REQUIRE(partials->setBin(4, {0.75f, 0.0f}) == Result::Success);

    REQUIRE(partials->nyquist() == Catch::Approx(0.75f));
}


TEST_CASE("Partials has no Nyquist bin for odd FFT size", "[DSP][Partials]")
{
    std::unique_ptr<Partials> partials;

    REQUIRE(Partials::create(7, partials) == Result::Success);
    REQUIRE(partials != nullptr);
    REQUIRE_FALSE(partials->hasNyquistBin());

    REQUIRE(partials->nyquist() == Catch::Approx(0.0f));
}


TEST_CASE("Partials spectrum scaling", "[DSP][Partials]")
{
    std::unique_ptr<Partials> partials;

    REQUIRE(Partials::create(8, partials) == Result::Success);

    REQUIRE(partials->setBin(0, {1.0f, 0.0f}) == Result::Success);
    REQUIRE(partials->setBin(1, {2.0f, 3.0f}) == Result::Success);
    REQUIRE(partials->setBin(2, {-1.0f, 2.0f}) == Result::Success);

    SECTION("scales all bins")
    {
        REQUIRE(partials->scale(2.0f) == Result::Success);

        REQUIRE(partials->spectrum()[0] == std::complex<float>{2.0f, 0.0f});
        REQUIRE(partials->spectrum()[1] == std::complex<float>{4.0f, 6.0f});
        REQUIRE(partials->spectrum()[2] == std::complex<float>{-2.0f, 4.0f});
    }

    SECTION("supports negative scaling")
    {
        REQUIRE(partials->scale(-1.0f) == Result::Success);

        REQUIRE(partials->spectrum()[0] == std::complex<float>{-1.0f, 0.0f});
        REQUIRE(partials->spectrum()[1] == std::complex<float>{-2.0f, -3.0f});
    }

    SECTION("rejects a non-finite factor")
    {
        REQUIRE(
            partials->scale(std::numeric_limits<float>::infinity())
            == Result::InvalidArgument
        );

        REQUIRE(partials->spectrum()[0] == std::complex<float>{1.0f, 0.0f});
    }
}


TEST_CASE("Partials spectrum multiplication", "[DSP][Partials]")
{
    std::unique_ptr<Partials> first;
    std::unique_ptr<Partials> second;

    REQUIRE(Partials::create(8, first) == Result::Success);
    REQUIRE(Partials::create(8, second) == Result::Success);

    REQUIRE(first->setBin(0, {1.0f, 0.0f}) == Result::Success);
    REQUIRE(first->setBin(1, {2.0f, 1.0f}) == Result::Success);

    REQUIRE(second->setBin(0, {2.0f, 0.0f}) == Result::Success);
    REQUIRE(second->setBin(1, {1.0f, -1.0f}) == Result::Success);

    SECTION("multiplies corresponding complex bins")
    {
        REQUIRE(first->multiply(*second) == Result::Success);

        REQUIRE(first->spectrum()[0] == std::complex<float>{2.0f, 0.0f});
        REQUIRE(first->spectrum()[1] == std::complex<float>{3.0f, -1.0f});
    }

    SECTION("rejects different FFT sizes")
    {
        std::unique_ptr<Partials> differentSize;

        REQUIRE(Partials::create(16, differentSize) == Result::Success);

        REQUIRE(first->multiply(*differentSize) == Result::InvalidArgument);
    }

    SECTION("supports multiplication with itself")
    {
        REQUIRE(first->multiply(*first) == Result::Success);

        REQUIRE(first->spectrum()[0] == std::complex<float>{1.0f, 0.0f});
        REQUIRE(first->spectrum()[1] == std::complex<float>{3.0f, 4.0f});
    }
}


TEST_CASE("Partials spectrum interpolation", "[DSP][Partials]")
{
    std::unique_ptr<Partials> first;
    std::unique_ptr<Partials> second;

    REQUIRE(Partials::create(8, first) == Result::Success);
    REQUIRE(Partials::create(8, second) == Result::Success);

    REQUIRE(first->setBin(0, {1.0f, 0.0f}) == Result::Success);
    REQUIRE(first->setBin(1, {2.0f, 2.0f}) == Result::Success);

    REQUIRE(second->setBin(0, {3.0f, 0.0f}) == Result::Success);
    REQUIRE(second->setBin(1, {4.0f, -2.0f}) == Result::Success);

    SECTION("interpolates halfway")
    {
        REQUIRE(first->interpolate(*second, 0.5f) == Result::Success);

        REQUIRE(first->spectrum()[0] == std::complex<float>{2.0f, 0.0f});
        REQUIRE(first->spectrum()[1] == std::complex<float>{3.0f, 0.0f});
    }

    SECTION("position zero preserves the original spectrum")
    {
        const auto original = first->spectrum()[1];

        REQUIRE(first->interpolate(*second, 0.0f) == Result::Success);

        REQUIRE(first->spectrum()[1] == original);
    }

    SECTION("position one copies the other spectrum")
    {
        REQUIRE(first->interpolate(*second, 1.0f) == Result::Success);

        REQUIRE(first->spectrum()[0] == second->spectrum()[0]);
        REQUIRE(first->spectrum()[1] == second->spectrum()[1]);
    }

    SECTION("rejects positions outside the valid range")
    {
        REQUIRE(first->interpolate(*second, -0.1f) == Result::InvalidArgument);
        REQUIRE(first->interpolate(*second, 1.1f) == Result::InvalidArgument);
    }

    SECTION("rejects non-finite positions")
    {
        REQUIRE(
            first->interpolate(
                *second,
                std::numeric_limits<float>::quiet_NaN()
            ) == Result::InvalidArgument
        );

        REQUIRE(
            first->interpolate(
                *second,
                std::numeric_limits<float>::infinity()
            ) == Result::InvalidArgument
        );
    }

    SECTION("rejects different FFT sizes")
    {
        std::unique_ptr<Partials> differentSize;

        REQUIRE(Partials::create(16, differentSize) == Result::Success);

        REQUIRE(
            first->interpolate(*differentSize, 0.5f)
            == Result::InvalidArgument
        );
    }
}


TEST_CASE("Partials move operations", "[DSP][Partials]")
{
    std::unique_ptr<Partials> source;
    REQUIRE(Partials::create(8, source) == Result::Success);

    REQUIRE(source->setBin(4, {0.75f, 0.0f}) == Result::Success);

    SECTION("move constructor")
    {
        Partials moved(std::move(*source));

        REQUIRE(moved.fftSize() == 8);
        REQUIRE(moved.binCount() == 5);
        REQUIRE(moved.nyquist() == Catch::Approx(0.75f));

        REQUIRE(source->fftSize() == 0);
        REQUIRE(source->binCount() == 0);
        REQUIRE_FALSE(source->hasNyquistBin());
        REQUIRE(source->nyquist() == 0.0f);
    }

    SECTION("move assignment")
    {
        std::unique_ptr<Partials> destination;
        REQUIRE(Partials::create(16, destination) == Result::Success);

        *destination = std::move(*source);

        REQUIRE(destination->fftSize() == 8);
        REQUIRE(destination->nyquist() == Catch::Approx(0.75f));

        REQUIRE(source->fftSize() == 0);
        REQUIRE(source->binCount() == 0);
        REQUIRE(source->nyquist() == 0.0f);
    }
}


} // namespace Grain::DSP::Tests
