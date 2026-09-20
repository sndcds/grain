//
// MathTests.cpp
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Math/Math.hpp>

#include <cmath>
#include <cstdint>
#include <limits>

using namespace Grain;

namespace {

constexpr double epsilon = 1.0e-12;
constexpr float epsilonFloat = 1.0e-5f;

} // namespace


//------------------------------------------------------------------------------
// Angle
//------------------------------------------------------------------------------

TEST_CASE("Math::degreesToRadians", "[Math][Angle]")
{
    CHECK(Math::degreesToRadians(0.0) == 0.0);
    CHECK_THAT(
        Math::degreesToRadians(180.0),
        Catch::Matchers::WithinAbs(Math::kPi, epsilon)
    );
    CHECK_THAT(
        Math::degreesToRadians(90.0),
        Catch::Matchers::WithinAbs(Math::kPi / 2.0, epsilon)
    );
    CHECK_THAT(
        Math::degreesToRadians(360.0),
        Catch::Matchers::WithinAbs(Math::kTau, epsilon)
    );
    CHECK_THAT(
        Math::degreesToRadians(-90.0),
        Catch::Matchers::WithinAbs(-Math::kPi / 2.0, epsilon)
    );
}


TEST_CASE("Math::radiansToDegrees", "[Math][Angle]")
{
    CHECK(Math::radiansToDegrees(0.0) == 0.0);

    CHECK_THAT(
        Math::radiansToDegrees(Math::kPi),
        Catch::Matchers::WithinAbs(180.0, epsilon)
    );

    CHECK_THAT(
        Math::radiansToDegrees(Math::kPi / 2.0),
        Catch::Matchers::WithinAbs(90.0, epsilon)
    );

    CHECK_THAT(
        Math::radiansToDegrees(Math::kTau),
        Catch::Matchers::WithinAbs(360.0, epsilon)
    );

    CHECK_THAT(
        Math::radiansToDegrees(-Math::kPi / 2.0),
        Catch::Matchers::WithinAbs(-90.0, epsilon)
    );
}


//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

TEST_CASE("Math::lerp double", "[Math][Interpolation]")
{
    CHECK(Math::lerp(0.0, 10.0, 0.0) == 0.0);
    CHECK(Math::lerp(0.0, 10.0, 1.0) == 10.0);

    CHECK_THAT(
        Math::lerp(0.0, 10.0, 0.5),
        Catch::Matchers::WithinAbs(5.0, epsilon)
    );

    CHECK_THAT(
        Math::lerp(10.0, 20.0, 0.25),
        Catch::Matchers::WithinAbs(12.5, epsilon)
    );

    // Extrapolation
    CHECK_THAT(
        Math::lerp(0.0, 10.0, -1.0),
        Catch::Matchers::WithinAbs(-10.0, epsilon)
    );

    CHECK_THAT(
        Math::lerp(0.0, 10.0, 2.0),
        Catch::Matchers::WithinAbs(20.0, epsilon)
    );
}


TEST_CASE("Math::lerp float", "[Math][Interpolation]")
{
    CHECK(Math::lerp(0.0f, 10.0f, 0.0f) == 0.0f);
    CHECK(Math::lerp(0.0f, 10.0f, 1.0f) == 10.0f);

    CHECK_THAT(
        Math::lerp(0.0f, 10.0f, 0.5f),
        Catch::Matchers::WithinAbs(5.0f, epsilonFloat)
    );
}


TEST_CASE("Math::inverseLerp double", "[Math][Interpolation]")
{
    CHECK(Math::inverseLerp(0.0, 10.0, 0.0) == 0.0);
    CHECK(Math::inverseLerp(0.0, 10.0, 10.0) == 1.0);

    CHECK_THAT(
        Math::inverseLerp(0.0, 10.0, 5.0),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::inverseLerp(10.0, 20.0, 15.0),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::inverseLerp(0.0, 10.0, -5.0),
        Catch::Matchers::WithinAbs(-0.5, epsilon)
    );

    CHECK(Math::inverseLerp(5.0, 5.0, 100.0) == 0.0);
}


