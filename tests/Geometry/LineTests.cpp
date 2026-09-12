#include "catch2/catch_approx.hpp"

#include <catch2/catch_test_macros.hpp>

#include <grain/Geometry/Line.hpp>

#include <cmath>
#include <limits>

using namespace Grain;

namespace {

constexpr double epsilon = 1e-10;

void REQUIRE_VEC2_NEAR(
    const Vec2d& actual,
    const Vec2d& expected,
    double margin = epsilon
)
{
    REQUIRE(actual.x == Catch::Approx(expected.x).margin(margin));
    REQUIRE(actual.y == Catch::Approx(expected.y).margin(margin));
}

void REQUIRE_LINE_NEAR(
    const Lined& actual,
    const Lined& expected,
    double margin = epsilon
)
{
    REQUIRE_VEC2_NEAR(actual.p1(), expected.p1(), margin);
    REQUIRE_VEC2_NEAR(actual.p2(), expected.p2(), margin);
}

} // namespace

//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

TEST_CASE("Line construction", "[Line]")
{
    SECTION("default")
    {
        Lined line;

        REQUIRE(line.p1() == Vec2d(0.0, 0.0));
        REQUIRE(line.p2() == Vec2d(0.0, 0.0));
    }

    SECTION("from points")
    {
        Lined line(
            Vec2d(1.0, 2.0),
            Vec2d(3.0, 4.0)
        );

        REQUIRE(line.p1() == Vec2d(1.0, 2.0));
        REQUIRE(line.p2() == Vec2d(3.0, 4.0));
    }

    SECTION("from coordinates")
    {
        Lined line(1.0, 2.0, 3.0, 4.0);

        REQUIRE(line.p1() == Vec2d(1.0, 2.0));
        REQUIRE(line.p2() == Vec2d(3.0, 4.0));
    }

    SECTION("center, length and rotation")
    {
        Lined line(
            Vec2d(10.0, 20.0),
            10.0,
            0.0
        );

        REQUIRE_VEC2_NEAR(line.p1(), Vec2d(5.0, 20.0));
        REQUIRE_VEC2_NEAR(line.p2(), Vec2d(15.0, 20.0));
        REQUIRE(line.length() == Catch::Approx(10.0));
        REQUIRE_VEC2_NEAR(line.center(), Vec2d(10.0, 20.0));
    }

    SECTION("center, length and rotation")
    {
        Lined line(
            Vec2d(0.0, 0.0),
            10.0,
            90.0
        );

        REQUIRE_VEC2_NEAR(line.p1(), Vec2d(0.0, -5.0));
        REQUIRE_VEC2_NEAR(line.p2(), Vec2d(0.0, 5.0));
    }
}

//------------------------------------------------------------------------------
// Comparison
//------------------------------------------------------------------------------

TEST_CASE("Line comparison", "[Line]")
{
    const Lined a(
        Vec2d(1.0, 2.0),
        Vec2d(3.0, 4.0)
    );

    const Lined b(
        Vec2d(1.0, 2.0),
        Vec2d(3.0, 4.0)
    );

    const Lined c(
        Vec2d(3.0, 4.0),
        Vec2d(1.0, 2.0)
    );

    REQUIRE(a == b);
    REQUIRE_FALSE(a != b);

    REQUIRE(a != c);
    REQUIRE_FALSE(a == c);
}

//------------------------------------------------------------------------------
// Basic geometry
//------------------------------------------------------------------------------

TEST_CASE("Line geometry", "[Line]")
{
    const Lined line(
        Vec2d(1.0, 2.0),
        Vec2d(4.0, 6.0)
    );

    SECTION("vector")
    {
        REQUIRE(line.vec() == Vec2d(3.0, 4.0));
    }

    SECTION("length")
    {
        REQUIRE(line.length() == Catch::Approx(5.0));
        REQUIRE(line.squaredLength() == Catch::Approx(25.0));
    }

    SECTION("normalized vector")
    {
        REQUIRE_VEC2_NEAR(
            line.normalizedVec(),
            Vec2d(0.6, 0.8)
        );
    }

    SECTION("center")
    {
        REQUIRE(line.center() == Vec2d(2.5, 4.0));
    }

    SECTION("normal")
    {
        REQUIRE_VEC2_NEAR(
            line.normal(),
            Vec2d(-0.8, 0.6)
        );
    }

    SECTION("bounds")
    {
        REQUIRE(line.minX() == Catch::Approx(1.0));
        REQUIRE(line.minY() == Catch::Approx(2.0));
        REQUIRE(line.maxX() == Catch::Approx(4.0));
        REQUIRE(line.maxY() == Catch::Approx(6.0));
    }
}

