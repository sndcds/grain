#pragma once

namespace Grain::Geo {

/// Calculates the great-circle distance between two points on a sphere.
///
/// Longitude and latitude are specified in radians.
/// Radius and returned distance use the same unit.
[[nodiscard]]
double haversineDistance(
    double radius,
    double lon1,
    double lat1,
    double lon2,
    double lat2
) noexcept;

/// Calculates the great-circle distance between two points on Earth.
///
/// Longitude and latitude are specified in radians.
/// The returned distance is in metres.
[[nodiscard]]
double haversineDistanceOnEarth(
    double lon1,
    double lat1,
    double lon2,
    double lat2
) noexcept;

}