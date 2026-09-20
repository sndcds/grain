#include <grain/Math/Optics.hpp>

#include <cmath>

namespace Grain::Optics {

double angleOfView(
    const double sensorSize,
    const double focalLength
) noexcept
{
    return 2.0 * std::atan(
        sensorSize / (2.0 * focalLength)
    );
}

}