//------------------------------------------------------------------------------
// pointAt
//------------------------------------------------------------------------------

TEST_CASE("Line pointAt", "[Line]")
{
    const Lined line(
        Vec2d(10.0, 20.0),
        Vec2d(20.0, 40.0)
    );

    REQUIRE(line.pointAt(0.0) == Vec2d(10.0, 20.0));
    REQUIRE(line.pointAt(1.0) == Vec2d(20.0, 40.0));

    REQUIRE_VEC2_NEAR(
        line.pointAt(0.5),
        Vec2d(15.0, 30.0)
    );

    SECTION("values outside segment")
    {
        REQUIRE_VEC2_NEAR(
            line.pointAt(-1.0),
            Vec2d(0.0, 0.0)
        );

        REQUIRE_VEC2_NEAR(
            line.pointAt(2.0),
            Vec2d(30.0, 60.0)
        );
    }

    SECTION("pointAtT compatibility")
    {
        REQUIRE(
            line.pointAtT(0.25) ==
            line.pointAt(0.25)
        );
    }
}

//------------------------------------------------------------------------------
// side
//------------------------------------------------------------------------------

TEST_CASE("Line side", "[Line]")
{
    const Lined line(
        Vec2d(0.0, 0.0),
        Vec2d(10.0, 0.0)
    );

    REQUIRE(line.side(Vec2d(5.0, 1.0)) > 0.0);
    REQUIRE(line.side(Vec2d(5.0, -1.0)) < 0.0);
    REQUIRE(line.side(Vec2d(5.0, 0.0)) == Catch::Approx(0.0));
}

//------------------------------------------------------------------------------
// Distance
//------------------------------------------------------------------------------

TEST_CASE("Line distance", "[Line]")
{
    const Lined line(
        Vec2d(0.0, 0.0),
        Vec2d(10.0, 0.0)
    );

    SECTION("infinite line")
    {
        REQUIRE(
            line.distance(Vec2d(5.0, 3.0)) ==
            Catch::Approx(3.0)
        );

        REQUIRE(
            line.distance(Vec2d(5.0, -3.0)) ==
            Catch::Approx(3.0)
        );
    }

    SECTION("point outside segment")
    {
        // Infinite line distance is still zero.
        REQUIRE(
            line.distance(Vec2d(20.0, 0.0)) ==
            Catch::Approx(0.0)
        );
    }

    SECTION("segment distance")
    {
        REQUIRE(
            line.distanceToSegment(Vec2d(5.0, 3.0)) ==
            Catch::Approx(3.0)
        );

        REQUIRE(
            line.distanceToSegment(Vec2d(20.0, 0.0)) ==
            Catch::Approx(10.0)
        );

        REQUIRE(
            line.distanceToSegment(Vec2d(-5.0, 0.0)) ==
            Catch::Approx(5.0)
        );
    }

    SECTION("degenerate line")
    {
        const Lined degenerate(
            Vec2d(1.0, 1.0),
            Vec2d(1.0, 1.0)
        );

        REQUIRE(
            std::isinf(
                degenerate.distance(Vec2d(2.0, 2.0))
            )
        );

        REQUIRE(
            std::isinf(
                degenerate.distanceToSegment(Vec2d(2.0, 2.0))
            )
        );
    }
}

//------------------------------------------------------------------------------
// coefficients
//------------------------------------------------------------------------------

TEST_CASE("Line coefficients", "[Line]")
{
    SECTION("diagonal")
    {
        const Lined line(
            Vec2d(1.0, 3.0),
            Vec2d(5.0, 11.0)
        );

        const Vec2d coefficients = line.coefficients();

        // y = 2x + 1
        REQUIRE(
            coefficients.x ==
            Catch::Approx(2.0)
        );

        REQUIRE(
            coefficients.y ==
            Catch::Approx(1.0)
        );
    }

    SECTION("horizontal")
    {
        const Lined line(
            Vec2d(0.0, 5.0),
            Vec2d(10.0, 5.0)
        );

        const Vec2d coefficients = line.coefficients();

        REQUIRE(coefficients.x == Catch::Approx(0.0));
        REQUIRE(coefficients.y == Catch::Approx(5.0));
    }

    SECTION("vertical")
    {
        const Lined line(
            Vec2d(5.0, 0.0),
            Vec2d(5.0, 10.0)
        );

        const Vec2d coefficients = line.coefficients();

        REQUIRE(std::isinf(coefficients.x));
        REQUIRE(std::isinf(coefficients.y));
    }
}

