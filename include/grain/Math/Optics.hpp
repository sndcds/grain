#pragma once

namespace Grain::Optics {

[[nodiscard]]
double angleOfView(
    double sensorSize,
    double focalLength
) noexcept;

}