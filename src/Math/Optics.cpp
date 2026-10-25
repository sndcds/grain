#include <grain/Math/Optics.hpp>

#include <cmath>

namespace Grain::Optics {

auto angleOfView(const double sensor_size, const double focal_length) -> double {
    return 2.0 * std::atan(sensor_size / (2.0 * focal_length));
}

}