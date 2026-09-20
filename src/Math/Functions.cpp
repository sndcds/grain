#include <grain/Math/Functions.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>

namespace Grain::Functions {

double gaussKernel(
    const double x,
    const double sigma
) noexcept
{
    if (sigma <= 0.0) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    const double normalizedX = x / sigma;

    return std::exp(-0.5 * normalizedX * normalizedX)
         / (sigma * std::sqrt(2.0 * std::numbers::pi));
}

double sinc(const double x) noexcept {
    if (x == 0.0) {
        return 1.0;
    }

    const double px = std::numbers::pi * x;

    return std::sin(px) / px;
}

double besselI0(const double x) noexcept {
    const double halfX = x / 2.0;

    double term = 1.0;
    double sum = 1.0;

    for (std::int32_t i = 1; i < 1000; ++i) {
        const double n = static_cast<double>(i);

        term *= (halfX * halfX) / (n * n);

        const double next = sum + term;

        if (next == sum) {
            break;
        }

        sum = next;
    }

    return sum;
}

} // namespace Grain::Functions