#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Math/Polynomial.hpp>

#include <algorithm>
#include <cmath>

namespace {

using Catch::Matchers::WithinAbs;

void requireRoot(double actual, double expected, double epsilon = 1e-9)
{
    REQUIRE_THAT(actual, WithinAbs(expected, epsilon));
}

void requireRoots(
    const double (&actual)[3],
    int32_t count,
    std::initializer_list<double> expected,
    double epsilon = 1e-9
) {
    REQUIRE(count == static_cast<int32_t>(expected.size()));

    std::size_t i = 0;
    for (const double value : expected) {
        requireRoot(actual[i++], value, epsilon);
    }
}

void requireRoots(
    const double (&actual)[2],
    int32_t count,
    std::initializer_list<double> expected,
    double epsilon = 1e-9
) {
    REQUIRE(count == static_cast<int32_t>(expected.size()));

    std::size_t i = 0;
    for (const double value : expected) {
        requireRoot(actual[i++], value, epsilon);
    }
}

} // namespace


TEST_CASE("solveQuadratic", "[Polynomial]")
{
    double roots[2]{};

    SECTION("two distinct real roots")
    {
        // x² - 5x + 6 = 0
        // roots: 2, 3
        const auto count = Grain::Polynomial::solveQuadratic(
            1.0, -5.0, 6.0, roots);

        REQUIRE(count == 2);
        requireRoots(roots, count, {2.0, 3.0});
    }

    SECTION("two negative real roots")
    {
        // x² + 5x + 6 = 0
        // roots: -3, -2
        const auto count = Grain::Polynomial::solveQuadratic(
            1.0, 5.0, 6.0, roots);

        REQUIRE(count == 2);
        requireRoots(roots, count, {-3.0, -2.0});
    }

    SECTION("one repeated real root")
    {
        // x² - 4x + 4 = 0
        // root: 2
        const auto count = Grain::Polynomial::solveQuadratic(
            1.0, -4.0, 4.0, roots);

        REQUIRE(count == 1);
        requireRoots(roots, count, {2.0});
    }

    SECTION("no real roots")
    {
        // x² + 1 = 0
        const auto count = Grain::Polynomial::solveQuadratic(
            1.0, 0.0, 1.0, roots);

        REQUIRE(count == 0);
    }

    SECTION("linear equation")
    {
        // 2x - 6 = 0
        // root: 3
        const auto count = Grain::Polynomial::solveQuadratic(
            0.0, 2.0, -6.0, roots);

        REQUIRE(count == 1);
        requireRoots(roots, count, {3.0});
    }

    SECTION("constant non-zero equation")
    {
        // 5 = 0
        const auto count = Grain::Polynomial::solveQuadratic(
            0.0, 0.0, 5.0, roots);

        REQUIRE(count == 0);
    }

    SECTION("zero polynomial")
    {
        const auto count = Grain::Polynomial::solveQuadratic(
            0.0, 0.0, 0.0, roots);

        // Adjust this expectation if your implementation defines
        // the zero polynomial differently.
        REQUIRE(count == 0);
    }

    SECTION("fractional roots")
    {
        // 4x² - 4x - 3 = 0
        // roots: -0.5, 1.5
        const auto count = Grain::Polynomial::solveQuadratic(
            4.0, -4.0, -3.0, roots);

        REQUIRE(count == 2);
        requireRoots(roots, count, {-0.5, 1.5});
    }
}


TEST_CASE("solveCubic", "[Polynomial]")
{
    double roots[3]{};

    SECTION("three distinct real roots")
    {
        // x³ - 6x² + 11x - 6 = 0
        // roots: 1, 2, 3
        const auto count = Grain::Polynomial::solveCubic(
            1.0, -6.0, 11.0, -6.0, roots);

        REQUIRE(count == 3);
        requireRoots(roots, count, {1.0, 2.0, 3.0});
    }

    SECTION("three negative real roots")
    {
        // x³ + 6x² + 11x + 6 = 0
        // roots: -3, -2, -1
        const auto count = Grain::Polynomial::solveCubic(
            1.0, 6.0, 11.0, 6.0, roots);

        REQUIRE(count == 3);
        requireRoots(roots, count, {-3.0, -2.0, -1.0});
    }

    SECTION("one real root")
    {
        // x³ - x + 1 = 0
        // approximately -1.324717957
        const auto count = Grain::Polynomial::solveCubic(
            1.0, 0.0, -1.0, 1.0, roots);

        REQUIRE(count == 1);
        requireRoot(roots[0], -1.324717957244746, 1e-9);
    }

    SECTION("three repeated roots")
    {
        // (x - 2)³ = x³ - 6x² + 12x - 8
        const auto count = Grain::Polynomial::solveCubic(
            1.0, -6.0, 12.0, -8.0, roots);

        // Most polynomial solvers report this as one distinct root.
        REQUIRE(count == 1);
        requireRoot(roots[0], 2.0);
    }

    SECTION("one simple and one repeated root")
    {
        // (x - 1)²(x + 2)
        // = x³ - 3x + 2
        // roots: -2, 1
        const auto count = Grain::Polynomial::solveCubic(
            1.0, 0.0, -3.0, 2.0, roots);

        REQUIRE(count == 2);
        requireRoots(roots, count, {-2.0, 1.0});
    }

    SECTION("no real roots")
    {
        // x³ + x + 1 has one real root, so use:
        // x³ + 1 = 0 actually has one real root.
        //
        // Every real cubic with real coefficients has at least
        // one real root. Therefore count must never be zero
        // for a non-degenerate cubic.
        const auto count = Grain::Polynomial::solveCubic(
            1.0, 0.0, 1.0, 1.0, roots);

        REQUIRE(count == 1);
    }

    SECTION("degenerates to quadratic")
    {
        // x² - 5x + 6 = 0
        // roots: 2, 3
        const auto count = Grain::Polynomial::solveCubic(
            0.0, 1.0, -5.0, 6.0, roots);

        REQUIRE(count == 2);
        requireRoots(roots, count, {2.0, 3.0});
    }

    SECTION("degenerates to linear")
    {
        // 2x - 6 = 0
        const auto count = Grain::Polynomial::solveCubic(
            0.0, 0.0, 2.0, -6.0, roots);

        REQUIRE(count == 1);
        requireRoot(roots[0], 3.0);
    }
}