TEST_CASE("Math::inverseLerp float", "[Math][Interpolation]")
{
    CHECK(Math::inverseLerp(0.0f, 10.0f, 0.0f) == 0.0f);
    CHECK(Math::inverseLerp(0.0f, 10.0f, 10.0f) == 1.0f);

    CHECK_THAT(
        Math::inverseLerp(0.0f, 10.0f, 5.0f),
        Catch::Matchers::WithinAbs(0.5f, epsilonFloat)
    );

    CHECK(Math::inverseLerp(5.0f, 5.0f, 100.0f) == 0.0f);
}


//------------------------------------------------------------------------------
// Remapping
//------------------------------------------------------------------------------

TEST_CASE("Math::remap double", "[Math][Remapping]")
{
    CHECK(Math::remap(0.0, 10.0, 0.0, 100.0, 0.0) == 0.0);
    CHECK(Math::remap(0.0, 10.0, 0.0, 100.0, 10.0) == 100.0);

    CHECK_THAT(
        Math::remap(0.0, 10.0, 0.0, 100.0, 5.0),
        Catch::Matchers::WithinAbs(50.0, epsilon)
    );

    CHECK_THAT(
        Math::remap(0.0, 100.0, 10.0, 20.0, 25.0),
        Catch::Matchers::WithinAbs(12.5, epsilon)
    );

    // Reversed output range
    CHECK_THAT(
        Math::remap(0.0, 10.0, 100.0, 0.0, 2.5),
        Catch::Matchers::WithinAbs(75.0, epsilon)
    );

    // Extrapolation
    CHECK_THAT(
        Math::remap(0.0, 10.0, 0.0, 100.0, 20.0),
        Catch::Matchers::WithinAbs(200.0, epsilon)
    );

    // Degenerate input range
    CHECK(Math::remap(5.0, 5.0, 10.0, 20.0, 100.0) == 10.0);
}


TEST_CASE("Math::remap float", "[Math][Remapping]")
{
    CHECK_THAT(
        Math::remap(0.0f, 10.0f, 0.0f, 100.0f, 5.0f),
        Catch::Matchers::WithinAbs(50.0f, epsilonFloat)
    );

    CHECK(Math::remap(5.0f, 5.0f, 10.0f, 20.0f, 100.0f) == 10.0f);
}


