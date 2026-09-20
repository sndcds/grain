#include <grain/Math/Polynomial.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace Grain::Polynomial {

namespace {

constexpr double kEpsilon = 1e-12;
constexpr double kTwoPi = 2.0 * std::numbers::pi;

} // namespace

void _pushRoot(
    double (&outValues)[3],
    int32_t& count,
    const double root
) noexcept
{
    if (!std::isfinite(root)) {
        return;
    }

    for (int32_t i = 0; i < count; ++i) {
        if (std::abs(outValues[i] - root) <= kEpsilon) {
            return;
        }
    }

    if (count < 3) {
        outValues[count++] = root;
    }
}


int32_t solveQuadratic(
    const double a,
    const double b,
    const double c,
    double (&outValues)[2]
) noexcept
{
    if (std::abs(a) <= kEpsilon) {
        if (std::abs(b) <= kEpsilon) {
            return 0;
        }

        outValues[0] = -c / b;
        return 1;
    }

    const double discriminant = b * b - 4.0 * a * c;

    if (discriminant < -kEpsilon) {
        return 0;
    }

    if (std::abs(discriminant) <= kEpsilon) {
        outValues[0] = -b / (2.0 * a);
        return 1;
    }

    const double sqrtDiscriminant = std::sqrt(discriminant);

    // Numerically more stable than the direct quadratic formula.
    const double q = -0.5 * (
        b + std::copysign(sqrtDiscriminant, b)
    );

    if (std::abs(q) <= kEpsilon) {
        outValues[0] =
            (-b - sqrtDiscriminant) / (2.0 * a);

        outValues[1] =
            (-b + sqrtDiscriminant) / (2.0 * a);

        if (outValues[1] < outValues[0]) {
            std::swap(outValues[0], outValues[1]);
        }

        return 2;
    }

    outValues[0] = q / a;
    outValues[1] = c / q;

    if (outValues[1] < outValues[0]) {
        std::swap(outValues[0], outValues[1]);
    }

    return 2;
}


int32_t solveCubic(
    const double a,
    const double b,
    const double c,
    const double d,
    double (&outValues)[3]
) noexcept
{
    // Degenerate cubic -> quadratic.
    if (std::abs(a) <= kEpsilon) {
        double quadraticRoots[2]{};

        const int32_t count =
            solveQuadratic(b, c, d, quadraticRoots);

        for (int32_t i = 0; i < count; ++i) {
            outValues[i] = quadraticRoots[i];
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

    const double p = B - A2 / 3.0;

    const double q =
        2.0 * A2 * A / 27.0
        - A * B / 3.0
        + C;

    const double discriminant =
        q * q / 4.0
        + p * p * p / 27.0;

    const double offset = A / 3.0;

    if (discriminant > kEpsilon) {
        const double sqrtDiscriminant =
            std::sqrt(discriminant);

        const double u =
            std::cbrt(-q / 2.0 + sqrtDiscriminant);

        const double v =
            std::cbrt(-q / 2.0 - sqrtDiscriminant);

        outValues[0] = u + v - offset;
        return 1;
    }

    if (std::abs(discriminant) <= kEpsilon) {
        if (std::abs(p) <= kEpsilon &&
            std::abs(q) <= kEpsilon) {
            outValues[0] = -offset;
            return 1;
        }

        const double u = std::cbrt(-q / 2.0);

        const double root1 =
            2.0 * u - offset;

        const double root2 =
            -u - offset;

        outValues[0] = root1;
        outValues[1] = root2;

        if (outValues[1] < outValues[0]) {
            std::swap(outValues[0], outValues[1]);
        }

        return 2;
    }

    // Three distinct real roots.
    //
    // p must be negative in this case.
    const double radius =
        2.0 * std::sqrt(-p / 3.0);

    double cosine =
        (3.0 * q / (2.0 * p))
        * std::sqrt(-3.0 / p);

    // Protect acos() from small floating-point excursions.
    cosine = std::clamp(cosine, -1.0, 1.0);

    const double theta =
        std::acos(cosine);

    outValues[0] =
        radius * std::cos(theta / 3.0)
        - offset;

    outValues[1] =
        radius * std::cos((theta + kTwoPi) / 3.0)
        - offset;

    outValues[2] =
        radius * std::cos((theta + 2.0 * kTwoPi) / 3.0)
        - offset;

    std::sort(
        outValues,
        outValues + 3
    );

    return 3;
}


int32_t solveCubicBezier(
    const double p0,
    const double p1,
    const double p2,
    const double p3,
    const double p,
    double (&outValues)[3]
) noexcept
{
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
     * A t³ + B t² + C t + D = 0
     */

    const double a =
        -p0
        + 3.0 * p1
        - 3.0 * p2
        + p3;

    const double b =
        3.0 * p0
        - 6.0 * p1
        + 3.0 * p2;

    const double c =
        -3.0 * p0
        + 3.0 * p1;

    const double d =
        p0 - p;

    double roots[3]{};

    const int32_t rootCount =
        solveCubic(a, b, c, d, roots);

    int32_t count = 0;

    for (int32_t i = 0; i < rootCount; ++i) {
        const double root = roots[i];

        // Bézier parameter must be in [0, 1].
        if (root < -kEpsilon ||
            root > 1.0 + kEpsilon) {
            continue;
        }

        // Remove tiny numerical excursions outside [0, 1].
        const double clampedRoot =
            std::clamp(root, 0.0, 1.0);

        _pushRoot(
            outValues,
            count,
            clampedRoot
        );
    }

    std::sort(
        outValues,
        outValues + count
    );

    return count;
}

} // namespace Grain::Polynomial