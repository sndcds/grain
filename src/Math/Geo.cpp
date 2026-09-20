#include <grain/Math/Geo.hpp>

#include <cmath>
#include <numbers>
#include <algorithm>

namespace Grain::Geo {

namespace {

constexpr double kEarthRadiusMeters = 6'371'000.0;

}

double haversineDistance(
    const double radius,
    const double lon1,
    const double lat1,
    const double lon2,
    const double lat2
) noexcept
{
    const double dLat = lat2 - lat1;
    const double dLon = lon2 - lon1;

    const double sinLat = std::sin(dLat / 2.0);
    const double sinLon = std::sin(dLon / 2.0);

    const double a =
        sinLat * sinLat
        + std::cos(lat1)
        * std::cos(lat2)
        * sinLon * sinLon;

    const double c =
        2.0 * std::asin(
            std::sqrt(
                std::min(1.0, a)
            )
        );

    return radius * c;
}

double haversineDistanceOnEarth(
    const double lon1,
    const double lat1,
    const double lon2,
    const double lat2
) noexcept
{
    return haversineDistance(
        kEarthRadiusMeters,
        lon1,
        lat1,
        lon2,
        lat2
    );
}

}