TEST_CASE("Math::normalize double", "[Math][Remapping]")
{
    CHECK(Math::normalize(0.0, 10.0, 0.0) == 0.0);
    CHECK(Math::normalize(0.0, 10.0, 10.0) == 1.0);

    CHECK_THAT(
        Math::normalize(0.0, 10.0, 5.0),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::normalize(10.0, 20.0, 15.0),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK(Math::normalize(5.0, 5.0, 100.0) == 0.0);
}


TEST_CASE("Math::normalize float", "[Math][Remapping]")
{
    CHECK(Math::normalize(0.0f, 10.0f, 0.0f) == 0.0f);
    CHECK(Math::normalize(0.0f, 10.0f, 10.0f) == 1.0f);

    CHECK_THAT(
        Math::normalize(0.0f, 10.0f, 5.0f),
        Catch::Matchers::WithinAbs(0.5f, epsilonFloat)
    );

    CHECK(Math::normalize(5.0f, 5.0f, 100.0f) == 0.0f);
}


TEST_CASE("Math::remapClamped double", "[Math][Remapping]")
{
    CHECK_THAT(
        Math::remapClamped(0.0, 10.0, 0.0, 100.0, 5.0),
        Catch::Matchers::WithinAbs(50.0, epsilon)
    );

    CHECK(Math::remapClamped(0.0, 10.0, 0.0, 100.0, -5.0) == 0.0);
    CHECK(Math::remapClamped(0.0, 10.0, 0.0, 100.0, 15.0) == 100.0);

    // Reversed output range
    CHECK_THAT(
        Math::remapClamped(0.0, 10.0, 100.0, 0.0, 5.0),
        Catch::Matchers::WithinAbs(50.0, epsilon)
    );

    CHECK(Math::remapClamped(0.0, 10.0, 100.0, 0.0, -5.0) == 100.0);
    CHECK(Math::remapClamped(0.0, 10.0, 100.0, 0.0, 15.0) == 0.0);
}


TEST_CASE("Math::remapClamped float", "[Math][Remapping]")
{
    CHECK_THAT(
        Math::remapClamped(0.0f, 10.0f, 0.0f, 100.0f, 5.0f),
        Catch::Matchers::WithinAbs(50.0f, epsilonFloat)
    );

    CHECK(Math::remapClamped(0.0f, 10.0f, 0.0f, 100.0f, -5.0f) == 0.0f);
    CHECK(Math::remapClamped(0.0f, 10.0f, 0.0f, 100.0f, 15.0f) == 100.0f);
}


//------------------------------------------------------------------------------
// Step functions
//------------------------------------------------------------------------------

TEST_CASE("Math::unitStep double", "[Math][Step]")
{
    CHECK(Math::unitStep(5.0, 0.0) == 0.0);
    CHECK(Math::unitStep(5.0, 4.999) == 0.0);
    CHECK(Math::unitStep(5.0, 5.0) == 1.0);
    CHECK(Math::unitStep(5.0, 10.0) == 1.0);
}


TEST_CASE("Math::unitStep float", "[Math][Step]")
{
    CHECK(Math::unitStep(5.0f, 4.0f) == 0.0f);
    CHECK(Math::unitStep(5.0f, 5.0f) == 1.0f);
    CHECK(Math::unitStep(5.0f, 6.0f) == 1.0f);
}


TEST_CASE("Math::unitStep with range double", "[Math][Step]")
{
    CHECK(Math::unitStep(10.0, 2.0, 7.0) == 0.0);

    CHECK_THAT(
        Math::unitStep(10.0, 2.0, 8.0),
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );

    CHECK_THAT(
        Math::unitStep(10.0, 2.0, 9.0),
        Catch::Matchers::WithinAbs(0.25, epsilon)
    );

    CHECK_THAT(
        Math::unitStep(10.0, 2.0, 10.0),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::unitStep(10.0, 2.0, 11.0),
        Catch::Matchers::WithinAbs(0.75, epsilon)
    );

    CHECK(Math::unitStep(10.0, 2.0, 12.0) == 1.0);
}


TEST_CASE("Math::unitStep with range float", "[Math][Step]")
{
    CHECK(Math::unitStep(10.0f, 2.0f, 7.0f) == 0.0f);

    CHECK_THAT(
        Math::unitStep(10.0f, 2.0f, 9.0f),
        Catch::Matchers::WithinAbs(0.25f, epsilonFloat)
    );

    CHECK_THAT(
        Math::unitStep(10.0f, 2.0f, 10.0f),
        Catch::Matchers::WithinAbs(0.5f, epsilonFloat)
    );

    CHECK(Math::unitStep(10.0f, 2.0f, 12.0f) == 1.0f);
}


//------------------------------------------------------------------------------
// Smooth interpolation
//------------------------------------------------------------------------------

TEST_CASE("Math::smoothStep double", "[Math][SmoothStep]")
{
    CHECK(Math::smoothStep(0.0) == 0.0);
    CHECK(Math::smoothStep(1.0) == 1.0);

    CHECK_THAT(
        Math::smoothStep(0.5),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::smoothStep(0.25),
        Catch::Matchers::WithinAbs(0.15625, epsilon)
    );

    CHECK_THAT(
        Math::smoothStep(0.0, 100.0, 0.5),
        Catch::Matchers::WithinAbs(50.0, epsilon)
    );
}


TEST_CASE("Math::smoothStep float", "[Math][SmoothStep]")
{
    CHECK(Math::smoothStep(0.0f) == 0.0f);
    CHECK(Math::smoothStep(1.0f) == 1.0f);

    CHECK_THAT(
        Math::smoothStep(0.5f),
        Catch::Matchers::WithinAbs(0.5f, epsilonFloat)
    );

    CHECK_THAT(
        Math::smoothStep(0.0f, 100.0f, 0.5f),
        Catch::Matchers::WithinAbs(50.0f, epsilonFloat)
    );
}


TEST_CASE("Math::smootherStep double", "[Math][SmoothStep]")
{
    CHECK(Math::smootherStep(0.0) == 0.0);
    CHECK(Math::smootherStep(1.0) == 1.0);

    CHECK_THAT(
        Math::smootherStep(0.5),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::smootherStep(0.25),
        Catch::Matchers::WithinAbs(0.103515625, epsilon)
    );

    CHECK_THAT(
        Math::smootherStep(0.0, 100.0, 0.5),
        Catch::Matchers::WithinAbs(50.0, epsilon)
    );
}


TEST_CASE("Math::smootherStep float", "[Math][SmoothStep]")
{
    CHECK(Math::smootherStep(0.0f) == 0.0f);
    CHECK(Math::smootherStep(1.0f) == 1.0f);

    CHECK_THAT(
        Math::smootherStep(0.5f),
        Catch::Matchers::WithinAbs(0.5f, epsilonFloat)
    );

    CHECK_THAT(
        Math::smootherStep(0.0f, 100.0f, 0.5f),
        Catch::Matchers::WithinAbs(50.0f, epsilonFloat)
    );
}


//------------------------------------------------------------------------------
// Wrapping
//------------------------------------------------------------------------------

TEST_CASE("Math::wrap", "[Math][Wrapping]")
{
    CHECK(Math::wrap(0.0) == 0.0);
    CHECK(Math::wrap(1.0) == 0.0);
    CHECK(Math::wrap(2.0) == 0.0);

    CHECK_THAT(
        Math::wrap(0.25),
        Catch::Matchers::WithinAbs(0.25, epsilon)
    );

    CHECK_THAT(
        Math::wrap(1.25),
        Catch::Matchers::WithinAbs(0.25, epsilon)
    );

    CHECK_THAT(
        Math::wrap(-0.25),
        Catch::Matchers::WithinAbs(0.75, epsilon)
    );
}


TEST_CASE("Math::wrap with range", "[Math][Wrapping]")
{
    CHECK(Math::wrap(0.0, 0.0, 10.0) == 0.0);
    CHECK(Math::wrap(10.0, 0.0, 10.0) == 0.0);
    CHECK(Math::wrap(20.0, 0.0, 10.0) == 0.0);

    CHECK_THAT(
        Math::wrap(2.5, 0.0, 10.0),
        Catch::Matchers::WithinAbs(2.5, epsilon)
    );

    CHECK_THAT(
        Math::wrap(12.5, 0.0, 10.0),
        Catch::Matchers::WithinAbs(2.5, epsilon)
    );

    CHECK_THAT(
        Math::wrap(-2.5, 0.0, 10.0),
        Catch::Matchers::WithinAbs(7.5, epsilon)
    );

    CHECK_THAT(
        Math::wrap(12.5, 10.0, 20.0),
        Catch::Matchers::WithinAbs(12.5, epsilon)
    );
}


TEST_CASE("Math::pingPong", "[Math][Wrapping]")
{
    CHECK(Math::pingPong(0.0) == 0.0);
    CHECK(Math::pingPong(1.0) == 1.0);
    CHECK(Math::pingPong(2.0) == 0.0);
    CHECK(Math::pingPong(3.0) == 1.0);

    CHECK_THAT(
        Math::pingPong(0.5),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::pingPong(1.5),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::pingPong(-0.5),
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    CHECK_THAT(
        Math::pingPong(-1.0),
        Catch::Matchers::WithinAbs(1.0, epsilon)
    );
}


//------------------------------------------------------------------------------
// Ratios
//------------------------------------------------------------------------------

TEST_CASE("Math::quotient", "[Math][Ratios]")
{
    CHECK(Math::quotient(10.0, 2.0) == 5.0);

    CHECK_THAT(
        Math::quotient(7.0, 2.0),
        Catch::Matchers::WithinAbs(3.5, epsilon)
    );

    CHECK(std::isnan(Math::quotient(1.0, 0.0)));
    CHECK(std::isnan(Math::quotient(1.0, -1.0)));
}


TEST_CASE("Math::percent", "[Math][Ratios]")
{
    CHECK(Math::percent(50.0, 100.0) == 50.0);

    CHECK_THAT(
        Math::percent(1.0, 3.0),
        Catch::Matchers::WithinAbs(33.333333333333, epsilon)
    );

    CHECK(Math::percent(0.0, 100.0) == 0.0);

    CHECK(std::isnan(Math::percent(1.0, 0.0)));
    CHECK(std::isnan(Math::percent(1.0, -1.0)));
}


TEST_CASE("Math::xPerY", "[Math][Ratios]")
{
    CHECK(Math::xPerY(10.0, 2.0) == 5.0);

    CHECK_THAT(
        Math::xPerY(7.0, 2.0),
        Catch::Matchers::WithinAbs(3.5, epsilon)
    );

    CHECK(std::isnan(Math::xPerY(1.0, 0.0)));
    CHECK(std::isnan(Math::xPerY(1.0, -1.0)));
}


TEST_CASE("Math::xPerYInt", "[Math][Ratios]")
{
    CHECK(Math::xPerYInt(10.0, 2.0) == 5);
    CHECK(Math::xPerYInt(11.0, 2.0) == 6);
    CHECK(Math::xPerYInt(10.0, 3.0) == 3);
    CHECK(Math::xPerYInt(11.0, 3.0) == 4);

    CHECK(Math::xPerYInt(1.0, 0.0) == 0);
    CHECK(Math::xPerYInt(1.0, -1.0) == 0);
}


//------------------------------------------------------------------------------
// Integer / discrete math
//------------------------------------------------------------------------------

TEST_CASE("Math::greatestCommonDivisor", "[Math][Integer]")
{
    CHECK(Math::greatestCommonDivisor(0, 0) == 0);
    CHECK(Math::greatestCommonDivisor(12, 8) == 4);
    CHECK(Math::greatestCommonDivisor(8, 12) == 4);

    CHECK(Math::greatestCommonDivisor(17, 13) == 1);
    CHECK(Math::greatestCommonDivisor(100, 25) == 25);

    CHECK(Math::greatestCommonDivisor(-12, 8) == 4);
    CHECK(Math::greatestCommonDivisor(12, -8) == 4);
    CHECK(Math::greatestCommonDivisor(-12, -8) == 4);

    CHECK(Math::greatestCommonDivisor(0, 10) == 10);
    CHECK(Math::greatestCommonDivisor(10, 0) == 10);
}


TEST_CASE("Math::factorial", "[Math][Integer]")
{
    CHECK(Math::factorial(-1) == 0);
    CHECK(Math::factorial(0) == 1);
    CHECK(Math::factorial(1) == 1);
    CHECK(Math::factorial(2) == 2);
    CHECK(Math::factorial(3) == 6);
    CHECK(Math::factorial(4) == 24);
    CHECK(Math::factorial(5) == 120);
    CHECK(Math::factorial(10) == 3628800);

    CHECK(Math::factorial(20) == 2432902008176640000LL);

    // Saturates rather than overflowing.
    CHECK(Math::factorial(21) == std::numeric_limits<int64_t>::max());
}


TEST_CASE("Math::sumN", "[Math][Integer]")
{
    CHECK(Math::sumN(-1) == 0);
    CHECK(Math::sumN(0) == 0);
    CHECK(Math::sumN(1) == 1);
    CHECK(Math::sumN(2) == 3);
    CHECK(Math::sumN(3) == 6);
    CHECK(Math::sumN(10) == 55);
    CHECK(Math::sumN(100) == 5050);
}


TEST_CASE("Math::roundToNearestPowerOfTwo", "[Math][Integer]")
{
    CHECK(Math::roundToNearestPowerOfTwo(-1.0) == 0);
    CHECK(Math::roundToNearestPowerOfTwo(0.0) == 0);

    CHECK(Math::roundToNearestPowerOfTwo(1.0) == 1);
    CHECK(Math::roundToNearestPowerOfTwo(2.0) == 2);
    CHECK(Math::roundToNearestPowerOfTwo(4.0) == 4);
    CHECK(Math::roundToNearestPowerOfTwo(8.0) == 8);

    CHECK(Math::roundToNearestPowerOfTwo(3.0) == 4);
    CHECK(Math::roundToNearestPowerOfTwo(5.0) == 4);
    CHECK(Math::roundToNearestPowerOfTwo(6.0) == 8);
    CHECK(Math::roundToNearestPowerOfTwo(7.0) == 8);

    CHECK(Math::roundToNearestPowerOfTwo(15.0) == 16);
    CHECK(Math::roundToNearestPowerOfTwo(17.0) == 16);
}


TEST_CASE("Math::stepsToReachAtOrAfterInt", "[Math][Integer]")
{
    CHECK(Math::stepsToReachAtOrAfterInt(0, 1, 0) == 0);
    CHECK(Math::stepsToReachAtOrAfterInt(0, 1, 1) == 1);
    CHECK(Math::stepsToReachAtOrAfterInt(0, 1, 10) == 10);

    CHECK(Math::stepsToReachAtOrAfterInt(0, 3, 1) == 1);
    CHECK(Math::stepsToReachAtOrAfterInt(0, 3, 3) == 1);
    CHECK(Math::stepsToReachAtOrAfterInt(0, 3, 4) == 2);
    CHECK(Math::stepsToReachAtOrAfterInt(0, 3, 10) == 4);

    CHECK(Math::stepsToReachAtOrAfterInt(10, 3, 5) == 0);

    CHECK(Math::stepsToReachAtOrAfterInt(0, 0, 10) == 0);
    CHECK(Math::stepsToReachAtOrAfterInt(0, -1, 10) == 0);
}


TEST_CASE("Math::stepsToReachAtOrAfter", "[Math][Integer]")
{
    CHECK(Math::stepsToReachAtOrAfter(0.0, 1.0, 0.0) == 0);
    CHECK(Math::stepsToReachAtOrAfter(0.0, 1.0, 1.0) == 1);
    CHECK(Math::stepsToReachAtOrAfter(0.0, 1.0, 10.0) == 10);

    CHECK(Math::stepsToReachAtOrAfter(0.0, 3.0, 1.0) == 1);
    CHECK(Math::stepsToReachAtOrAfter(0.0, 3.0, 3.0) == 1);
    CHECK(Math::stepsToReachAtOrAfter(0.0, 3.0, 4.0) == 2);
    CHECK(Math::stepsToReachAtOrAfter(0.0, 3.0, 10.0) == 4);

    CHECK(Math::stepsToReachAtOrAfter(10.0, 1.0, 5.0) == 0);

    CHECK(Math::stepsToReachAtOrAfter(0.0, 0.0, 10.0) == 0);
    CHECK(Math::stepsToReachAtOrAfter(0.0, -1.0, 10.0) == 0);
}


//------------------------------------------------------------------------------
// Powers of two
//------------------------------------------------------------------------------

TEST_CASE("Math::nextLog2", "[Math][PowersOfTwo]")
{
    CHECK(Math::nextLog2(-1) == 0);
    CHECK(Math::nextLog2(0) == 0);
    CHECK(Math::nextLog2(1) == 0);

    CHECK(Math::nextLog2(2) == 1);
    CHECK(Math::nextLog2(3) == 2);
    CHECK(Math::nextLog2(4) == 2);
    CHECK(Math::nextLog2(5) == 3);
    CHECK(Math::nextLog2(7) == 3);
    CHECK(Math::nextLog2(8) == 3);
    CHECK(Math::nextLog2(9) == 4);

    CHECK(Math::nextLog2(16) == 4);
    CHECK(Math::nextLog2(17) == 5);
}


TEST_CASE("Math::isPowerOfTwo", "[Math][PowersOfTwo]")
{
    CHECK_FALSE(Math::isPowerOfTwo(-1));
    CHECK_FALSE(Math::isPowerOfTwo(0));

    CHECK(Math::isPowerOfTwo(1));
    CHECK(Math::isPowerOfTwo(2));
    CHECK_FALSE(Math::isPowerOfTwo(3));
    CHECK(Math::isPowerOfTwo(4));
    CHECK_FALSE(Math::isPowerOfTwo(5));
    CHECK_FALSE(Math::isPowerOfTwo(6));
    CHECK_FALSE(Math::isPowerOfTwo(7));
    CHECK(Math::isPowerOfTwo(8));
    CHECK(Math::isPowerOfTwo(16));
    CHECK_FALSE(Math::isPowerOfTwo(17));
    CHECK(Math::isPowerOfTwo(1024));
}


TEST_CASE("Math::log2IfPowerOfTwo", "[Math][PowersOfTwo]")
{
    CHECK(Math::log2IfPowerOfTwo(-1) == -1);
    CHECK(Math::log2IfPowerOfTwo(0) == -1);

    CHECK(Math::log2IfPowerOfTwo(1) == 0);
    CHECK(Math::log2IfPowerOfTwo(2) == 1);
    CHECK(Math::log2IfPowerOfTwo(4) == 2);
    CHECK(Math::log2IfPowerOfTwo(8) == 3);
    CHECK(Math::log2IfPowerOfTwo(16) == 4);
    CHECK(Math::log2IfPowerOfTwo(1024) == 10);

    CHECK(Math::log2IfPowerOfTwo(3) == -1);
    CHECK(Math::log2IfPowerOfTwo(5) == -1);
    CHECK(Math::log2IfPowerOfTwo(6) == -1);
    CHECK(Math::log2IfPowerOfTwo(7) == -1);
}


TEST_CASE("Math::nextPowerOfTwo", "[Math][PowersOfTwo]")
{
    CHECK(Math::nextPowerOfTwo(-10) == 1);
    CHECK(Math::nextPowerOfTwo(0) == 1);
    CHECK(Math::nextPowerOfTwo(1) == 1);

    CHECK(Math::nextPowerOfTwo(2) == 2);
    CHECK(Math::nextPowerOfTwo(3) == 4);
    CHECK(Math::nextPowerOfTwo(4) == 4);
    CHECK(Math::nextPowerOfTwo(5) == 8);
    CHECK(Math::nextPowerOfTwo(7) == 8);
    CHECK(Math::nextPowerOfTwo(8) == 8);
    CHECK(Math::nextPowerOfTwo(9) == 16);

    CHECK(Math::nextPowerOfTwo(15) == 16);
    CHECK(Math::nextPowerOfTwo(16) == 16);
    CHECK(Math::nextPowerOfTwo(17) == 32);

    CHECK(Math::nextPowerOfTwo(1024) == 1024);
    CHECK(Math::nextPowerOfTwo(1025) == 2048);
}


//------------------------------------------------------------------------------
// Trigonometry
//------------------------------------------------------------------------------

TEST_CASE("Math::secant", "[Math][Trigonometry]")
{
    CHECK_THAT(
        Math::secant(0.0),
        Catch::Matchers::WithinAbs(1.0, epsilon)
    );

    CHECK_THAT(
        Math::secant(60.0),
        Catch::Matchers::WithinAbs(2.0, epsilon)
    );

    CHECK_THAT(
        Math::secant(-60.0),
        Catch::Matchers::WithinAbs(2.0, epsilon)
    );

    CHECK_THAT(
        Math::secant(180.0),
        Catch::Matchers::WithinAbs(-1.0, epsilon)
    );

    // cos(90°) is not exactly zero on typical floating-point
    // implementations, so test that the result is very large
    // rather than requiring NaN.
    CHECK(std::abs(Math::secant(90.0)) > 1.0e12);
}


//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

TEST_CASE("Math constants", "[Math]")
{
    CHECK_THAT(
        Math::kPi,
        Catch::Matchers::WithinAbs(3.14159265358979323846, epsilon)
    );

    CHECK_THAT(
        Math::kTau,
        Catch::Matchers::WithinAbs(6.28318530717958647692, epsilon)
    );

    CHECK(Math::kEpsilon ==
          std::numeric_limits<double>::epsilon());

    CHECK(Math::kEpsilonFloat ==
          std::numeric_limits<float>::epsilon());
}