//------------------------------------------------------------------------------
// set
//------------------------------------------------------------------------------

TEST_CASE("Line set", "[Line]")
{
    Lined line;

    SECTION("set points")
    {
        line.set(
            Vec2d(1.0, 2.0),
            Vec2d(3.0, 4.0)
        );

        REQUIRE(line.p1() == Vec2d(1.0, 2.0));
        REQUIRE(line.p2() == Vec2d(3.0, 4.0));
    }

    SECTION("set coordinates")
    {
        line.set(1.0, 2.0, 3.0, 4.0);

        REQUIRE(line.p1() == Vec2d(1.0, 2.0));
        REQUIRE(line.p2() == Vec2d(3.0, 4.0));
    }
}

//------------------------------------------------------------------------------
// Translation
//------------------------------------------------------------------------------

TEST_CASE("Line translation", "[Line]")
{
    SECTION("by coordinates")
    {
        Lined line(
            Vec2d(1.0, 2.0),
            Vec2d(3.0, 4.0)
        );

        line.translate(10.0, 20.0);

        REQUIRE(line.p1() == Vec2d(11.0, 22.0));
        REQUIRE(line.p2() == Vec2d(13.0, 24.0));
    }

    SECTION("by vector")
    {
        Lined line(
            Vec2d(1.0, 2.0),
            Vec2d(3.0, 4.0)
        );

        line.translate(Vec2d(10.0, 20.0));

        REQUIRE(line.p1() == Vec2d(11.0, 22.0));
        REQUIRE(line.p2() == Vec2d(13.0, 24.0));
    }
}

//------------------------------------------------------------------------------
// Rotation
//------------------------------------------------------------------------------

TEST_CASE("Line rotation", "[Line]")
{
    SECTION("around origin")
    {
        Lined line(
            Vec2d(1.0, 0.0),
            Vec2d(3.0, 0.0)
        );

        line.rotateDegrees(90.0);

        REQUIRE_VEC2_NEAR(line.p1(), Vec2d(0.0, 1.0));
        REQUIRE_VEC2_NEAR(line.p2(), Vec2d(0.0, 3.0));
    }

    SECTION("around pivot")
    {
        Lined line(
            Vec2d(1.0, 0.0),
            Vec2d(3.0, 0.0)
        );

        line.rotateDegrees(Vec2d(1.0, 0.0), 90.0);

        REQUIRE_VEC2_NEAR(line.p1(), Vec2d(1.0, 0.0));
        REQUIRE_VEC2_NEAR(line.p2(), Vec2d(1.0, 2.0));
    }

    SECTION("rotateP1")
    {
        Lined line(
            Vec2d(1.0, 0.0),
            Vec2d(3.0, 0.0)
        );

        line.rotateP1Degrees(90.0);

        REQUIRE_VEC2_NEAR(line.p1(), Vec2d(3.0, -2.0));
        REQUIRE(line.p2() == Vec2d(3.0, 0.0));
    }

    SECTION("rotateP2")
    {
        Lined line(
            Vec2d(1.0, 0.0),
            Vec2d(3.0, 0.0)
        );

        line.rotateP2Degrees(90.0);

        REQUIRE_VEC2_NEAR(line.p1(), Vec2d(1.0, 0.0));
        REQUIRE_VEC2_NEAR(line.p2(), Vec2d(1.0, 2.0));
    }

    SECTION("rotateCentered")
    {
        Lined line(
            Vec2d(0.0, 0.0),
            Vec2d(10.0, 0.0)
        );

        line.rotateCenteredDegrees(90.0);

        REQUIRE_VEC2_NEAR(line.p1(), Vec2d(5.0, -5.0));
        REQUIRE_VEC2_NEAR(line.p2(), Vec2d(5.0, 5.0));
    }
}

//------------------------------------------------------------------------------
// inset
//------------------------------------------------------------------------------

