#include <grain/Math/Interpolation.hpp>

#include <array>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace Grain::Interpolation::Tests {

TEST_CASE("Interpolation::cubic", "[Interpolation][cubic]")
{
    SECTION("returns the middle samples at the endpoints")
    {
        REQUIRE_THAT(
            cubic(0.0, 10.0, 20.0, 30.0, 0.0),
            WithinAbs(10.0, 1e-12)
        );

        REQUIRE_THAT(
            cubic(0.0, 10.0, 20.0, 30.0, 1.0),
            WithinAbs(20.0, 1e-12)
        );
    }

    SECTION("constant values remain constant")
    {
        REQUIRE_THAT(
            cubic(5.0, 5.0, 5.0, 5.0, 0.0),
            WithinAbs(5.0, 1e-12)
        );

        REQUIRE_THAT(
            cubic(5.0, 5.0, 5.0, 5.0, 0.25),
            WithinAbs(5.0, 1e-12)
        );

        REQUIRE_THAT(
            cubic(5.0, 5.0, 5.0, 5.0, 0.5),
            WithinAbs(5.0, 1e-12)
        );

        REQUIRE_THAT(
            cubic(5.0, 5.0, 5.0, 5.0, 0.75),
            WithinAbs(5.0, 1e-12)
        );

        REQUIRE_THAT(
            cubic(5.0, 5.0, 5.0, 5.0, 1.0),
            WithinAbs(5.0, 1e-12)
        );
    }

    SECTION("linear values remain linear")
    {
        // Samples from y = 2x + 1 at x = -1, 0, 1, 2.
        REQUIRE_THAT(
            cubic(-1.0, 1.0, 3.0, 5.0, 0.25),
            WithinAbs(1.5, 1e-12)
        );

        REQUIRE_THAT(
            cubic(-1.0, 1.0, 3.0, 5.0, 0.5),
            WithinAbs(2.0, 1e-12)
        );

        REQUIRE_THAT(
            cubic(-1.0, 1.0, 3.0, 5.0, 0.75),
            WithinAbs(2.5, 1e-12)
        );
    }

    SECTION("midpoint of symmetric data")
    {
        REQUIRE_THAT(
            cubic(0.0, 1.0, 1.0, 0.0, 0.5),
            WithinAbs(1.125, 1e-12)
        );
    }

    SECTION("negative values")
    {
        REQUIRE_THAT(
            cubic(-30.0, -20.0, -10.0, 0.0, 0.5),
            WithinAbs(-15.0, 1e-12)
        );
    }
}


TEST_CASE("Interpolation::bicubic", "[Interpolation][bicubic]")
{
    SECTION("constant grid remains constant")
    {
        const std::array samples{
            5.0, 5.0, 5.0, 5.0,
            5.0, 5.0, 5.0, 5.0,
            5.0, 5.0, 5.0, 5.0,
            5.0, 5.0, 5.0, 5.0
        };

        REQUIRE_THAT(
            bicubic(samples, 0.0, 0.0),
            WithinAbs(5.0, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.25, 0.75),
            WithinAbs(5.0, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.5, 0.5),
            WithinAbs(5.0, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 1.0, 1.0),
            WithinAbs(5.0, 1e-12)
        );
    }

    SECTION("linear function in x")
    {
        // f(x,y) = 2x + 10
        const std::array samples{
            8.0, 10.0, 12.0, 14.0,
            8.0, 10.0, 12.0, 14.0,
            8.0, 10.0, 12.0, 14.0,
            8.0, 10.0, 12.0, 14.0
        };

        REQUIRE_THAT(
            bicubic(samples, 0.25, 0.5),
            WithinAbs(10.5, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.5, 0.5),
            WithinAbs(11.0, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.75, 0.5),
            WithinAbs(11.5, 1e-12)
        );
    }

    SECTION("linear function in y")
    {
        // f(x,y) = 3y + 10
        const std::array samples{
            7.0, 7.0, 7.0, 7.0,
            10.0, 10.0, 10.0, 10.0,
            13.0, 13.0, 13.0, 13.0,
            16.0, 16.0, 16.0, 16.0
        };

        REQUIRE_THAT(
            bicubic(samples, 0.5, 0.25),
            WithinAbs(10.75, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.5, 0.5),
            WithinAbs(11.5, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.5, 0.75),
            WithinAbs(12.25, 1e-12)
        );
    }

    SECTION("bilinear function remains exact")
    {
        // f(x,y) = 2x + 3y + 5
        const std::array samples{
             0.0,  2.0,  4.0,  6.0,
             3.0,  5.0,  7.0,  9.0,
             6.0,  8.0, 10.0, 12.0,
             9.0, 11.0, 13.0, 15.0
        };

        REQUIRE_THAT(
            bicubic(samples, 0.25, 0.25),
            WithinAbs(6.25, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.5, 0.5),
            WithinAbs(7.5, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.75, 0.75),
            WithinAbs(8.75, 1e-12)
        );
    }

    SECTION("interpolates the four-dimensional corners")
    {
        const std::array samples{
             0.0,  1.0,  2.0,  3.0,
             4.0,  5.0,  6.0,  7.0,
             8.0,  9.0, 10.0, 11.0,
            12.0, 13.0, 14.0, 15.0
        };

        REQUIRE_THAT(
            bicubic(samples, 0.0, 0.0),
            WithinAbs(5.0, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 1.0, 0.0),
            WithinAbs(6.0, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 0.0, 1.0),
            WithinAbs(9.0, 1e-12)
        );

        REQUIRE_THAT(
            bicubic(samples, 1.0, 1.0),
            WithinAbs(10.0, 1e-12)
        );
    }
}


