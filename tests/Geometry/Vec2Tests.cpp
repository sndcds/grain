#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <Grain.hpp>
#include <grain/Geometry/Vec2.hpp>

#include <cstdint>
#include <numbers>

using namespace Grain;

TEST_CASE("Vec2 construction and access", "[Vec2]")
{
    SECTION("default construction")
    {
        Vec2i v;

        REQUIRE(v.x == 0);
        REQUIRE(v.y == 0);
        REQUIRE(v[0] == 0);
        REQUIRE(v[1] == 0);
    }

    SECTION("construction from coordinates")
    {
        Vec2i v{10, 20};

        REQUIRE(v.x == 10);
        REQUIRE(v.y == 20);
        REQUIRE(v[0] == 10);
        REQUIRE(v[1] == 20);
    }

    SECTION("copy construction")
    {
        Vec2i original{10, 20};
        Vec2i copy{original};

        REQUIRE(copy == original);
    }

    SECTION("conversion between vector types")
    {
        Vec2d source{10.5, 20.75};
        Vec2i converted{source};

        REQUIRE(converted.x == 10);
        REQUIRE(converted.y == 20);
    }

    SECTION("component assignment")
    {
        Vec2i v{1, 2};

        v[0] = 10;
        v[1] = 20;

        REQUIRE(v.x == 10);
        REQUIRE(v.y == 20);
    }

    SECTION("assignment between vector types")
    {
        Vec2d source{10.5, 20.75};
        Vec2i destination;

        destination = source;

        REQUIRE(destination.x == 10);
        REQUIRE(destination.y == 20);
    }
}


TEST_CASE("Vec2 comparison", "[Vec2]")
{
    const Vec2i a{10, 20};
    const Vec2i b{10, 20};
    const Vec2i c{20, 10};

    REQUIRE(a == b);
    REQUIRE_FALSE(a != b);

    REQUIRE(a != c);
    REQUIRE_FALSE(a == c);
}


TEST_CASE("Vec2 unary arithmetic", "[Vec2]")
{
    const Vec2i v{10, -20};

    REQUIRE(-v == Vec2i{-10, 20});
}


TEST_CASE("Vec2 vector arithmetic", "[Vec2]")
{
    const Vec2i a{10, 20};
    const Vec2i b{3, 4};

    SECTION("addition")
    {
        REQUIRE(a + b == Vec2i{13, 24});
    }

    SECTION("subtraction")
    {
        REQUIRE(a - b == Vec2i{7, 16});
    }

    SECTION("component-wise multiplication")
    {
        REQUIRE(a * b == Vec2i{30, 80});
    }

    SECTION("component-wise division")
    {
        REQUIRE(a / b == Vec2i{3, 5});
    }

    SECTION("scalar multiplication")
    {
        REQUIRE(a * 2 == Vec2i{20, 40});
        REQUIRE(2 * a == Vec2i{20, 40});
    }

    SECTION("scalar division")
    {
        REQUIRE(a / 2 == Vec2i{5, 10});
    }
}


TEST_CASE("Vec2 compound arithmetic", "[Vec2]")
{
    SECTION("addition assignment")
    {
        Vec2i v{10, 20};

        v += Vec2i{3, 4};

        REQUIRE(v == Vec2i{13, 24});
    }

    SECTION("subtraction assignment")
    {
        Vec2i v{10, 20};

        v -= Vec2i{3, 4};

        REQUIRE(v == Vec2i{7, 16});
    }

    SECTION("component-wise multiplication assignment")
    {
        Vec2i v{10, 20};

        v *= Vec2i{3, 4};

        REQUIRE(v == Vec2i{30, 80});
    }

    SECTION("component-wise division assignment")
    {
        Vec2i v{10, 20};

        v /= Vec2i{2, 4};

        REQUIRE(v == Vec2i{5, 5});
    }

    SECTION("scalar multiplication assignment")
    {
        Vec2i v{10, 20};

        v *= 3;

        REQUIRE(v == Vec2i{30, 60});
    }

    SECTION("scalar division assignment")
    {
        Vec2i v{10, 20};

        v /= 2;

        REQUIRE(v == Vec2i{5, 10});
    }
}


TEST_CASE("Vec2 length and distance", "[Vec2]")
{
    const Vec2d a{3.0, 4.0};
    const Vec2d b{6.0, 8.0};

    SECTION("length")
    {
        REQUIRE(a.length() == Catch::Approx(5.0));
    }

    SECTION("squared length")
    {
        REQUIRE(a.squaredLength() == 25.0);
    }

    SECTION("distance")
    {
        REQUIRE(a.distance(b) == Catch::Approx(5.0));
    }

    SECTION("squared distance")
    {
        REQUIRE(a.squaredDistance(b) == 25.0);
    }
}


TEST_CASE("Vec2 dot and cross product", "[Vec2]")
{
    const Vec2d a{3.0, 4.0};
    const Vec2d b{5.0, 2.0};

    REQUIRE(a.dot(b) == Catch::Approx(23.0));
    REQUIRE(a.cross(b) == Catch::Approx(-14.0));
}


