#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Math/Functions.hpp>

#include <cmath>
#include <limits>
#include <numbers>

namespace Grain::Functions::Tests {

using Catch::Matchers::WithinAbs;

constexpr double kTolerance = 1e-12;
constexpr double kLooseTolerance = 1e-10;


TEST_CASE("gaussKernel", "[Math][Functions]") {
    SECTION("unit sigma at zero") {
        const double expected =
            1.0 / std::sqrt(2.0 * std::numbers::pi);

        REQUIRE_THAT(
            gaussKernel(0.0, 1.0),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("unit sigma at one") {
        const double expected =
            std::exp(-0.5)
            / std::sqrt(2.0 * std::numbers::pi);

        REQUIRE_THAT(
            gaussKernel(1.0, 1.0),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("unit sigma at negative one") {
        const double expected =
            std::exp(-0.5)
            / std::sqrt(2.0 * std::numbers::pi);

        REQUIRE_THAT(
            gaussKernel(-1.0, 1.0),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("different sigma") {
        constexpr double x = 2.0;
        constexpr double sigma = 2.0;

        const double normalizedX = x / sigma;

        const double expected =
            std::exp(-0.5 * normalizedX * normalizedX)
            / (sigma * std::sqrt(2.0 * std::numbers::pi));

        REQUIRE_THAT(
            gaussKernel(x, sigma),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("negative x and positive x are symmetric") {
        constexpr double sigma = 1.5;

        REQUIRE_THAT(
            gaussKernel(-2.0, sigma),
            WithinAbs(gaussKernel(2.0, sigma), kTolerance)
        );
    }

    SECTION("sigma zero returns NaN") {
        REQUIRE(std::isnan(gaussKernel(1.0, 0.0)));
    }

    SECTION("negative sigma returns NaN") {
        REQUIRE(std::isnan(gaussKernel(1.0, -1.0)));
    }

    SECTION("larger sigma produces a lower peak") {
        REQUIRE(
            gaussKernel(0.0, 2.0)
            < gaussKernel(0.0, 1.0)
        );
    }

    SECTION("kernel decreases away from zero") {
        REQUIRE(
            gaussKernel(0.0, 1.0)
            > gaussKernel(1.0, 1.0)
        );

        REQUIRE(
            gaussKernel(1.0, 1.0)
            > gaussKernel(2.0, 1.0)
        );
    }
}


TEST_CASE("sinc", "[Math][Functions]") {
    SECTION("zero is one") {
        REQUIRE_THAT(
            sinc(0.0),
            WithinAbs(1.0, kTolerance)
        );
    }

    SECTION("one") {
        REQUIRE_THAT(
            sinc(1.0),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("negative one") {
        REQUIRE_THAT(
            sinc(-1.0),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("half") {
        const double expected =
            std::sin(std::numbers::pi / 2.0)
            / (std::numbers::pi / 2.0);

        REQUIRE_THAT(
            sinc(0.5),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("quarter") {
        const double px =
            std::numbers::pi * 0.25;

        const double expected =
            std::sin(px) / px;

        REQUIRE_THAT(
            sinc(0.25),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("two") {
        REQUIRE_THAT(
            sinc(2.0),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("negative and positive values are symmetric") {
        constexpr double values[] = {
            0.1,
            0.25,
            0.5,
            0.75,
            1.25,
            2.5
        };

        for (const double x : values) {
            REQUIRE_THAT(
                sinc(-x),
                WithinAbs(sinc(x), kTolerance)
            );
        }
    }

    SECTION("known zero crossings") {
        constexpr double values[] = {
            -3.0,
            -2.0,
            -1.0,
            1.0,
            2.0,
            3.0
        };

        for (const double x : values) {
            REQUIRE_THAT(
                sinc(x),
                WithinAbs(0.0, kTolerance)
            );
        }
    }
}


TEST_CASE("besselI0", "[Math][Functions]") {
    SECTION("zero is one") {
        REQUIRE_THAT(
            besselI0(0.0),
            WithinAbs(1.0, kTolerance)
        );
    }

    SECTION("negative zero is one") {
        REQUIRE_THAT(
            besselI0(-0.0),
            WithinAbs(1.0, kTolerance)
        );
    }

    SECTION("known value at one") {
        // I0(1) = 1.2660658777520084...
        REQUIRE_THAT(
            besselI0(1.0),
            WithinAbs(1.2660658777520084, kLooseTolerance)
        );
    }

    SECTION("known value at two") {
        // I0(2) = 2.2795853023360673...
        REQUIRE_THAT(
            besselI0(2.0),
            WithinAbs(2.2795853023360673, kLooseTolerance)
        );
    }

    SECTION("known value at three") {
        // I0(3) = 4.880792585865024...
        REQUIRE_THAT(
            besselI0(3.0),
            WithinAbs(4.880792585865024, kLooseTolerance)
        );
    }

    SECTION("known value at four") {
        // I0(4) = 11.30192195213633...
        REQUIRE_THAT(
            besselI0(4.0),
            WithinAbs(11.30192195213633, kLooseTolerance)
        );
    }

    SECTION("function is even") {
        constexpr double values[] = {
            0.1,
            0.5,
            1.0,
            2.0,
            5.0,
            10.0
        };

        for (const double x : values) {
            REQUIRE_THAT(
                besselI0(-x),
                WithinAbs(besselI0(x), kLooseTolerance)
            );
        }
    }

    SECTION("value increases with positive x") {
        REQUIRE(besselI0(1.0) > besselI0(0.0));
        REQUIRE(besselI0(2.0) > besselI0(1.0));
        REQUIRE(besselI0(3.0) > besselI0(2.0));
        REQUIRE(besselI0(4.0) > besselI0(3.0));
    }

    SECTION("series agrees with the defining terms") {
        constexpr double x = 1.0;

        double expected = 1.0;
        double term = 1.0;

        for (int i = 1; i < 20; ++i) {
            const double n = static_cast<double>(i);

            term *=
                (x / 2.0) * (x / 2.0)
                / (n * n);

            expected += term;
        }

        REQUIRE_THAT(
            besselI0(x),
            WithinAbs(expected, kTolerance)
        );
    }
}


TEST_CASE("Functions special values",
          "[Math][Functions]") {
    SECTION("gaussKernel is finite for valid sigma") {
        REQUIRE(std::isfinite(gaussKernel(0.0, 1.0)));
        REQUIRE(std::isfinite(gaussKernel(10.0, 2.0)));
        REQUIRE(std::isfinite(gaussKernel(-10.0, 2.0)));
    }

    SECTION("sinc is finite at zero") {
        REQUIRE(std::isfinite(sinc(0.0)));
    }

    SECTION("besselI0 is finite for ordinary values") {
        REQUIRE(std::isfinite(besselI0(0.0)));
        REQUIRE(std::isfinite(besselI0(1.0)));
        REQUIRE(std::isfinite(besselI0(10.0)));
    }
}

} // namespace Grain::Functions::Tests