TEST_CASE("Line inset", "[Line]")
{
    Lined line(
        Vec2d(0.0, 0.0),
        Vec2d(10.0, 0.0)
    );

    line.inset(2.0, 3.0);

    REQUIRE(line.p1() == Vec2d(2.0, 0.0));
    REQUIRE(line.p2() == Vec2d(7.0, 0.0));
}

//------------------------------------------------------------------------------
// offset
//------------------------------------------------------------------------------

TEST_CASE("Line offset", "[Line]")
{
    Lined line(
        Vec2d(0.0, 0.0),
        Vec2d(10.0, 0.0)
    );

    line.offset(3.0);

    REQUIRE_VEC2_NEAR(line.p1(), Vec2d(0.0, 3.0));
    REQUIRE_VEC2_NEAR(line.p2(), Vec2d(10.0, 3.0));
}

//------------------------------------------------------------------------------
// scaleCentered
//------------------------------------------------------------------------------

TEST_CASE("Line scaleCentered", "[Line]")
{
    Lined line(
        Vec2d(0.0, 0.0),
        Vec2d(10.0, 0.0)
    );

    line.scaleCentered(2.0);

    REQUIRE_VEC2_NEAR(line.p1(), Vec2d(-5.0, 0.0));
    REQUIRE_VEC2_NEAR(line.p2(), Vec2d(15.0, 0.0));
}

//------------------------------------------------------------------------------
// Intersection
//------------------------------------------------------------------------------

TEST_CASE("Line intersection", "[Line]")
{
    SECTION("intersecting lines")
    {
        const Lined horizontal(
            Vec2d(0.0, 0.0),
            Vec2d(10.0, 0.0)
        );

        const Lined vertical(
            Vec2d(5.0, -5.0),
            Vec2d(5.0, 5.0)
        );

        Vec2d result;

        REQUIRE(horizontal.intersects(vertical, result));
        REQUIRE_VEC2_NEAR(result, Vec2d(5.0, 0.0));
    }

    SECTION("intersection outside segments")
    {
        const Lined a(
            Vec2d(0.0, 0.0),
            Vec2d(1.0, 0.0)
        );

        const Lined b(
            Vec2d(2.0, -1.0),
            Vec2d(2.0, 1.0)
        );

        Vec2d result;

        // This intentionally succeeds because intersects() works
        // on infinite lines.
        REQUIRE(a.intersects(b, result));
        REQUIRE_VEC2_NEAR(result, Vec2d(2.0, 0.0));
    }

    SECTION("parallel lines")
    {
        const Lined a(
            Vec2d(0.0, 0.0),
            Vec2d(10.0, 0.0)
        );

        const Lined b(
            Vec2d(0.0, 5.0),
            Vec2d(10.0, 5.0)
        );

        Vec2d result;

        REQUIRE_FALSE(a.intersects(b, result));
    }

    SECTION("degenerate line")
    {
        const Lined a(
            Vec2d(0.0, 0.0),
            Vec2d(0.0, 0.0)
        );

        const Lined b(
            Vec2d(-1.0, 0.0),
            Vec2d(1.0, 0.0)
        );

        Vec2d result;

        REQUIRE_FALSE(a.intersects(b, result));
    }
}

//------------------------------------------------------------------------------
// nearestPoint
//------------------------------------------------------------------------------

TEST_CASE("Line nearestPoint", "[Line]")
{
    const Lined a(
        Vec2d(0.0, 0.0),
        Vec2d(10.0, 0.0)
    );

    SECTION("overlapping lines")
    {
        const Lined b(
            Vec2d(5.0, 2.0),
            Vec2d(8.0, 2.0)
        );

        REQUIRE(
            a.nearestPoint(b) ==
            Catch::Approx(std::sqrt(8.0))
        );
    }

    SECTION("separated lines")
    {
        const Lined b(
            Vec2d(20.0, 0.0),
            Vec2d(30.0, 0.0)
        );

        REQUIRE(
            a.nearestPoint(b) ==
            Catch::Approx(10.0)
        );
    }
}

//------------------------------------------------------------------------------
// hit
//------------------------------------------------------------------------------

