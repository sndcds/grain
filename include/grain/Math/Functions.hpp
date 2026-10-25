#pragma once

namespace Grain::Functions {

/// Normalized Gaussian kernel.
///
/// Returns
///     exp(-x² / (2σ²)) / (sqrt(2π) σ)
///
/// sigma must be positive.
[[nodiscard]] auto gaussKernel(double x, double sigma) -> double;

/// Normalized sinc function.
///
/// Returns
///     sin(πx) / (πx)
///
/// with sinc(0) = 1.
[[nodiscard]] auto sinc(double x) -> double;

[[nodiscard]] auto besselI0(double x) -> double;

} // namespace Grain::Functions