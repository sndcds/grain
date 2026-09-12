#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <grain/Math/Vec3.hpp>

using namespace Grain;

namespace {

constexpr double epsilon = 1e-10;

void REQUIRE_VEC3_NEAR(
    const Vec3d& actual,
    const Vec3d& expected,
    double margin = epsilon
) {
    REQUIRE(actual.x == Catch::Approx(expected.x).margin(margin));
    REQUIRE(actual.y == Catch::Approx(expected.y).margin(margin));
    REQUIRE(actual.z == Catch::Approx(expected.z).margin(margin));
}

} // namespace


TEST_CASE("Vec3 construction", "[Vec3]")
{
    SECTION("default construction")
    {
        constexpr Vec3d v;

        REQUIRE(v.x == 0.0);
        REQUIRE(v.y == 0.0);
        REQUIRE(v.z == 0.0);
    }

    SECTION("value construction")
    {
        constexpr Vec3d v(1.0, 2.0, 3.0);

        REQUIRE(v.x == 1.0);
        REQUIRE(v.y == 2.0);
        REQUIRE(v.z == 3.0);
    }

    SECTION("copy construction")
    {
        const Vec3d a(1.0, 2.0, 3.0);
        const Vec3d b(a);

        REQUIRE(b == a);
    }

    SECTION("conversion construction")
    {
        const Vec3i a(1, 2, 3);
        const Vec3d b(a);

        REQUIRE(b.x == 1.0);
        REQUIRE(b.y == 2.0);
        REQUIRE(b.z == 3.0);
    }
}


TEST_CASE("Vec3 assignment", "[Vec3]")
{
    SECTION("copy assignment")
    {
        Vec3d a(1.0, 2.0, 3.0);
        Vec3d b;

        b = a;

        REQUIRE(b == a);
    }

    SECTION("conversion assignment")
    {
        Vec3i a(1, 2, 3);
        Vec3d b;

        b = a;

        REQUIRE(b == Vec3d(1.0, 2.0, 3.0));
    }
}


TEST_CASE("Vec3 indexing", "[Vec3]")
{
    Vec3d v(10.0, 20.0, 30.0);

    REQUIRE(v[0] == 10.0);
    REQUIRE(v[1] == 20.0);
    REQUIRE(v[2] == 30.0);

    v[0] = 11.0;
    v[1] = 22.0;
    v[2] = 33.0;

    REQUIRE(v == Vec3d(11.0, 22.0, 33.0));
}


TEST_CASE("Vec3 comparison", "[Vec3]")
{
    const Vec3d a(1.0, 2.0, 3.0);
    const Vec3d b(1.0, 2.0, 3.0);
    const Vec3d c(1.0, 2.0, 4.0);

    REQUIRE(a == b);
    REQUIRE_FALSE(a != b);

    REQUIRE(a != c);
    REQUIRE_FALSE(a == c);
}


TEST_CASE("Vec3 unary arithmetic", "[Vec3]")
{
    const Vec3d v(1.0, -2.0, 3.0);

    REQUIRE(-v == Vec3d(-1.0, 2.0, -3.0));
}


TEST_CASE("Vec3 vector arithmetic", "[Vec3]")
{
    const Vec3d a(2.0, 4.0, 6.0);
    const Vec3d b(1.0, 2.0, 3.0);

    SECTION("addition")
    {
        REQUIRE(a + b == Vec3d(3.0, 6.0, 9.0));
    }

    SECTION("subtraction")
    {
        REQUIRE(a - b == Vec3d(1.0, 2.0, 3.0));
    }

    SECTION("component-wise multiplication")
    {
        REQUIRE(a * b == Vec3d(2.0, 8.0, 18.0));
    }

    SECTION("component-wise division")
    {
        REQUIRE(a / b == Vec3d(2.0, 2.0, 2.0));
    }

    SECTION("scalar multiplication")
    {
        REQUIRE(a * 2.0 == Vec3d(4.0, 8.0, 12.0));
    }

    SECTION("scalar division")
    {
        REQUIRE(a / 2.0 == Vec3d(1.0, 2.0, 3.0));
    }

    SECTION("scalar multiplication from left")
    {
        REQUIRE(2.0 * a == Vec3d(4.0, 8.0, 12.0));
    }
}


