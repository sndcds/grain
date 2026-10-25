#pragma once

namespace Grain::Optics {

[[nodiscard]] auto angleOfView(double sensor_size, double focal_length) -> double;

}