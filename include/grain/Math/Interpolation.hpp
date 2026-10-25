#pragma once

#include <cstddef>
#include <span>

namespace Grain::Interpolation {

/**
 * Cubic interpolation through four samples.
 *
 * The samples correspond to x = -1, 0, 1, 2 and `t` is the
 * interpolation position between the middle two samples:
 *
 *   t = 0 -> p1
 *   t = 1 -> p2
 */
auto cubic(double p0, double p1, double p2, double p3, double t) -> double;

/**
 * Bicubic interpolation of a 4x4 sample grid.
 *
 * Samples are supplied in row-major order:
 *
 *   p[y * 4 + x]
 *
 * `tx` and `ty` are the interpolation positions between the
 * middle samples in x and y.
 */
auto bicubic(
    std::span<const double, 16> samples,
    double tx,
    double ty
) -> double;

/**
 * Tricubic interpolation of a 4x4x4 sample grid.
 *
 * Samples are supplied with x varying fastest:
 *
 *   p[z * 16 + y * 4 + x]
 *
 * `tx`, `ty` and `tz` are the interpolation positions between
 * the middle samples in each dimension.
 */
auto tricubic(
    std::span<const double, 64> samples,
    double tx,
    double ty,
    double tz
) -> double;

/**
 * N-dimensional cubic interpolation.
 *
 * For N dimensions there are 4^N samples. Samples are stored
 * in row-major order, with the first dimension varying fastest.
 *
 * Example:
 *
 *   2D:  4^2  = 16 samples
 *   3D:  4^3  = 64 samples
 *   4D:  4^4  = 256 samples
 *
 * `dimensions` contains the size of the coordinate vector.
 * The number of samples must be exactly 4^dimensions.size().
 */
auto nCubic(
    std::span<const double> samples,
    std::span<const double> coordinates
) -> double;

} // namespace Grain::Interpolation