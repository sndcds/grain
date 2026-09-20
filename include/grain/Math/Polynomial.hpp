#pragma once

#include <cstdint>

namespace Grain::Polynomial {

[[nodiscard]]
int32_t solveQuadratic(
    double a,
    double b,
    double c,
    double (&outValues)[2]
) noexcept;

[[nodiscard]]
int32_t solveCubic(
    double a,
    double b,
    double c,
    double d,
    double (&outValues)[3]
) noexcept;

[[nodiscard]]
int32_t solveCubicBezier(
    double p0,
    double p1,
    double p2,
    double p3,
    double p,
    double (&outValues)[3]
) noexcept;

}