TEST_CASE("Interpolation::tricubic", "[Interpolation][tricubic]")
{
    SECTION("constant volume remains constant")
    {
        std::array<double, 64> samples{
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,

            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,

            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,

            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0,
            7.0, 7.0, 7.0, 7.0
        };

        REQUIRE_THAT(
            tricubic(samples, 0.25, 0.5, 0.75),
            WithinAbs(7.0, 1e-12)
        );

        REQUIRE_THAT(
            tricubic(samples, 0.0, 0.0, 0.0),
            WithinAbs(7.0, 1e-12)
        );

        REQUIRE_THAT(
            tricubic(samples, 1.0, 1.0, 1.0),
            WithinAbs(7.0, 1e-12)
        );
    }

    SECTION("linear function remains exact")
    {
        // f(x,y,z) = x + 2y + 4z + 10
        //
        // The interpolation interval is between sample indices
        // 1 and 2, so:
        //
        // t = 0.25 -> coordinate 1.25
        // t = 0.50 -> coordinate 1.50
        // t = 0.75 -> coordinate 1.75
        std::array<double, 64> samples{};

        for (std::size_t z = 0; z < 4; ++z) {
            for (std::size_t y = 0; y < 4; ++y) {
                for (std::size_t x = 0; x < 4; ++x) {
                    samples[z * 16 + y * 4 + x] =
                        static_cast<double>(x)
                        + 2.0 * static_cast<double>(y)
                        + 4.0 * static_cast<double>(z)
                        + 10.0;
                }
            }
        }

        REQUIRE_THAT(
            tricubic(samples, 0.25, 0.5, 0.75),
            WithinAbs(21.25, 1e-12)
        );

        REQUIRE_THAT(
            tricubic(samples, 0.5, 0.5, 0.5),
            WithinAbs(20.5, 1e-12)
        );

        REQUIRE_THAT(
            tricubic(samples, 0.75, 0.75, 0.75),
            WithinAbs(22.25, 1e-12)
        );
    }

    SECTION("interpolates the central 2x2x2 region")
    {
        std::array<double, 64> samples{};

        for (std::size_t z = 0; z < 4; ++z) {
            for (std::size_t y = 0; y < 4; ++y) {
                for (std::size_t x = 0; x < 4; ++x) {
                    samples[z * 16 + y * 4 + x] =
                        static_cast<double>(
                            z * 16 + y * 4 + x
                        );
                }
            }
        }

        // t = 0 selects the central sample at index 1.
        REQUIRE_THAT(
            tricubic(samples, 0.0, 0.0, 0.0),
            WithinAbs(21.0, 1e-12)
        );

        // x = 2, y = 1, z = 1
        REQUIRE_THAT(
            tricubic(samples, 1.0, 0.0, 0.0),
            WithinAbs(22.0, 1e-12)
        );

        // x = 1, y = 2, z = 1
        REQUIRE_THAT(
            tricubic(samples, 0.0, 1.0, 0.0),
            WithinAbs(25.0, 1e-12)
        );

        // x = 1, y = 1, z = 2
        REQUIRE_THAT(
            tricubic(samples, 0.0, 0.0, 1.0),
            WithinAbs(37.0, 1e-12)
        );
    }
}