TEST_CASE("Line hit", "[Line]")
{
    const Lined line(
        Vec2d(0.0, 0.0),
        Vec2d(10.0, 0.0)
    );

    SECTION("point on segment")
    {
        double distance = 0.0;

        REQUIRE(
            line.hit(
                Vec2d(5.0, 0.0),
                0.1,
                distance
            )
        );

        REQUIRE(distance == Catch::Approx(0.0));
    }

    SECTION("point within tolerance")
    {
        double distance = 0.0;

        REQUIRE(
            line.hit(
                Vec2d(5.0, 2.0),
                2.1,
                distance
            )
        );

        REQUIRE(distance == Catch::Approx(2.0));
    }

    SECTION("point outside tolerance")
    {
        double distance = 0.0;

        REQUIRE_FALSE(
            line.hit(
                Vec2d(5.0, 2.0),
                1.9,
                distance
            )
        );

        REQUIRE(distance == Catch::Approx(2.0));
    }

    SECTION("point beyond endpoint")
    {
        double distance = 0.0;

        REQUIRE(
            line.hit(
                Vec2d(15.0, 0.0),
                5.0,
                distance
            )
        );

        REQUIRE(distance == Catch::Approx(5.0));
    }
}

//------------------------------------------------------------------------------
// Rectangle intersection
//------------------------------------------------------------------------------

TEST_CASE("Line rectangle intersection", "[Line]")
{
    const Rectd rect(
        0.0,
        0.0,
        10.0,
        10.0
    );

    SECTION("passes through rectangle")
    {
        const Lined line(
            Vec2d(-5.0, 5.0),
            Vec2d(15.0, 5.0)
        );

        REQUIRE(line.intersects(rect));
    }

    SECTION("starts inside rectangle")
    {
        const Lined line(
            Vec2d(5.0, 5.0),
            Vec2d(20.0, 5.0)
        );

        REQUIRE(line.intersects(rect));
    }

    SECTION("ends inside rectangle")
    {
        const Lined line(
            Vec2d(-10.0, 5.0),
            Vec2d(5.0, 5.0)
        );

        REQUIRE(line.intersects(rect));
    }

    SECTION("touches rectangle edge")
    {
        const Lined line(
            Vec2d(-5.0, 0.0),
            Vec2d(15.0, 0.0)
        );

        REQUIRE(line.intersects(rect));
    }

    SECTION("completely outside")
    {
        const Lined line(
            Vec2d(-5.0, 20.0),
            Vec2d(15.0, 20.0)
        );

        REQUIRE_FALSE(line.intersects(rect));
    }

    SECTION("completely before rectangle")
    {
        const Lined line(
            Vec2d(-20.0, 5.0),
            Vec2d(-10.0, 5.0)
        );

        REQUIRE_FALSE(line.intersects(rect));
    }

    SECTION("vertical through rectangle")
    {
        const Lined line(
            Vec2d(5.0, -5.0),
            Vec2d(5.0, 15.0)
        );

        REQUIRE(line.intersects(rect));
    }

    SECTION("vertical beside rectangle")
    {
        const Lined line(
            Vec2d(15.0, -5.0),
            Vec2d(15.0, 15.0)
        );

        REQUIRE_FALSE(line.intersects(rect));
    }
}

//------------------------------------------------------------------------------
// Degenerate line
//------------------------------------------------------------------------------

TEST_CASE("Degenerate Line", "[Line]")
{
    Lined line(
        Vec2d(5.0, 5.0),
        Vec2d(5.0, 5.0)
    );

    REQUIRE(line.length() == Catch::Approx(0.0));
    REQUIRE(line.squaredLength() == Catch::Approx(0.0));
    REQUIRE(line.center() == Vec2d(5.0, 5.0));

    SECTION("normal")
    {
        const Vec2d normal = line.normal();

        // Vec2::normalized() should define the zero-vector behavior.
        // This test deliberately does not assume a particular result.
        SUCCEED();
    }

    SECTION("offset")
    {
        line.offset(10.0);

        REQUIRE(line.p1() == Vec2d(5.0, 5.0));
        REQUIRE(line.p2() == Vec2d(5.0, 5.0));
    }
}

//------------------------------------------------------------------------------
// Float instantiation
//------------------------------------------------------------------------------

TEST_CASE("Line float instantiation", "[Line]")
{
    const Linef line(
        Vec2f(0.0f, 0.0f),
        Vec2f(3.0f, 4.0f)
    );

    REQUIRE(
        line.length() ==
        Catch::Approx(5.0f)
    );

    REQUIRE(
        line.squaredLength() ==
        Catch::Approx(25.0f)
    );
}