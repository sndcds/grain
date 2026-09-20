#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Math/Circle.hpp>

#include <numbers>

namespace Grain::Circle::Tests {

using Catch::Matchers::WithinAbs;

TEST_CASE("Circle::area", "[Math][Circle]") {
    SECTION("zero radius") {
        REQUIRE_THAT(
            area(0.0),
            WithinAbs(0.0, 1e-12)
        );
    }

    SECTION("unit radius") {
        REQUIRE_THAT(
            area(1.0),
            WithinAbs(std::numbers::pi, 1e-12)
        );
    }

    SECTION("radius 2") {
        REQUIRE_THAT(
            area(2.0),
            WithinAbs(4.0 * std::numbers::pi, 1e-12)
        );
    }

    SECTION("radius 10") {
        REQUIRE_THAT(
            area(10.0),
            WithinAbs(100.0 * std::numbers::pi, 1e-10)
        );
    }
}

TEST_CASE("Circle::circumference", "[Math][Circle]") {
    SECTION("zero radius") {
        REQUIRE_THAT(
            circumference(0.0),
            WithinAbs(0.0, 1e-12)
        );
    }

    SECTION("unit radius") {
        REQUIRE_THAT(
            circumference(1.0),
            WithinAbs(2.0 * std::numbers::pi, 1e-12)
        );
    }

    SECTION("radius 2") {
        REQUIRE_THAT(
            circumference(2.0),
            WithinAbs(4.0 * std::numbers::pi, 1e-12)
        );
    }

    SECTION("radius 10") {
        REQUIRE_THAT(
            circumference(10.0),
            WithinAbs(20.0 * std::numbers::pi, 1e-10)
        );
    }
}

TEST_CASE("Circle::radiusFromArea", "[Math][Circle]") {
    SECTION("zero area") {
        REQUIRE_THAT(
            radiusFromArea(0.0),
            WithinAbs(0.0, 1e-12)
        );
    }

    SECTION("area of unit circle") {
        REQUIRE_THAT(
            radiusFromArea(std::numbers::pi),
            WithinAbs(1.0, 1e-12)
        );
    }

    SECTION("area for radius 2") {
        REQUIRE_THAT(
            radiusFromArea(4.0 * std::numbers::pi),
            WithinAbs(2.0, 1e-12)
        );
    }

    SECTION("area for radius 10") {
        REQUIRE_THAT(
            radiusFromArea(100.0 * std::numbers::pi),
            WithinAbs(10.0, 1e-11)
        );
    }
}

TEST_CASE("Circle area and radiusFromArea are inverse functions",
          "[Math][Circle]") {
    SECTION("several radii") {
        const double radii[] = {
            0.0,
            0.1,
            0.5,
            1.0,
            2.0,
            5.0,
            10.0,
            100.0
        };

        for (const double radius : radii) {
            const double calculatedArea = area(radius);
            const double calculatedRadius = radiusFromArea(calculatedArea);

            REQUIRE_THAT(
                calculatedRadius,
                WithinAbs(radius, 1e-10)
            );
        }
    }
}

TEST_CASE("Circle area grows quadratically with radius",
          "[Math][Circle]") {
    const double radius = 3.0;

    REQUIRE_THAT(
        area(2.0 * radius),
        WithinAbs(4.0 * area(radius), 1e-12)
    );
}

TEST_CASE("Circle circumference grows linearly with radius",
          "[Math][Circle]") {
    const double radius = 3.0;

    REQUIRE_THAT(
        circumference(2.0 * radius),
        WithinAbs(2.0 * circumference(radius), 1e-12)
    );
}

} // namespace Grain::Circle::Tests