TEST_CASE("Vec2 angle", "[Vec2]")
{
    SECTION("parallel vectors")
    {
        const Vec2d a{1.0, 0.0};
        const Vec2d b{2.0, 0.0};

        REQUIRE(a.angle(b) == Catch::Approx(0.0));
    }

    SECTION("perpendicular vectors")
    {
        const Vec2d a{1.0, 0.0};
        const Vec2d b{0.0, 1.0};

        REQUIRE(a.angle(b) == Catch::Approx(90.0));
    }

    SECTION("opposite vectors")
    {
        const Vec2d a{1.0, 0.0};
        const Vec2d b{-1.0, 0.0};

        REQUIRE(a.angle(b) == Catch::Approx(180.0));
    }

    SECTION("zero vector")
    {
        const Vec2d zero{0.0, 0.0};
        const Vec2d v{1.0, 0.0};

        REQUIRE(zero.angle(v) == Catch::Approx(0.0));
        REQUIRE(v.angle(zero) == Catch::Approx(0.0));
    }
}


TEST_CASE("Vec2 flipping and reflection", "[Vec2]")
{
    SECTION("flipped")
    {
        const Vec2i v{10, -20};

        REQUIRE(v.flipped() == Vec2i{-10, 20});
    }

    SECTION("flip")
    {
        Vec2i v{10, -20};

        v.flip();

        REQUIRE(v == Vec2i{-10, 20});
    }

    SECTION("reflected point")
    {
        const Vec2d point{2.0, 3.0};
        const Vec2d pivot{5.0, 7.0};

        REQUIRE(point.reflectedPoint(pivot) == Vec2d{8.0, 11.0});
    }
}


TEST_CASE("Vec2 normalization", "[Vec2]")
{
    SECTION("normalized returns a unit vector")
    {
        const Vec2d v{3.0, 4.0};
        const Vec2d result = v.normalized();

        REQUIRE(result.x == Catch::Approx(0.6));
        REQUIRE(result.y == Catch::Approx(0.8));
        REQUIRE(result.length() == Catch::Approx(1.0));

        // Original remains unchanged.
        REQUIRE(v == Vec2d{3.0, 4.0});
    }

    SECTION("normalize modifies the vector")
    {
        Vec2d v{3.0, 4.0};

        v.normalize();

        REQUIRE(v.x == Catch::Approx(0.6));
        REQUIRE(v.y == Catch::Approx(0.8));
        REQUIRE(v.length() == Catch::Approx(1.0));
    }

    SECTION("normalizing zero vector does nothing")
    {
        Vec2d v{0.0, 0.0};

        v.normalize();

        REQUIRE(v == Vec2d{0.0, 0.0});
    }
}


TEST_CASE("Vec2 setLength", "[Vec2]")
{
    SECTION("changes vector length")
    {
        Vec2d v{3.0, 4.0};

        v.setLength(10.0);

        REQUIRE(v.length() == Catch::Approx(10.0));
        REQUIRE(v.x == Catch::Approx(6.0));
        REQUIRE(v.y == Catch::Approx(8.0));
    }

    SECTION("preserves direction")
    {
        Vec2d v{3.0, 4.0};

        v.setLength(2.0);

        REQUIRE(v.x == Catch::Approx(1.2));
        REQUIRE(v.y == Catch::Approx(1.6));
    }

    SECTION("zero vector remains unchanged")
    {
        Vec2d v{0.0, 0.0};

        v.setLength(10.0);

        REQUIRE(v == Vec2d{0.0, 0.0});
    }
}


TEST_CASE("Vec2 modification", "[Vec2]")
{
    SECTION("set")
    {
        Vec2i v{1, 2};

        v.set(10, 20);

        REQUIRE(v.x == 10);
        REQUIRE(v.y == 20);
    }

    SECTION("zero")
    {
        Vec2i v{10, 20};

        v.zero();

        REQUIRE(v == Vec2i{0, 0});
    }

    SECTION("translateX")
    {
        Vec2i v{10, 20};

        v.translateX(5);

        REQUIRE(v == Vec2i{15, 20});
    }

    SECTION("translateY")
    {
        Vec2i v{10, 20};

        v.translateY(5);

        REQUIRE(v == Vec2i{10, 25});
    }

    SECTION("translate")
    {
        Vec2i v{10, 20};

        v.translate(5, -10);

        REQUIRE(v == Vec2i{15, 10});
    }

    SECTION("scaleX")
    {
        Vec2i v{10, 20};

        v.scaleX(3);

        REQUIRE(v == Vec2i{30, 20});
    }

    SECTION("scaleY")
    {
        Vec2i v{10, 20};

        v.scaleY(3);

        REQUIRE(v == Vec2i{10, 60});
    }

    SECTION("uniform scale")
    {
        Vec2i v{10, 20};

        v.scale(3);

        REQUIRE(v == Vec2i{30, 60});
    }

    SECTION("non-uniform scale")
    {
        Vec2i v{10, 20};

        v.scale(2, 3);

        REQUIRE(v == Vec2i{20, 60});
    }
}


