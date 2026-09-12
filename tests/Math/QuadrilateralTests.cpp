#include <grain/Math/Quadrilateral.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <type_traits>

using namespace Grain;

namespace {

constexpr double epsilon = 1e-12;

void requirePoint(
    const Vec2d& actual,
    double x,
    double y,
    double margin = epsilon
)
{
    REQUIRE(actual.x == Catch::Approx(x).margin(margin));
    REQUIRE(actual.y == Catch::Approx(y).margin(margin));
}

} // namespace


//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral default construction", "[Quadrilateral]")
{
    const Quadrilateral q;

    REQUIRE_FALSE(q.isValid());

    requirePoint(q.p1(), -1.0, -1.0);
    requirePoint(q.p2(),  1.0, -1.0);
    requirePoint(q.p3(),  1.0,  1.0);
    requirePoint(q.p4(), -1.0,  1.0);
}


TEST_CASE("Quadrilateral construction from four points", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{4.0, 0.0},
        Vec2d{4.0, 3.0},
        Vec2d{0.0, 3.0}
    };

    requirePoint(q.p1(), 0.0, 0.0);
    requirePoint(q.p2(), 4.0, 0.0);
    requirePoint(q.p3(), 4.0, 3.0);
    requirePoint(q.p4(), 0.0, 3.0);

    REQUIRE(q.isValid());
}


TEST_CASE("Quadrilateral construction from min and max", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{10.0, 20.0},
        Vec2d{30.0, 40.0}
    };

    requirePoint(q.p1(), 10.0, 20.0);
    requirePoint(q.p2(), 30.0, 20.0);
    requirePoint(q.p3(), 30.0, 40.0);
    requirePoint(q.p4(), 10.0, 40.0);

    REQUIRE(q.isValid());
}


TEST_CASE("Quadrilateral copy construction", "[Quadrilateral]")
{
    const Quadrilateral original{
        Vec2d{0.0, 0.0},
        Vec2d{4.0, 0.0},
        Vec2d{4.0, 3.0},
        Vec2d{0.0, 3.0}
    };

    const Quadrilateral copy = original;

    REQUIRE(copy.p1() == original.p1());
    REQUIRE(copy.p2() == original.p2());
    REQUIRE(copy.p3() == original.p3());
    REQUIRE(copy.p4() == original.p4());
}


//------------------------------------------------------------------------------
// Point access
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral point access", "[Quadrilateral]")
{
    Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{4.0, 0.0},
        Vec2d{4.0, 3.0},
        Vec2d{0.0, 3.0}
    };

    REQUIRE(q.point(0) == q.p1());
    REQUIRE(q.point(1) == q.p2());
    REQUIRE(q.point(2) == q.p3());
    REQUIRE(q.point(3) == q.p4());

    q.point(0) = Vec2d{1.0, 2.0};

    REQUIRE(q.p1() == Vec2d{1.0, 2.0});
}


TEST_CASE("Quadrilateral points access", "[Quadrilateral]")
{
    Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{4.0, 0.0},
        Vec2d{4.0, 3.0},
        Vec2d{0.0, 3.0}
    };

    auto& points = q.points();

    points[0] = Vec2d{1.0, 1.0};

    REQUIRE(q.p1() == Vec2d{1.0, 1.0});
}


TEST_CASE("Quadrilateral set", "[Quadrilateral]")
{
    Quadrilateral q;

    q.set(
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 20.0},
        Vec2d{0.0, 20.0}
    );

    REQUIRE(q.isValid());

    requirePoint(q.p1(),  0.0,  0.0);
    requirePoint(q.p2(), 10.0,  0.0);
    requirePoint(q.p3(), 10.0, 20.0);
    requirePoint(q.p4(),  0.0, 20.0);
}


TEST_CASE("Quadrilateral setPoint", "[Quadrilateral]")
{
    Quadrilateral q;

    q.setPoint(2, Vec2d{42.0, 43.0});

    REQUIRE(q.p3() == Vec2d{42.0, 43.0});
}


TEST_CASE("Quadrilateral setByMinMax", "[Quadrilateral]")
{
    Quadrilateral q;

    q.setByMinMax(
        Vec2d{10.0, 20.0},
        Vec2d{30.0, 50.0}
    );

    requirePoint(q.p1(), 10.0, 20.0);
    requirePoint(q.p2(), 30.0, 20.0);
    requirePoint(q.p3(), 30.0, 50.0);
    requirePoint(q.p4(), 10.0, 50.0);

    REQUIRE(q.isValid());
}