TEST_CASE("Vec3 compound arithmetic", "[Vec3]")
{
    Vec3d v(2.0, 4.0, 6.0);

    SECTION("operator+=")
    {
        v += Vec3d(1.0, 2.0, 3.0);
        REQUIRE(v == Vec3d(3.0, 6.0, 9.0));
    }

    SECTION("operator-=")
    {
        v -= Vec3d(1.0, 2.0, 3.0);
        REQUIRE(v == Vec3d(1.0, 2.0, 3.0));
    }

    SECTION("component-wise operator*=")
    {
        v *= Vec3d(2.0, 3.0, 4.0);
        REQUIRE(v == Vec3d(4.0, 12.0, 24.0));
    }

    SECTION("component-wise operator/=")
    {
        v /= Vec3d(2.0, 2.0, 3.0);
        REQUIRE(v == Vec3d(1.0, 2.0, 2.0));
    }

    SECTION("scalar operator*=")
    {
        v *= 2.0;
        REQUIRE(v == Vec3d(4.0, 8.0, 12.0));
    }

    SECTION("scalar operator/=")
    {
        v /= 2.0;
        REQUIRE(v == Vec3d(1.0, 2.0, 3.0));
    }
}


TEST_CASE("Vec3 length and distance", "[Vec3]")
{
    SECTION("length")
    {
        const Vec3d v(2.0, 3.0, 6.0);

        REQUIRE(v.length() == Catch::Approx(7.0));
        REQUIRE(v.squaredLength() == 49.0);
    }

    SECTION("zero length")
    {
        const Vec3d v;

        REQUIRE(v.length() == 0.0);
        REQUIRE(v.squaredLength() == 0.0);
    }

    SECTION("distance")
    {
        const Vec3d a(1.0, 2.0, 3.0);
        const Vec3d b(4.0, 6.0, 6.0);

        REQUIRE(a.distance(b) == Catch::Approx(5.8309518948));
        REQUIRE(a.squaredDistance(b) == 34.0);
    }
}


TEST_CASE("Vec3 dot product", "[Vec3]")
{
    const Vec3d a(1.0, 2.0, 3.0);
    const Vec3d b(4.0, 5.0, 6.0);

    REQUIRE(a.dot(b) == 32.0);
    REQUIRE(b.dot(a) == 32.0);
}


TEST_CASE("Vec3 cross product", "[Vec3]")
{
    SECTION("standard basis")
    {
        const Vec3d x(1.0, 0.0, 0.0);
        const Vec3d y(0.0, 1.0, 0.0);
        const Vec3d z(0.0, 0.0, 1.0);

        REQUIRE(x.cross(y) == z);
        REQUIRE(y.cross(z) == x);
        REQUIRE(z.cross(x) == y);
    }

    SECTION("anti-commutative")
    {
        const Vec3d a(1.0, 2.0, 3.0);
        const Vec3d b(4.0, 5.0, 6.0);

        REQUIRE(a.cross(b) == -b.cross(a));
    }

    SECTION("perpendicular")
    {
        const Vec3d a(1.0, 2.0, 3.0);
        const Vec3d b(4.0, 5.0, 6.0);
        const Vec3d c = a.cross(b);

        REQUIRE(c.dot(a) == Catch::Approx(0.0).margin(epsilon));
        REQUIRE(c.dot(b) == Catch::Approx(0.0).margin(epsilon));
    }

    SECTION("parallel vectors")
    {
        const Vec3d a(1.0, 2.0, 3.0);
        const Vec3d b(2.0, 4.0, 6.0);

        REQUIRE(a.cross(b) == Vec3d(0.0, 0.0, 0.0));
    }
}


TEST_CASE("Vec3 angle", "[Vec3]")
{
    SECTION("same direction")
    {
        REQUIRE(
            Vec3d(1.0, 0.0, 0.0).angle(
                Vec3d(1.0, 0.0, 0.0)
            ) == Catch::Approx(0.0)
        );
    }

    SECTION("opposite direction")
    {
        REQUIRE(
            Vec3d(1.0, 0.0, 0.0).angle(
                Vec3d(-1.0, 0.0, 0.0)
            ) == Catch::Approx(180.0)
        );
    }

    SECTION("perpendicular")
    {
        REQUIRE(
            Vec3d(1.0, 0.0, 0.0).angle(
                Vec3d(0.0, 1.0, 0.0)
            ) == Catch::Approx(90.0)
        );
    }

    SECTION("zero vector")
    {
        REQUIRE(
            Vec3d(0.0, 0.0, 0.0).angle(
                Vec3d(1.0, 0.0, 0.0)
            ) == 0.0
        );
    }
}


TEST_CASE("Vec3 flipped and reflected", "[Vec3]")
{
    SECTION("flipped")
    {
        const Vec3d v(1.0, -2.0, 3.0);

        REQUIRE(v.flipped() == Vec3d(-1.0, 2.0, -3.0));
        REQUIRE(v == Vec3d(1.0, -2.0, 3.0));
    }

    SECTION("reflected point")
    {
        const Vec3d point(1.0, 2.0, 3.0);
        const Vec3d pivot(4.0, 6.0, 8.0);

        REQUIRE(
            point.reflectedPoint(pivot) ==
            Vec3d(7.0, 10.0, 13.0)
        );
    }
}