TEST_CASE("Vec2 rotation", "[Vec2]")
{
    constexpr double epsilon = 1e-10;

    SECTION("rotate 90 degrees")
    {
        Vec2d v{1.0, 0.0};

        v.rotate(90.0);

        REQUIRE(v.x == Catch::Approx(0.0).margin(epsilon));
        REQUIRE(v.y == Catch::Approx(1.0).margin(epsilon));
    }

    SECTION("rotate 180 degrees")
    {
        Vec2d v{1.0, 0.0};

        v.rotate(180.0);

        REQUIRE(v.x == Catch::Approx(-1.0).margin(epsilon));
        REQUIRE(v.y == Catch::Approx(0.0).margin(epsilon));
    }

    SECTION("rotate radians")
    {
        Vec2d v{1.0, 0.0};

        v.rotateRad(std::numbers::pi / 2.0);

        REQUIRE(v.x == Catch::Approx(0.0).margin(epsilon));
        REQUIRE(v.y == Catch::Approx(1.0).margin(epsilon));
    }

    SECTION("rotated does not modify original")
    {
        const Vec2d v{1.0, 0.0};

        const Vec2d result = v.rotated(90.0);

        REQUIRE(v == Vec2d{1.0, 0.0});
        REQUIRE(result.x == Catch::Approx(0.0).margin(epsilon));
        REQUIRE(result.y == Catch::Approx(1.0).margin(epsilon));
    }

    SECTION("rotatedRad does not modify original")
    {
        const Vec2d v{1.0, 0.0};

        const Vec2d result = v.rotatedRad(std::numbers::pi / 2.0);

        REQUIRE(v == Vec2d{1.0, 0.0});
        REQUIRE(result.x == Catch::Approx(0.0).margin(epsilon));
        REQUIRE(result.y == Catch::Approx(1.0).margin(epsilon));
    }
}


TEST_CASE("Vec2 linear interpolation", "[Vec2]")
{
    const Vec2d a{0.0, 10.0};
    const Vec2d b{10.0, 20.0};

    SECTION("setLerp")
    {
        Vec2d result;

        result.setLerp(a, b, 0.5);

        REQUIRE(result.x == Catch::Approx(5.0));
        REQUIRE(result.y == Catch::Approx(15.0));
    }

    SECTION("lerp")
    {
        const Vec2d result = Vec2d::lerp(a, b, 0.25);

        REQUIRE(result.x == Catch::Approx(2.5));
        REQUIRE(result.y == Catch::Approx(12.5));
    }

    SECTION("at start")
    {
        REQUIRE(Vec2d::lerp(a, b, 0.0) == a);
    }

    SECTION("at end")
    {
        REQUIRE(Vec2d::lerp(a, b, 1.0) == b);
    }
}


TEST_CASE("Vec2 approximate equality", "[Vec2]")
{
    const Vec2d a{10.0, 20.0};

    SECTION("within threshold")
    {
        const Vec2d b{10.001, 20.001};

        REQUIRE(a.checkEqual(b, 0.01));
    }

    SECTION("outside threshold")
    {
        const Vec2d b{10.1, 20.1};

        REQUIRE_FALSE(a.checkEqual(b, 0.01));
    }

    SECTION("exactly equal")
    {
        REQUIRE(a.checkEqual(a, 0.0));
    }
}


TEST_CASE("Vec2 CSV parsing", "[Vec2][CSV]")
{
    SECTION("parses comma separated values")
    {
        Vec2d v;

        REQUIRE(v.setByCSV("10.5,20.25"));
        REQUIRE(v.x == Catch::Approx(10.5));
        REQUIRE(v.y == Catch::Approx(20.25));
    }

    SECTION("parses custom delimiter")
    {
        Vec2d v;

        REQUIRE(v.setByCSV("10.5;20.25", ';'));
        REQUIRE(v.x == Catch::Approx(10.5));
        REQUIRE(v.y == Catch::Approx(20.25));
    }

    SECTION("rejects null")
    {
        Vec2d v{10.0, 20.0};

        REQUIRE_FALSE(v.setByCSV(nullptr));
        REQUIRE(v == Vec2d{10.0, 20.0});
    }
}


TEST_CASE("Vec2 stream output", "[Vec2]")
{
    SECTION("integer vector")
    {
        std::ostringstream stream;
        stream << Vec2i{10, 20};

        REQUIRE(stream.str() == "10, 20");
    }

    SECTION("floating point vector")
    {
        std::ostringstream stream;
        stream << Vec2d{10.5, 20.25};

        REQUIRE(stream.str() == "10.5, 20.25");
    }

    SECTION("int8 vector is printed numerically")
    {
        std::ostringstream stream;
        stream << Vec2<std::int8_t>{10, 20};

        REQUIRE(stream.str() == "10, 20");
    }
}