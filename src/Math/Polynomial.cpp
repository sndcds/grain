#include <algorithm>
#include <grain/Math/Polynomial.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <utility>

namespace Grain::Polynomial {

namespace {

constexpr double kEpsilon = 1e-12;
constexpr double kTwoPi = 2.0 * std::numbers::pi;

} // namespace

static void _pushRoot(
    std::array<double, 3>& out_values,
    std::size_t& count,
    double root
) noexcept {
    if (!std::isfinite(root)) {
        return;
    }

    for (std::size_t i = 0; i < count; ++i) {
        if (std::abs(out_values[i] - root) <= kEpsilon) {
            return;
        }
    }

    if (count < out_values.size()) {
        out_values[count++] = root;
    }
}


auto solveQuadratic(
    const double a,
    const double b,
    const double c,
    std::array<double, 2>& out_values
) -> int32_t {
    if (std::abs(a) <= kEpsilon) {
        if (std::abs(b) <= kEpsilon) {
            return 0;
        }

        out_values[0] = -c / b;
        return 1;
    }

    const double discriminant = (b * b) - (4.0 * a * c);

    if (discriminant < -kEpsilon) {
        return 0;
    }

    if (std::abs(discriminant) <= kEpsilon) {
        out_values[0] = -b / (2.0 * a);
        return 1;
    }

    const double sqrtDiscriminant = std::sqrt(discriminant);

    // Numerically more stable than the direct quadratic formula.
    const double q = -0.5 * (
        b + std::copysign(sqrtDiscriminant, b)
    );

    if (std::abs(q) <= kEpsilon) {
        out_values[0] =
            (-b - sqrtDiscriminant) / (2.0 * a);

        out_values[1] =
            (-b + sqrtDiscriminant) / (2.0 * a);

        if (out_values[1] < out_values[0]) {
            std::swap(out_values[0], out_values[1]);
        }

        return 2;
    }

    out_values[0] = q / a;
    out_values[1] = c / q;

    if (out_values[1] < out_values[0]) {
        std::swap(out_values[0], out_values[1]);
    }

    return 2;
}


auto solveCubic(
    const double a,
    const double b,
    const double c,
    const double d,
    std::array<double, 3>& out_values
) -> int32_t {
    // Degenerate cubic -> quadratic.
    if (std::abs(a) <= kEpsilon) {
        std::array<double, 2>quadraticRoots{};

        const int32_t count = solveQuadratic(b, c, d, quadraticRoots);

        for (int32_t i = 0; i < count; ++i) {
            out_values[i] = quadraticRoots[i];
        }

        return count;
    }

    // Normalize:
    //
    // x³ + A x² + B x + C = 0
    //
    const double A = b / a;
    const double B = c / a;
    const double C = d / a;

    // Convert to depressed cubic:
    //
    // y³ + p y + q = 0
    //
    // with x = y - A / 3.
    const double A2 = A * A;
    const double p = B - (A2 / 3.0);
    const double q = (2.0 * A2 * A / 27.0) - (A * B / 3.0) + C;

    const double discriminant = (q * q / 4.0) + (p * p * p / 27.0);

    const double offset = A / 3.0;

    // One distinct real root.
    if (discriminant > kEpsilon) {
        const double sqrtDiscriminant =
            std::sqrt(discriminant);

        const double u = std::cbrt((-q / 2.0) + sqrtDiscriminant);
        const double v = std::cbrt((-q / 2.0) - sqrtDiscriminant);

        out_values[0] = u + v - offset;

        return 1;
    }

    // Multiple real roots.
    if (std::abs(discriminant) <= kEpsilon) {
        // Triple root.
        if (std::abs(p) <= kEpsilon &&
            std::abs(q) <= kEpsilon) {

            out_values[0] = -offset;
            return 1;
        }

        // One simple root and one double root.
        const double u = std::cbrt(-q / 2.0);
        const double root1 = (2.0 * u) - offset;
        const double root2 = -u - offset;

        out_values[0] = root1;
        out_values[1] = root2;

        if (out_values[1] < out_values[0]) {
            std::swap(out_values[0], out_values[1]);
        }

        return 2;
    }

    // Three distinct real roots.
    //
    // Here discriminant < 0, which implies p < 0.
    const double radius = 2.0 * std::sqrt(-p / 3.0);
    double cosine = (3.0 * q / (2.0 * p)) * std::sqrt(-3.0 / p);

    // Protect acos() from small floating-point excursions.
    cosine = std::clamp(cosine, -1.0, 1.0);
    const double theta = std::acos(cosine);

    out_values[0] = (radius * std::cos(theta / 3.0)) - offset;
    out_values[1] = (radius * std::cos((theta + kTwoPi) / 3.0)) - offset;
    out_values[2] =(radius * std::cos((theta + 2.0 * kTwoPi) / 3.0)) - offset;

    std::ranges::sort(out_values);

    return 3;
}


auto solveCubicBezier(
    const double p0,
    const double p1,
    const double p2,
    const double p3,
    const double p,
    std::array<double, 3>& out_values
) -> int32_t {
    /*
     * Cubic Bézier:
     *
     * B(t) =
     *     (1-t)^3 p0
     *   + 3(1-t)^2 t p1
     *   + 3(1-t)t^2 p2
     *   + t^3 p3
     *
     * We solve:
     *
     *     B(t) - p = 0
     *
     * which gives:
     *
     *     A t³ + B t² + C t + D = 0
     */

    const double a = -p0 + (3.0 * p1) - (3.0 * p2) + p3;
    const double b = (3.0 * p0) - (6.0 * p1) + (3.0 * p2);
    const double c = (-3.0 * p0) + (3.0 * p1);
    const double d = p0 - p;

    std::array<double, 3>roots{};

    const int32_t rootCount = solveCubic(a, b, c, d, roots);

    std::size_t count = 0;

    for (int32_t i = 0; i < rootCount; ++i) {
        const double root = roots[i];

        // Bézier parameter must be in [0, 1].
        if (root < -kEpsilon ||
            root > 1.0 + kEpsilon) {
            continue;
        }

        // Remove tiny numerical excursions outside [0, 1].
        const double clampedRoot = std::clamp(root, 0.0, 1.0);

        _pushRoot(out_values, count, clampedRoot);
    }

    std::ranges::sort(out_values.begin(), out_values.begin() + count);

    return static_cast<std::int32_t>(count);
}

} // namespace Grain::Polynomial