TEST_CASE("Vec3 normalization", "[Vec3]")
{
    SECTION("normalize")
    {
        Vec3d v(2.0, 3.0, 6.0);
        v.normalize();

        REQUIRE(v.length() == Catch::Approx(1.0));
        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(
                2.0 / 7.0,
                3.0 / 7.0,
                6.0 / 7.0
            )
        );
    }

    SECTION("normalized does not modify original")
    {
        const Vec3d v(2.0, 3.0, 6.0);
        const Vec3d result = v.normalized();

        REQUIRE(v == Vec3d(2.0, 3.0, 6.0));
        REQUIRE(result.length() == Catch::Approx(1.0));
    }

    SECTION("zero vector")
    {
        Vec3d v;

        v.normalize();

        REQUIRE(v == Vec3d(0.0, 0.0, 0.0));
        REQUIRE(v.normalized() == Vec3d(0.0, 0.0, 0.0));
    }
}


TEST_CASE("Vec3 setLength", "[Vec3]")
{
    SECTION("increase length")
    {
        Vec3d v(1.0, 0.0, 0.0);

        v.setLength(5.0);

        REQUIRE(v == Vec3d(5.0, 0.0, 0.0));
        REQUIRE(v.length() == Catch::Approx(5.0));
    }

    SECTION("decrease length")
    {
        Vec3d v(2.0, 0.0, 0.0);

        v.setLength(1.0);

        REQUIRE(v == Vec3d(1.0, 0.0, 0.0));
    }

    SECTION("zero vector")
    {
        Vec3d v;

        v.setLength(10.0);

        REQUIRE(v == Vec3d(0.0, 0.0, 0.0));
    }
}


TEST_CASE("Vec3 checkEqual", "[Vec3]")
{
    const Vec3d a(1.0, 2.0, 3.0);

    REQUIRE(a.checkEqual(
        Vec3d(1.0, 2.0, 3.0),
        0.0
    ));

    REQUIRE(a.checkEqual(
        Vec3d(1.0, 2.0, 3.0),
        0.001
    ));

    REQUIRE(a.checkEqual(
        Vec3d(1.0, 2.0, 3.0),
        Vec3d(1.0, 2.0, 3.001).z - 3.0
    ));

    REQUIRE_FALSE(a.checkEqual(
        Vec3d(1.0, 2.0, 3.0),
        0.001
    ) == false);
}


TEST_CASE("Vec3 modification", "[Vec3]")
{
    SECTION("set")
    {
        Vec3d v;

        v.set(1.0, 2.0, 3.0);

        REQUIRE(v == Vec3d(1.0, 2.0, 3.0));
    }

    SECTION("zero")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.zero();

        REQUIRE(v == Vec3d(0.0, 0.0, 0.0));
    }

    SECTION("flip")
    {
        Vec3d v(1.0, -2.0, 3.0);

        v.flip();

        REQUIRE(v == Vec3d(-1.0, 2.0, -3.0));
    }

    SECTION("translateX")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.translateX(10.0);

        REQUIRE(v == Vec3d(11.0, 2.0, 3.0));
    }

    SECTION("translateY")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.translateY(10.0);

        REQUIRE(v == Vec3d(1.0, 12.0, 3.0));
    }

    SECTION("translateZ")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.translateZ(10.0);

        REQUIRE(v == Vec3d(1.0, 2.0, 13.0));
    }

    SECTION("translate")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.translate(10.0, 20.0, 30.0);

        REQUIRE(v == Vec3d(11.0, 22.0, 33.0));
    }
}


TEST_CASE("Vec3 scaling", "[Vec3]")
{
    SECTION("scaleX")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.scaleX(10.0);

        REQUIRE(v == Vec3d(10.0, 2.0, 3.0));
    }

    SECTION("scaleY")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.scaleY(10.0);

        REQUIRE(v == Vec3d(1.0, 20.0, 3.0));
    }

    SECTION("scaleZ")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.scaleZ(10.0);

        REQUIRE(v == Vec3d(1.0, 2.0, 30.0));
    }

    SECTION("uniform scale")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.scale(2.0);

        REQUIRE(v == Vec3d(2.0, 4.0, 6.0));
    }

    SECTION("non-uniform scale")
    {
        Vec3d v(1.0, 2.0, 3.0);

        v.scale(2.0, 3.0, 4.0);

        REQUIRE(v == Vec3d(2.0, 6.0, 12.0));
    }
}


TEST_CASE("Vec3 rotation around X axis", "[Vec3]")
{
    SECTION("90 degrees")
    {
        Vec3d v(0.0, 1.0, 0.0);

        v.rotateXDegrees(90.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, 0.0, 1.0)
        );
    }

    SECTION("radians")
    {
        Vec3d v(0.0, 1.0, 0.0);

        v.rotateX(std::numbers::pi / 2.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, 0.0, 1.0)
        );
    }

    SECTION("negative rotation")
    {
        Vec3d v(0.0, 1.0, 0.0);

        v.rotateXDegrees(-90.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, 0.0, -1.0)
        );
    }
}


