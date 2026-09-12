#include <grain/Math/Mat3.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <type_traits>

using namespace Grain;

TEST_CASE("Mat3 default construction", "[Mat3]")
{
    constexpr Mat3d m;

    REQUIRE(m(0, 0) == 1.0);
    REQUIRE(m(0, 1) == 0.0);
    REQUIRE(m(0, 2) == 0.0);

    REQUIRE(m(1, 0) == 0.0);
    REQUIRE(m(1, 1) == 1.0);
    REQUIRE(m(1, 2) == 0.0);

    REQUIRE(m(2, 0) == 0.0);
    REQUIRE(m(2, 1) == 0.0);
    REQUIRE(m(2, 2) == 1.0);
}


TEST_CASE("Mat3 explicit construction", "[Mat3]")
{
    constexpr Mat3d m{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    REQUIRE(m(0, 0) == 1.0);
    REQUIRE(m(0, 1) == 2.0);
    REQUIRE(m(0, 2) == 3.0);

    REQUIRE(m(1, 0) == 4.0);
    REQUIRE(m(1, 1) == 5.0);
    REQUIRE(m(1, 2) == 6.0);

    REQUIRE(m(2, 0) == 7.0);
    REQUIRE(m(2, 1) == 8.0);
    REQUIRE(m(2, 2) == 9.0);
}


TEST_CASE("Mat3 identity", "[Mat3]")
{
    constexpr auto m = Mat3d::identity();

    REQUIRE(m(0, 0) == 1.0);
    REQUIRE(m(0, 1) == 0.0);
    REQUIRE(m(0, 2) == 0.0);

    REQUIRE(m(1, 0) == 0.0);
    REQUIRE(m(1, 1) == 1.0);
    REQUIRE(m(1, 2) == 0.0);

    REQUIRE(m(2, 0) == 0.0);
    REQUIRE(m(2, 1) == 0.0);
    REQUIRE(m(2, 2) == 1.0);
}


TEST_CASE("Mat3 element access", "[Mat3]")
{
    Mat3d m;

    m(0, 1) = 2.0;
    m(1, 2) = 3.0;
    m(2, 0) = 4.0;

    REQUIRE(m(0, 1) == 2.0);
    REQUIRE(m(1, 2) == 3.0);
    REQUIRE(m(2, 0) == 4.0);
}


TEST_CASE("Mat3 const element access", "[Mat3]")
{
    constexpr Mat3d m{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    STATIC_REQUIRE(
        std::is_same_v<
            decltype(m(0, 0)),
            double
        >
    );

    REQUIRE(m(0, 0) == 1.0);
    REQUIRE(m(1, 1) == 5.0);
    REQUIRE(m(2, 2) == 9.0);
}


TEST_CASE("Mat3 data access", "[Mat3]")
{
    Mat3d m{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    double* data = m.data();

    REQUIRE(data[0] == 1.0);
    REQUIRE(data[1] == 2.0);
    REQUIRE(data[2] == 3.0);

    REQUIRE(data[3] == 4.0);
    REQUIRE(data[4] == 5.0);
    REQUIRE(data[5] == 6.0);

    REQUIRE(data[6] == 7.0);
    REQUIRE(data[7] == 8.0);
    REQUIRE(data[8] == 9.0);

    data[4] = 42.0;

    REQUIRE(m(1, 1) == 42.0);
}


TEST_CASE("Mat3 const data access", "[Mat3]")
{
    const Mat3d m{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    const double* data = m.data();

    REQUIRE(data[0] == 1.0);
    REQUIRE(data[4] == 5.0);
    REQUIRE(data[8] == 9.0);
}


TEST_CASE("Mat3 translation", "[Mat3]")
{
    constexpr auto m = Mat3d::translation(10.0, 20.0);

    REQUIRE(m(0, 0) == 1.0);
    REQUIRE(m(0, 1) == 0.0);
    REQUIRE(m(0, 2) == 10.0);

    REQUIRE(m(1, 0) == 0.0);
    REQUIRE(m(1, 1) == 1.0);
    REQUIRE(m(1, 2) == 20.0);

    REQUIRE(m(2, 0) == 0.0);
    REQUIRE(m(2, 1) == 0.0);
    REQUIRE(m(2, 2) == 1.0);
}


TEST_CASE("Mat3 scaling", "[Mat3]")
{
    SECTION("non-uniform")
    {
        constexpr auto m = Mat3d::scaling(2.0, 3.0);

        REQUIRE(m(0, 0) == 2.0);
        REQUIRE(m(1, 1) == 3.0);
        REQUIRE(m(2, 2) == 1.0);

        REQUIRE(m(0, 1) == 0.0);
        REQUIRE(m(0, 2) == 0.0);
        REQUIRE(m(1, 0) == 0.0);
        REQUIRE(m(1, 2) == 0.0);
    }

    SECTION("uniform")
    {
        constexpr auto m = Mat3d::scaling(4.0);

        REQUIRE(m(0, 0) == 4.0);
        REQUIRE(m(1, 1) == 4.0);
        REQUIRE(m(2, 2) == 1.0);
    }
}


TEST_CASE("Mat3 rotation", "[Mat3]")
{
    const auto m = Mat3d::rotation(
        std::numbers::pi / 2.0
    );

    REQUIRE(m(0, 0) == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(m(0, 1) == Catch::Approx(-1.0).margin(1e-12));

    REQUIRE(m(1, 0) == Catch::Approx(1.0).margin(1e-12));
    REQUIRE(m(1, 1) == Catch::Approx(0.0).margin(1e-12));

    REQUIRE(m(2, 2) == 1.0);
}


TEST_CASE("Mat3 rotationDegrees", "[Mat3]")
{
    const auto m = Mat3d::rotationDegrees(90.0);

    REQUIRE(m(0, 0) == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(m(0, 1) == Catch::Approx(-1.0).margin(1e-12));

    REQUIRE(m(1, 0) == Catch::Approx(1.0).margin(1e-12));
    REQUIRE(m(1, 1) == Catch::Approx(0.0).margin(1e-12));

    REQUIRE(m(2, 2) == 1.0);
}


TEST_CASE("Mat3 transforms Vec2", "[Mat3]")
{
    SECTION("identity")
    {
        constexpr Mat3d m;
        constexpr Vec2d v{3.0, 4.0};

        const auto result = m.transform(v);

        REQUIRE(result.x == 3.0);
        REQUIRE(result.y == 4.0);
    }

    SECTION("translation")
    {
        constexpr auto m = Mat3d::translation(10.0, 20.0);
        constexpr Vec2d v{3.0, 4.0};

        const auto result = m.transform(v);

        REQUIRE(result.x == 13.0);
        REQUIRE(result.y == 24.0);
    }

    SECTION("scaling")
    {
        constexpr auto m = Mat3d::scaling(2.0, 3.0);
        constexpr Vec2d v{4.0, 5.0};

        const auto result = m.transform(v);

        REQUIRE(result.x == 8.0);
        REQUIRE(result.y == 15.0);
    }

    SECTION("rotation")
    {
        const auto m = Mat3d::rotationDegrees(90.0);
        constexpr Vec2d v{1.0, 0.0};

        const auto result = m.transform(v);

        REQUIRE(result.x == Catch::Approx(0.0).margin(1e-12));
        REQUIRE(result.y == Catch::Approx(1.0).margin(1e-12));
    }
}


TEST_CASE("Mat3 transforms Vec3", "[Mat3]")
{
    constexpr Mat3d m{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    constexpr Vec3d v{1.0, 2.0, 3.0};

    const auto result = m.transform(v);

    REQUIRE(result.x == 14.0);
    REQUIRE(result.y == 32.0);
    REQUIRE(result.z == 50.0);
}


TEST_CASE("Mat3 matrix multiplication", "[Mat3]")
{
    constexpr Mat3d a{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    constexpr Mat3d b{
        9.0, 8.0, 7.0,
        6.0, 5.0, 4.0,
        3.0, 2.0, 1.0
    };

    constexpr auto result = a * b;

    REQUIRE(result(0, 0) == 30.0);
    REQUIRE(result(0, 1) == 24.0);
    REQUIRE(result(0, 2) == 18.0);

    REQUIRE(result(1, 0) == 84.0);
    REQUIRE(result(1, 1) == 69.0);
    REQUIRE(result(1, 2) == 54.0);

    REQUIRE(result(2, 0) == 138.0);
    REQUIRE(result(2, 1) == 114.0);
    REQUIRE(result(2, 2) == 90.0);
}


TEST_CASE("Mat3 matrix multiplication assignment", "[Mat3]")
{
    Mat3d a{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    constexpr Mat3d b{
        9.0, 8.0, 7.0,
        6.0, 5.0, 4.0,
        3.0, 2.0, 1.0
    };

    a *= b;

    REQUIRE(a(0, 0) == 30.0);
    REQUIRE(a(0, 1) == 24.0);
    REQUIRE(a(0, 2) == 18.0);

    REQUIRE(a(1, 0) == 84.0);
    REQUIRE(a(1, 1) == 69.0);
    REQUIRE(a(1, 2) == 54.0);

    REQUIRE(a(2, 0) == 138.0);
    REQUIRE(a(2, 1) == 114.0);
    REQUIRE(a(2, 2) == 90.0);
}


TEST_CASE("Mat3 determinant", "[Mat3]")
{
    SECTION("identity")
    {
        constexpr Mat3d m;
        REQUIRE(m.determinant() == 1.0);
    }

    SECTION("known matrix")
    {
        constexpr Mat3d m{
            1.0, 2.0, 3.0,
            0.0, 1.0, 4.0,
            5.0, 6.0, 0.0
        };

        REQUIRE(m.determinant() == 1.0);
    }

    SECTION("scaling")
    {
        constexpr auto m = Mat3d::scaling(2.0, 3.0);

        REQUIRE(m.determinant() == 6.0);
    }

    SECTION("translation")
    {
        constexpr auto m = Mat3d::translation(10.0, 20.0);

        REQUIRE(m.determinant() == 1.0);
    }
}


TEST_CASE("Mat3 transpose", "[Mat3]")
{
    Mat3d m{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    m.transpose();

    REQUIRE(m(0, 0) == 1.0);
    REQUIRE(m(0, 1) == 4.0);
    REQUIRE(m(0, 2) == 7.0);

    REQUIRE(m(1, 0) == 2.0);
    REQUIRE(m(1, 1) == 5.0);
    REQUIRE(m(1, 2) == 8.0);

    REQUIRE(m(2, 0) == 3.0);
    REQUIRE(m(2, 1) == 6.0);
    REQUIRE(m(2, 2) == 9.0);
}


TEST_CASE("Mat3 transposed returns a new matrix", "[Mat3]")
{
    constexpr Mat3d m{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    constexpr auto result = m.transposed();

    REQUIRE(result(0, 0) == 1.0);
    REQUIRE(result(0, 1) == 4.0);
    REQUIRE(result(0, 2) == 7.0);

    REQUIRE(result(1, 0) == 2.0);
    REQUIRE(result(1, 1) == 5.0);
    REQUIRE(result(1, 2) == 8.0);

    REQUIRE(result(2, 0) == 3.0);
    REQUIRE(result(2, 1) == 6.0);
    REQUIRE(result(2, 2) == 9.0);

    // Original remains unchanged.
    REQUIRE(m(0, 1) == 2.0);
    REQUIRE(m(1, 0) == 4.0);
}


TEST_CASE("Mat3 transformation composition", "[Mat3]")
{
    // Matrix multiplication is applied right-to-left:
    //
    // scale -> translate
    //
    // v = (1, 2)
    // scale by 2 -> (2, 4)
    // translate by (10, 20) -> (12, 24)

    constexpr auto scale = Mat3d::scaling(2.0);
    constexpr auto translate = Mat3d::translation(10.0, 20.0);

    constexpr auto transform = translate * scale;

    constexpr Vec2d v{1.0, 2.0};

    const auto result = transform.transform(v);

    REQUIRE(result.x == 12.0);
    REQUIRE(result.y == 24.0);
}


TEST_CASE("Mat3 identity multiplication", "[Mat3]")
{
    constexpr Mat3d m{
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    constexpr Mat3d identity;

    constexpr auto left = identity * m;
    constexpr auto right = m * identity;

    REQUIRE(left(0, 0) == m(0, 0));
    REQUIRE(left(0, 1) == m(0, 1));
    REQUIRE(left(0, 2) == m(0, 2));
    REQUIRE(left(1, 0) == m(1, 0));
    REQUIRE(left(1, 1) == m(1, 1));
    REQUIRE(left(1, 2) == m(1, 2));
    REQUIRE(left(2, 0) == m(2, 0));
    REQUIRE(left(2, 1) == m(2, 1));
    REQUIRE(left(2, 2) == m(2, 2));

    REQUIRE(right(0, 0) == m(0, 0));
    REQUIRE(right(0, 1) == m(0, 1));
    REQUIRE(right(0, 2) == m(0, 2));
    REQUIRE(right(1, 0) == m(1, 0));
    REQUIRE(right(1, 1) == m(1, 1));
    REQUIRE(right(1, 2) == m(1, 2));
    REQUIRE(right(2, 0) == m(2, 0));
    REQUIRE(right(2, 1) == m(2, 1));
    REQUIRE(right(2, 2) == m(2, 2));
}


TEST_CASE("Mat3 rotation preserves vector length", "[Mat3]")
{
    constexpr Vec2d v{3.0, 4.0};

    const auto m = Mat3d::rotationDegrees(37.0);
    const auto result = m.transform(v);

    REQUIRE(
        std::hypot(result.x, result.y)
        == Catch::Approx(5.0)
    );
}


TEST_CASE("Mat3 aliases", "[Mat3]")
{
    STATIC_REQUIRE(std::is_same_v<Mat3f, Mat3<float>>);
    STATIC_REQUIRE(std::is_same_v<Mat3d, Mat3<double>>);
}