TEST_CASE("solveCubicBezier", "[Polynomial]")
{
    double roots[3]{};

    SECTION("linear Bezier")
    {
        // All control points form y = x.
        //
        // B(t) = t
        // B(t) = 0.25 -> t = 0.25
        const auto count = Grain::Polynomial::solveCubicBezier(
            0.0,
            1.0 / 3.0,
            2.0 / 3.0,
            1.0,
            0.25,
            roots);

        REQUIRE(count == 1);
        requireRoot(roots[0], 0.25);
    }

    SECTION("standard ease-in-out curve")
    {
        // Cubic Bezier:
        // P0 = 0
        // P1 = 0
        // P2 = 1
        // P3 = 1
        //
        // B(t) = 3t² - 2t³
        //
        // B(0.5) = 0.5
        const auto count = Grain::Polynomial::solveCubicBezier(
            0.0,
            0.0,
            1.0,
            1.0,
            0.5,
            roots);

        REQUIRE(count == 1);
        requireRoot(roots[0], 0.5);
    }

    SECTION("Bezier with known parameter")
    {
        constexpr double t = 0.25;

        constexpr double p0 = 0.0;
        constexpr double p1 = 0.2;
        constexpr double p2 = 0.8;
        constexpr double p3 = 1.0;

        constexpr double u = 1.0 - t;

        constexpr double p =
            u * u * u * p0 +
            3.0 * u * u * t * p1 +
            3.0 * u * t * t * p2 +
            t * t * t * p3;

        const auto count = Grain::Polynomial::solveCubicBezier(
            p0, p1, p2, p3, p, roots);

        REQUIRE(count >= 1);

        bool found = false;
        for (int32_t i = 0; i < count; ++i) {
            if (std::abs(roots[i] - t) <= 1e-9) {
                found = true;
                break;
            }
        }

        REQUIRE(found);
    }

    SECTION("Bezier at start")
    {
        const auto count = Grain::Polynomial::solveCubicBezier(
            0.0,
            0.25,
            0.75,
            1.0,
            0.0,
            roots);

        REQUIRE(count >= 1);

        bool found = false;
        for (int32_t i = 0; i < count; ++i) {
            if (std::abs(roots[i]) <= 1e-9) {
                found = true;
                break;
            }
        }

        REQUIRE(found);
    }

    SECTION("Bezier at end")
    {
        const auto count = Grain::Polynomial::solveCubicBezier(
            0.0,
            0.25,
            0.75,
            1.0,
            1.0,
            roots);

        REQUIRE(count >= 1);

        bool found = false;
        for (int32_t i = 0; i < count; ++i) {
            if (std::abs(roots[i] - 1.0) <= 1e-9) {
                found = true;
                break;
            }
        }

        REQUIRE(found);
    }

    SECTION("constant Bezier")
    {
        // B(t) = 0.5 for every t.
        const auto count = Grain::Polynomial::solveCubicBezier(
            0.5,
            0.5,
            0.5,
            0.5,
            0.5,
            roots);

        // Adjust according to your API's convention for
        // infinitely many solutions.
        REQUIRE(count == 0);
    }

    SECTION("target outside Bezier range")
    {
        // Monotonic Bezier from 0 to 1 cannot reach 2.
        const auto count = Grain::Polynomial::solveCubicBezier(
            0.0,
            0.25,
            0.75,
            1.0,
            2.0,
            roots);

        REQUIRE(count == 0);
    }
}