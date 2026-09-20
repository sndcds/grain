#pragma once

namespace Grain::Functions {

/// Normalized Gaussian kernel.
///
/// Returns
///     exp(-x² / (2σ²)) / (sqrt(2π) σ)
///
/// sigma must be positive.
[[nodiscard]]
double gaussKernel(
    double x,
    double sigma
) noexcept;

/// Normalized sinc function.
///
/// Returns
///     sin(πx) / (πx)
///
/// with sinc(0) = 1.
[[nodiscard]]
double sinc(
    double x
) noexcept;

[[nodiscard]]
double besselI0(
    double x
) noexcept;

} // namespace Grain::Functions