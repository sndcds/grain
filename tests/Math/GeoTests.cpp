#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Math/Geo.hpp>

#include <cmath>
#include <numbers>

namespace Grain::Geo::Tests {

using Catch::Matchers::WithinAbs;

constexpr double kTolerance = 1e-6;
constexpr double kEarthRadiusMeters = 6'371'000.0;

constexpr double degreesToRadians(const double degrees) noexcept {
    return degrees * std::numbers::pi / 180.0;
}


TEST_CASE("haversineDistance", "[Math][Geo]") {
    SECTION("identical points have zero distance") {
        REQUIRE_THAT(
            haversineDistance(
                1.0,
                degreesToRadians(9.99),
                degreesToRadians(54.78),
                degreesToRadians(9.99),
                degreesToRadians(54.78)
            ),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("distance is symmetric") {
        const double lon1 = degreesToRadians(9.0);
        const double lat1 = degreesToRadians(54.0);
        const double lon2 = degreesToRadians(10.0);
        const double lat2 = degreesToRadians(55.0);

        REQUIRE_THAT(
            haversineDistance(1.0, lon1, lat1, lon2, lat2),
            WithinAbs(
                haversineDistance(1.0, lon2, lat2, lon1, lat1),
                kTolerance
            )
        );
    }

    SECTION("quarter circumference on equator") {
        const double distance = haversineDistance(
            1.0,
            0.0,
            0.0,
            std::numbers::pi / 2.0,
            0.0
        );

        REQUIRE_THAT(
            distance,
            WithinAbs(std::numbers::pi / 2.0, kTolerance)
        );
    }

    SECTION("half circumference on equator") {
        const double distance = haversineDistance(
            1.0,
            0.0,
            0.0,
            std::numbers::pi,
            0.0
        );

        REQUIRE_THAT(
            distance,
            WithinAbs(std::numbers::pi, kTolerance)
        );
    }

    SECTION("north pole to south pole") {
        const double distance = haversineDistance(
            1.0,
            0.0,
            std::numbers::pi / 2.0,
            0.0,
            -std::numbers::pi / 2.0
        );

        REQUIRE_THAT(
            distance,
            WithinAbs(std::numbers::pi, kTolerance)
        );
    }

    SECTION("longitude difference on equator") {
        const double distance = haversineDistance(
            1.0,
            0.0,
            0.0,
            degreesToRadians(1.0),
            0.0
        );

        REQUIRE_THAT(
            distance,
            WithinAbs(degreesToRadians(1.0), kTolerance)
        );
    }

    SECTION("latitude difference on meridian") {
        const double distance = haversineDistance(
            1.0,
            0.0,
            0.0,
            0.0,
            degreesToRadians(1.0)
        );

        REQUIRE_THAT(
            distance,
            WithinAbs(degreesToRadians(1.0), kTolerance)
        );
    }

    SECTION("distance scales linearly with radius") {
        const double lon1 = degreesToRadians(9.0);
        const double lat1 = degreesToRadians(54.0);
        const double lon2 = degreesToRadians(10.0);
        const double lat2 = degreesToRadians(55.0);

        const double distance = haversineDistance(
            1.0,
            lon1,
            lat1,
            lon2,
            lat2
        );

        REQUIRE_THAT(
            haversineDistance(
                2.0,
                lon1,
                lat1,
                lon2,
                lat2
            ),
            WithinAbs(2.0 * distance, kTolerance)
        );

        REQUIRE_THAT(
            haversineDistance(
                10.0,
                lon1,
                lat1,
                lon2,
                lat2
            ),
            WithinAbs(10.0 * distance, kTolerance)
        );
    }

    SECTION("zero radius always gives zero") {
        REQUIRE_THAT(
            haversineDistance(
                0.0,
                0.0,
                0.0,
                std::numbers::pi,
                0.0
            ),
            WithinAbs(0.0, kTolerance)
        );
    }
}


TEST_CASE("haversineDistanceOnEarth", "[Math][Geo]") {
    SECTION("identical points have zero distance") {
        REQUIRE_THAT(
            haversineDistanceOnEarth(
                degreesToRadians(9.99),
                degreesToRadians(54.78),
                degreesToRadians(9.99),
                degreesToRadians(54.78)
            ),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("one degree longitude at equator") {
        const double expected =
            kEarthRadiusMeters
            * degreesToRadians(1.0);

        REQUIRE_THAT(
            haversineDistanceOnEarth(
                0.0,
                0.0,
                degreesToRadians(1.0),
                0.0
            ),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("one degree latitude") {
        const double expected =
            kEarthRadiusMeters
            * degreesToRadians(1.0);

        REQUIRE_THAT(
            haversineDistanceOnEarth(
                0.0,
                0.0,
                0.0,
                degreesToRadians(1.0)
            ),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("quarter circumference") {
        const double expected =
            kEarthRadiusMeters
            * std::numbers::pi
            / 2.0;

        REQUIRE_THAT(
            haversineDistanceOnEarth(
                0.0,
                0.0,
                std::numbers::pi / 2.0,
                0.0
            ),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("half circumference") {
        const double expected =
            kEarthRadiusMeters
            * std::numbers::pi;

        REQUIRE_THAT(
            haversineDistanceOnEarth(
                0.0,
                0.0,
                std::numbers::pi,
                0.0
            ),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("known Flensburg to Kiel distance") {
        // Approximate coordinates:
        // Flensburg: 9.446996° E, 54.793743° N
        // Kiel:      10.122765° E, 54.323292° N
        //
        // Expected great-circle distance is approximately 69 km.
        const double distance = haversineDistanceOnEarth(
            degreesToRadians(9.446996),
            degreesToRadians(54.793743),
            degreesToRadians(10.122765),
            degreesToRadians(54.323292)
        );

        REQUIRE(distance > 68'000.0);
        REQUIRE(distance < 70'000.0);
    }
}


TEST_CASE("Haversine distance handles longitude wraparound",
          "[Math][Geo]") {
    const double distance = haversineDistance(
        1.0,
        degreesToRadians(179.0),
        0.0,
        degreesToRadians(-179.0),
        0.0
    );

    REQUIRE_THAT(
        distance,
        WithinAbs(degreesToRadians(2.0), kTolerance)
    );
}


TEST_CASE("Haversine distance is symmetric",
          "[Math][Geo]") {
    constexpr double lon1 = 0.25;
    constexpr double lat1 = 0.75;
    constexpr double lon2 = 1.25;
    constexpr double lat2 = 0.5;

    REQUIRE_THAT(
        haversineDistance(
            kEarthRadiusMeters,
            lon1,
            lat1,
            lon2,
            lat2
        ),
        WithinAbs(
            haversineDistance(
                kEarthRadiusMeters,
                lon2,
                lat2,
                lon1,
                lat1
            ),
            kTolerance
        )
    );
}

} // namespace Grain::Geo::Tests