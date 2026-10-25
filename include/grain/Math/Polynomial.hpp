#pragma once

#include <array>

namespace Grain::Polynomial {

[[nodiscard]] auto solveQuadratic(
    double a,
    double b,
    double c,
    std::array<double, 2>& out_values
) -> int32_t;

[[nodiscard]] auto solveCubic(
    double a,
    double b,
    double c,
    double d,
    std::array<double, 3>& out_values
) -> int32_t;

[[nodiscard]] auto solveCubicBezier(
    double p0,
    double p1,
    double p2,
    double p3,
    double p,
    std::array<double, 3>& out_values
) -> int32_t;
}