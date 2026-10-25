#include <grain/Math/Functions.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>

namespace Grain::Functions {

auto gaussKernel(const double x,const double sigma) -> double {
    if (sigma <= 0.0) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    const double normalized_x = x / sigma;

    return std::exp(-0.5 * normalized_x * normalized_x) / (sigma * std::sqrt(2.0 * std::numbers::pi));
}

auto sinc(const double x) -> double {
    if (x == 0.0) {
        return 1.0;
    }

    const double px = std::numbers::pi * x;

    return std::sin(px) / px;
}

auto besselI0(const double x) -> double {
    const double half_x = x / 2.0;

    double term = 1.0;
    double sum = 1.0;

    for (std::int32_t i = 1; i < 1000; ++i) {
        const auto n = static_cast<double>(i);

        term *= (half_x * half_x) / (n * n);

        const double next = sum + term;
        if (next == sum) {
            break;
        }

        sum = next;
    }

    return sum;
}

} // namespace Grain::Functions