//------------------------------------------------------------------------------
// Geometry
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral rectangle geometry", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{4.0, 0.0},
        Vec2d{4.0, 3.0},
        Vec2d{0.0, 3.0}
    };

    REQUIRE(q.isSimple());
    REQUIRE(q.isConvex());

    REQUIRE(q.area() == Catch::Approx(12.0).margin(epsilon));

    REQUIRE(
        q.flattestAngle()
        == Catch::Approx(90.0).margin(epsilon)
    );

    requirePoint(q.centroid(), 2.0, 1.5);
}


TEST_CASE("Quadrilateral square geometry", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{-1.0, -1.0},
        Vec2d{ 1.0, -1.0},
        Vec2d{ 1.0,  1.0},
        Vec2d{-1.0,  1.0}
    };

    REQUIRE(q.area() == Catch::Approx(4.0).margin(epsilon));
    REQUIRE(q.isSimple());
    REQUIRE(q.isConvex());

    REQUIRE(
        q.flattestAngle()
        == Catch::Approx(90.0).margin(epsilon)
    );

    requirePoint(q.centroid(), 0.0, 0.0);
}


TEST_CASE("Quadrilateral contains points", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{4.0, 0.0},
        Vec2d{4.0, 3.0},
        Vec2d{0.0, 3.0}
    };

    REQUIRE(q.contains(Vec2d{2.0, 1.0}));
    REQUIRE(q.contains(Vec2d{0.0, 0.0}));
    REQUIRE(q.contains(Vec2d{4.0, 3.0}));

    REQUIRE_FALSE(q.contains(Vec2d{-1.0, 1.0}));
    REQUIRE_FALSE(q.contains(Vec2d{5.0, 1.0}));
    REQUIRE_FALSE(q.contains(Vec2d{2.0, 4.0}));
}


TEST_CASE("Quadrilateral detects non-convex shape", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{4.0, 0.0},
        Vec2d{1.0, 1.0},
        Vec2d{0.0, 4.0}
    };

    REQUIRE(q.isSimple());
    REQUIRE_FALSE(q.isConvex());
}


//------------------------------------------------------------------------------
// Projection
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral project unit square", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{-1.0, -1.0},
        Vec2d{ 1.0, -1.0},
        Vec2d{ 1.0,  1.0},
        Vec2d{-1.0,  1.0}
    };

    requirePoint(q.project(0.0, 0.0), -1.0, -1.0);
    requirePoint(q.project(1.0, 0.0),  1.0, -1.0);
    requirePoint(q.project(1.0, 1.0),  1.0,  1.0);
    requirePoint(q.project(0.0, 1.0), -1.0,  1.0);

    requirePoint(q.project(0.5, 0.5), 0.0, 0.0);
}


TEST_CASE("Quadrilateral project Vec2", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 20.0},
        Vec2d{0.0, 20.0}
    };

    requirePoint(q.project(Vec2d{0.0, 0.0}), 0.0, 0.0);
    requirePoint(q.project(Vec2d{1.0, 0.0}), 10.0, 0.0);
    requirePoint(q.project(Vec2d{1.0, 1.0}), 10.0, 20.0);
    requirePoint(q.project(Vec2d{0.0, 1.0}), 0.0, 20.0);

    requirePoint(q.project(Vec2d{0.5, 0.5}), 5.0, 10.0);
}


TEST_CASE("Quadrilateral project output parameter", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 20.0},
        Vec2d{0.0, 20.0}
    };

    Vec2d result;

    REQUIRE(
        q.project(Vec2d{0.25, 0.75}, result)
    );

    requirePoint(result, 2.5, 15.0);
}


//------------------------------------------------------------------------------
// Mapping
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral map rectangle", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 20.0},
        Vec2d{0.0, 20.0}
    };

    requirePoint(q.map(Vec2d{0.0, 0.0}), 0.0, 0.0);
    requirePoint(q.map(Vec2d{10.0, 0.0}), 1.0, 0.0);
    requirePoint(q.map(Vec2d{10.0, 20.0}), 1.0, 1.0);
    requirePoint(q.map(Vec2d{0.0, 20.0}), 0.0, 1.0);

    requirePoint(q.map(Vec2d{5.0, 10.0}), 0.5, 0.5);
}


