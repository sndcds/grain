#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Math/Optics.hpp>

#include <cmath>
#include <numbers>

namespace Grain::Optics::Tests {

using Catch::Matchers::WithinAbs;

constexpr double kTolerance = 1e-12;


TEST_CASE("angleOfView", "[Math][Optics]") {
    SECTION("zero sensor size gives zero angle") {
        REQUIRE_THAT(
            angleOfView(0.0, 50.0),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("sensor size equals focal length") {
        const double expected =
            2.0 * std::atan(0.5);

        REQUIRE_THAT(
            angleOfView(50.0, 50.0),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("sensor size is twice the focal length") {
        const double expected =
            2.0 * std::atan(1.0);

        REQUIRE_THAT(
            angleOfView(100.0, 50.0),
            WithinAbs(expected, kTolerance)
        );

        REQUIRE_THAT(
            angleOfView(100.0, 50.0),
            WithinAbs(std::numbers::pi / 2.0, kTolerance)
        );
    }

    SECTION("sensor size is four times the focal length") {
        const double expected =
            2.0 * std::atan(2.0);

        REQUIRE_THAT(
            angleOfView(200.0, 50.0),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("typical full-frame horizontal field of view") {
        // 36 mm sensor width, 50 mm focal length.
        const double expected =
            2.0 * std::atan(36.0 / (2.0 * 50.0));

        REQUIRE_THAT(
            angleOfView(36.0, 50.0),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("typical full-frame 35 mm lens") {
        // 36 mm sensor width, 35 mm focal length.
        const double expected =
            2.0 * std::atan(36.0 / (2.0 * 35.0));

        REQUIRE_THAT(
            angleOfView(36.0, 35.0),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("angle increases with sensor size") {
        const double focalLength = 50.0;

        REQUIRE(
            angleOfView(24.0, focalLength)
            < angleOfView(36.0, focalLength)
        );

        REQUIRE(
            angleOfView(36.0, focalLength)
            < angleOfView(48.0, focalLength)
        );
    }

    SECTION("angle decreases with focal length") {
        const double sensorSize = 36.0;

        REQUIRE(
            angleOfView(sensorSize, 100.0)
            < angleOfView(sensorSize, 50.0)
        );

        REQUIRE(
            angleOfView(sensorSize, 50.0)
            < angleOfView(sensorSize, 35.0)
        );
    }

    SECTION("same sensor-to-focal-length ratio gives same angle") {
        REQUIRE_THAT(
            angleOfView(36.0, 50.0),
            WithinAbs(
                angleOfView(72.0, 100.0),
                kTolerance
            )
        );

        REQUIRE_THAT(
            angleOfView(24.0, 50.0),
            WithinAbs(
                angleOfView(48.0, 100.0),
                kTolerance
            )
        );
    }

    SECTION("angle is in radians") {
        REQUIRE_THAT(
            angleOfView(100.0, 50.0),
            WithinAbs(std::numbers::pi / 2.0, kTolerance)
        );
    }
}

} // namespace Grain::Optics::Tests