TEST_CASE("Vec3 rotation around Y axis", "[Vec3]")
{
    SECTION("90 degrees")
    {
        Vec3d v(1.0, 0.0, 0.0);

        v.rotateYDegrees(90.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, 0.0, -1.0)
        );
    }

    SECTION("radians")
    {
        Vec3d v(1.0, 0.0, 0.0);

        v.rotateY(std::numbers::pi / 2.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, 0.0, -1.0)
        );
    }

    SECTION("negative rotation")
    {
        Vec3d v(1.0, 0.0, 0.0);

        v.rotateYDegrees(-90.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, 0.0, 1.0)
        );
    }
}


TEST_CASE("Vec3 rotation around Z axis", "[Vec3]")
{
    SECTION("90 degrees")
    {
        Vec3d v(1.0, 0.0, 0.0);

        v.rotateZDegrees(90.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, 1.0, 0.0)
        );
    }

    SECTION("radians")
    {
        Vec3d v(1.0, 0.0, 0.0);

        v.rotateZ(std::numbers::pi / 2.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, 1.0, 0.0)
        );
    }

    SECTION("negative rotation")
    {
        Vec3d v(1.0, 0.0, 0.0);

        v.rotateZDegrees(-90.0);

        REQUIRE_VEC3_NEAR(
            v,
            Vec3d(0.0, -1.0, 0.0)
        );
    }
}


TEST_CASE("Vec3 rotation preserves length", "[Vec3]")
{
    const Vec3d original(1.0, 2.0, 3.0);

    Vec3d x = original;
    Vec3d y = original;
    Vec3d z = original;

    x.rotateXDegrees(123.0);
    y.rotateYDegrees(123.0);
    z.rotateZDegrees(123.0);

    REQUIRE(x.length() == Catch::Approx(original.length()));
    REQUIRE(y.length() == Catch::Approx(original.length()));
    REQUIRE(z.length() == Catch::Approx(original.length()));
}


TEST_CASE("Vec3 lerp", "[Vec3]")
{
    const Vec3d a(0.0, 10.0, 20.0);
    const Vec3d b(10.0, 20.0, 30.0);

    SECTION("t = 0")
    {
        REQUIRE(Vec3d::lerp(a, b, 0.0) == a);
    }

    SECTION("t = 1")
    {
        REQUIRE(Vec3d::lerp(a, b, 1.0) == b);
    }

    SECTION("t = 0.5")
    {
        REQUIRE(
            Vec3d::lerp(a, b, 0.5) ==
            Vec3d(5.0, 15.0, 25.0)
        );
    }

    SECTION("setLerp")
    {
        Vec3d result;

        result.setLerp(a, b, 0.25);

        REQUIRE(
            result ==
            Vec3d(2.5, 12.5, 22.5)
        );
    }

    SECTION("extrapolation")
    {
        REQUIRE(
            Vec3d::lerp(a, b, 2.0) ==
            Vec3d(20.0, 30.0, 40.0)
        );
    }
}


TEST_CASE("Vec3 CSV", "[Vec3]")
{
    SECTION("null input")
    {
        Vec3d v(1.0, 2.0, 3.0);

        REQUIRE_FALSE(v.setByCSV(nullptr));
        REQUIRE(v == Vec3d(1.0, 2.0, 3.0));
    }

    SECTION("parser currently disabled")
    {
        Vec3d v(1.0, 2.0, 3.0);

        REQUIRE_FALSE(v.setByCSV("4,5,6"));
        REQUIRE(v == Vec3d(1.0, 2.0, 3.0));
    }

    SECTION("constructor currently leaves default values")
    {
        const Vec3d v("4,5,6");

        REQUIRE(v == Vec3d(0.0, 0.0, 0.0));
    }
}


TEST_CASE("Vec3 standard aliases", "[Vec3]")
{
    STATIC_REQUIRE(std::is_same_v<Vec3i, Vec3<int32_t>>);
    STATIC_REQUIRE(std::is_same_v<Vec3l, Vec3<int64_t>>);
    STATIC_REQUIRE(std::is_same_v<Vec3f, Vec3<float>>);
    STATIC_REQUIRE(std::is_same_v<Vec3d, Vec3<double>>);
}


TEST_CASE("Vec3 integer operations", "[Vec3]")
{
    Vec3i v(1, 2, 3);

    REQUIRE(v + Vec3i(4, 5, 6) == Vec3i(5, 7, 9));
    REQUIRE(v * 2 == Vec3i(2, 4, 6));

    v.translate(10, 20, 30);

    REQUIRE(v == Vec3i(11, 22, 33));
}