TEST_CASE("Quadrilateral map and project are inverses", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 2.0},
        Vec2d{8.0, 12.0},
        Vec2d{-1.0, 9.0}
    };

    const Vec2d uv{0.25, 0.75};

    const Vec2d position = q.project(uv);
    const Vec2d result = q.map(position);

    requirePoint(result, uv.x, uv.y);
}


TEST_CASE("Quadrilateral project and map overloads", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 20.0},
        Vec2d{0.0, 20.0}
    };

    const Vec2d projected = q.project(0.25, 0.75);

    const Vec2d mapped = q.map(
        projected.x,
        projected.y
    );

    requirePoint(mapped, 0.25, 0.75);

    Vec2d result;

    REQUIRE(
        q.map(
            projected.x,
            projected.y,
            result
        )
    );

    requirePoint(result, 0.25, 0.75);
}


TEST_CASE("Quadrilateral projectPoints", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 20.0},
        Vec2d{0.0, 20.0}
    };

    Vec2d points[] = {
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 1.0},
        {0.0, 1.0},
        {0.5, 0.5}
    };

    q.projectPoints(points, 5);

    requirePoint(points[0], 0.0, 0.0);
    requirePoint(points[1], 10.0, 0.0);
    requirePoint(points[2], 10.0, 20.0);
    requirePoint(points[3], 0.0, 20.0);
    requirePoint(points[4], 5.0, 10.0);
}


//------------------------------------------------------------------------------
// Lines
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral horizontalLine", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 20.0},
        Vec2d{0.0, 20.0}
    };

    const Lined line = q.horizontalLine(0.25);

    requirePoint(line.p1(), 0.0, 5.0);
    requirePoint(line.p2(), 10.0, 5.0);
}


TEST_CASE("Quadrilateral verticalLine", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 20.0},
        Vec2d{0.0, 20.0}
    };

    const Lined line = q.verticalLine(0.25);

    requirePoint(line.p1(), 2.5, 0.0);
    requirePoint(line.p2(), 2.5, 20.0);
}


//------------------------------------------------------------------------------
// Bézier circle
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral bezierCirclePoints", "[Quadrilateral]")
{
    const Quadrilateral q{
        Vec2d{-1.0, -1.0},
        Vec2d{ 1.0, -1.0},
        Vec2d{ 1.0,  1.0},
        Vec2d{-1.0,  1.0}
    };

    const auto points = q.bezierCirclePoints();

    REQUIRE(points.size() == 12);

    for (const auto& point : points) {
        REQUIRE(std::isfinite(point.x));
        REQUIRE(std::isfinite(point.y));
    }
}


//------------------------------------------------------------------------------
// Remap
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral remap", "[Quadrilateral]")
{
    Quadrilateral q{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{0.0, 10.0}
    };

    const RemapRectd remap{
        Rectd{0.0, 0.0, 10.0, 10.0},
        Rectd{0.0, 0.0, 100.0, 200.0}
    };

    q.remap(remap);

    constexpr double epsilon = 1e-12;

    REQUIRE(q.p1().x == Catch::Approx(0.0).margin(epsilon));
    REQUIRE(q.p1().y == Catch::Approx(0.0).margin(epsilon));

    REQUIRE(q.p2().x == Catch::Approx(100.0).margin(epsilon));
    REQUIRE(q.p2().y == Catch::Approx(0.0).margin(epsilon));

    REQUIRE(q.p3().x == Catch::Approx(100.0).margin(epsilon));
    REQUIRE(q.p3().y == Catch::Approx(200.0).margin(epsilon));

    REQUIRE(q.p4().x == Catch::Approx(0.0).margin(epsilon));
    REQUIRE(q.p4().y == Catch::Approx(200.0).margin(epsilon));
}


//------------------------------------------------------------------------------
// Type/API checks
//------------------------------------------------------------------------------

TEST_CASE("Quadrilateral point array type", "[Quadrilateral]")
{
    STATIC_REQUIRE(
        std::is_same_v<
            Quadrilateral::PointArray,
            std::array<Vec2d, 4>
        >
    );
}


TEST_CASE("Quadrilateral points are mutable", "[Quadrilateral]")
{
    Quadrilateral q;

    static_assert(
        std::is_same_v<
            decltype(q.points()),
            Quadrilateral::PointArray&
        >
    );

    static_assert(
        std::is_same_v<
            decltype(std::as_const(q).points()),
            const Quadrilateral::PointArray&
        >
    );
}