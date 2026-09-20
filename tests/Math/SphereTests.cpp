#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Math/Sphere.hpp>

#include <cmath>
#include <numbers>

namespace Grain::Sphere::Tests {

using Catch::Matchers::WithinAbs;

constexpr double kTolerance = 1e-12;

TEST_CASE("Sphere::area", "[Math][Sphere]") {
    SECTION("zero radius") {
        REQUIRE_THAT(
            area(0.0),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("unit sphere") {
        REQUIRE_THAT(
            area(1.0),
            WithinAbs(4.0 * std::numbers::pi, kTolerance)
        );
    }

    SECTION("radius two") {
        REQUIRE_THAT(
            area(2.0),
            WithinAbs(16.0 * std::numbers::pi, kTolerance)
        );
    }

    SECTION("radius three") {
        REQUIRE_THAT(
            area(3.0),
            WithinAbs(36.0 * std::numbers::pi, kTolerance)
        );
    }

    SECTION("area scales with square of radius") {
        const double r1 = 2.0;
        const double r2 = 4.0;

        REQUIRE_THAT(
            area(r2),
            WithinAbs(
                area(r1) * 4.0,
                kTolerance
            )
        );
    }

    SECTION("area is independent of radius sign") {
        REQUIRE_THAT(
            area(-2.0),
            WithinAbs(area(2.0), kTolerance)
        );
    }
}


TEST_CASE("Sphere::volume", "[Math][Sphere]") {
    SECTION("zero radius") {
        REQUIRE_THAT(
            volume(0.0),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("unit sphere") {
        REQUIRE_THAT(
            volume(1.0),
            WithinAbs(
                (4.0 / 3.0) * std::numbers::pi,
                kTolerance
            )
        );
    }

    SECTION("radius two") {
        REQUIRE_THAT(
            volume(2.0),
            WithinAbs(
                (32.0 / 3.0) * std::numbers::pi,
                kTolerance
            )
        );
    }

    SECTION("radius three") {
        REQUIRE_THAT(
            volume(3.0),
            WithinAbs(
                36.0 * std::numbers::pi,
                kTolerance
            )
        );
    }

    SECTION("volume scales with cube of radius") {
        const double r1 = 2.0;
        const double r2 = 4.0;

        REQUIRE_THAT(
            volume(r2),
            WithinAbs(
                volume(r1) * 8.0,
                kTolerance
            )
        );
    }

    SECTION("volume changes sign with radius") {
        REQUIRE_THAT(
            volume(-2.0),
            WithinAbs(-volume(2.0), kTolerance)
        );
    }
}


TEST_CASE("Sphere::area and Sphere::volume relationship",
          "[Math][Sphere]") {
    SECTION("volume divided by radius equals one third of area") {
        constexpr double radius = 5.0;

        REQUIRE_THAT(
            volume(radius) / radius,
            WithinAbs(
                area(radius) / 3.0,
                kTolerance
            )
        );
    }

    SECTION("surface area and volume for radius one") {
        REQUIRE_THAT(
            area(1.0),
            WithinAbs(4.0 * std::numbers::pi, kTolerance)
        );

        REQUIRE_THAT(
            volume(1.0),
            WithinAbs(
                4.0 * std::numbers::pi / 3.0,
                kTolerance
            )
        );
    }
}

} // namespace Grain::Sphere::Tests