TEST_CASE("Interpolation::nCubic", "[Interpolation][nCubic]")
{
    SECTION("one-dimensional interpolation")
    {
        const std::array samples{
            -1.0,
             1.0,
             3.0,
             5.0
        };

        REQUIRE_THAT(
            nCubic(samples, std::array{0.0}),
            WithinAbs(1.0, 1e-12)
        );

        REQUIRE_THAT(
            nCubic(samples, std::array{0.5}),
            WithinAbs(2.0, 1e-12)
        );

        REQUIRE_THAT(
            nCubic(samples, std::array{1.0}),
            WithinAbs(3.0, 1e-12)
        );
    }

    SECTION("two-dimensional interpolation")
    {
        const std::array samples{
             0.0,  2.0,  4.0,  6.0,
             3.0,  5.0,  7.0,  9.0,
             6.0,  8.0, 10.0, 12.0,
             9.0, 11.0, 13.0, 15.0
        };

        REQUIRE_THAT(
            nCubic(samples, std::array{0.5, 0.5}),
            WithinAbs(7.5, 1e-12)
        );
    }

    SECTION("three-dimensional interpolation")
    {
        std::array<double, 64> samples{};

        for (std::size_t z = 0; z < 4; ++z) {
            for (std::size_t y = 0; y < 4; ++y) {
                for (std::size_t x = 0; x < 4; ++x) {
                    samples[z * 16 + y * 4 + x] =
                        static_cast<double>(x)
                        + 2.0 * static_cast<double>(y)
                        + 4.0 * static_cast<double>(z)
                        + 10.0;
                }
            }
        }

        REQUIRE_THAT(
            nCubic(samples, std::array{0.5, 0.5, 0.5}),
            WithinAbs(20.5, 1e-12)
        );
    }

    SECTION("four-dimensional interpolation")
    {
        std::array<double, 256> samples{};

        for (std::size_t w = 0; w < 4; ++w) {
            for (std::size_t z = 0; z < 4; ++z) {
                for (std::size_t y = 0; y < 4; ++y) {
                    for (std::size_t x = 0; x < 4; ++x) {
                        const std::size_t index =
                            w * 64
                            + z * 16
                            + y * 4
                            + x;

                        samples[index] =
                            static_cast<double>(x)
                            + 2.0 * static_cast<double>(y)
                            + 4.0 * static_cast<double>(z)
                            + 8.0 * static_cast<double>(w)
                            + 10.0;
                    }
                }
            }
        }

        REQUIRE_THAT(
            nCubic(samples, std::array{0.5, 0.5, 0.5, 0.5}),
            WithinAbs(32.5, 1e-12)
        );
    }

    SECTION("constant values remain constant")
    {
        const std::array samples{
            42.0, 42.0, 42.0, 42.0,
            42.0, 42.0, 42.0, 42.0,
            42.0, 42.0, 42.0, 42.0,
            42.0, 42.0, 42.0, 42.0
        };

        REQUIRE_THAT(
            nCubic(samples, std::array{0.1, 0.9}),
            WithinAbs(42.0, 1e-12)
        );
    }

    SECTION("zero-dimensional interpolation")
    {
        const std::array samples{42.0};

        REQUIRE_THAT(
            nCubic(samples, std::array<double, 0>{}),
            WithinAbs(42.0, 1e-12)
        );
    }

    SECTION("invalid sample count returns zero")
    {
        const std::array samples{
            1.0, 2.0, 3.0
        };

        REQUIRE_THAT(
            nCubic(samples, std::array{0.5}),
            WithinAbs(0.0, 1e-12)
        );
    }

    SECTION("empty samples return zero")
    {
        const std::array<double, 0> samples{};

        REQUIRE_THAT(
            nCubic(samples, std::array{0.5}),
            WithinAbs(0.0, 1e-12)
        );
    }
}

} // namespace Grain::Interpolation::Tests