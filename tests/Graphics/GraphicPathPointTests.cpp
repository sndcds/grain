#include <grain/Graphics/GraphicPathPoint.hpp>
#include <grain/Math/Quadrilateral.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Grain;

namespace {

constexpr double epsilon = 1e-9;

void requirePoint(
    const Vec2d& actual,
    const Vec2d& expected,
    double tolerance = epsilon
)
{
    REQUIRE_THAT(
        actual.x,
        Catch::Matchers::WithinAbs(expected.x, tolerance)
    );

    REQUIRE_THAT(
        actual.y,
        Catch::Matchers::WithinAbs(expected.y, tolerance)
    );
}

} // namespace


TEST_CASE("GraphicPathPoint default constructor", "[GraphicPathPoint]")
{
    const GraphicPathPoint point;

    requirePoint(point.anchor, Vec2d{0.0, 0.0});
    requirePoint(point.left, Vec2d{0.0, 0.0});
    requirePoint(point.right, Vec2d{0.0, 0.0});

    REQUIRE_FALSE(point.leftFlag);
    REQUIRE_FALSE(point.rightFlag);
    REQUIRE(point.bezierSegmentLength == 0.0);
}


TEST_CASE("GraphicPathPoint coordinate constructor", "[GraphicPathPoint]")
{
    const GraphicPathPoint point{
        10.0,
        20.0
    };

    requirePoint(point.anchor, Vec2d{10.0, 20.0});
    requirePoint(point.left, Vec2d{10.0, 20.0});
    requirePoint(point.right, Vec2d{10.0, 20.0});

    REQUIRE_FALSE(point.leftFlag);
    REQUIRE_FALSE(point.rightFlag);
    REQUIRE(point.bezierSegmentLength == 0.0);
}


TEST_CASE("GraphicPathPoint constructor detects handles", "[GraphicPathPoint]")
{
    const GraphicPathPoint point{
        10.0, 20.0,
        5.0, 20.0,
        15.0, 20.0
    };

    requirePoint(point.anchor, Vec2d{10.0, 20.0});
    requirePoint(point.left, Vec2d{5.0, 20.0});
    requirePoint(point.right, Vec2d{15.0, 20.0});

    REQUIRE(point.leftFlag);
    REQUIRE(point.rightFlag);
}


TEST_CASE("GraphicPathPoint constructor disables coincident handles", "[GraphicPathPoint]")
{
    const GraphicPathPoint point{
        10.0, 20.0,
        10.0, 20.0,
        10.0, 20.0
    };

    REQUIRE_FALSE(point.leftFlag);
    REQUIRE_FALSE(point.rightFlag);
}


TEST_CASE("GraphicPathPoint constructor detects handles independently", "[GraphicPathPoint]")
{
    const GraphicPathPoint point{
        10.0, 20.0,
        10.0, 20.0,
        15.0, 20.0
    };

    REQUIRE_FALSE(point.leftFlag);
    REQUIRE(point.rightFlag);
}


TEST_CASE("GraphicPathPoint explicit flags constructor", "[GraphicPathPoint]")
{
    const GraphicPathPoint point{
        10.0, 20.0,
        false,
        5.0, 20.0,
        true,
        15.0, 20.0
    };

    requirePoint(point.anchor, Vec2d{10.0, 20.0});
    requirePoint(point.left, Vec2d{5.0, 20.0});
    requirePoint(point.right, Vec2d{15.0, 20.0});

    REQUIRE_FALSE(point.leftFlag);
    REQUIRE(point.rightFlag);
}


TEST_CASE("GraphicPathPoint Vec2 constructor", "[GraphicPathPoint]")
{
    const GraphicPathPoint point{
        Vec2d{10.0, 20.0},
        true,
        Vec2d{5.0, 20.0},
        false,
        Vec2d{15.0, 20.0}
    };

    requirePoint(point.anchor, Vec2d{10.0, 20.0});
    requirePoint(point.left, Vec2d{5.0, 20.0});
    requirePoint(point.right, Vec2d{15.0, 20.0});

    REQUIRE(point.leftFlag);
    REQUIRE_FALSE(point.rightFlag);
}


TEST_CASE("GraphicPathPoint translate by vector", "[GraphicPathPoint]")
{
    GraphicPathPoint point{
        10.0, 20.0,
        5.0, 15.0,
        15.0, 25.0
    };

    point.translate(Vec2d{3.0, -4.0});

    requirePoint(point.anchor, Vec2d{13.0, 16.0});
    requirePoint(point.left, Vec2d{8.0, 11.0});
    requirePoint(point.right, Vec2d{18.0, 21.0});

    REQUIRE(point.leftFlag);
    REQUIRE(point.rightFlag);
}


TEST_CASE("GraphicPathPoint translate by coordinates", "[GraphicPathPoint]")
{
    GraphicPathPoint point{
        10.0, 20.0,
        5.0, 15.0,
        15.0, 25.0
    };

    point.translate(3.0, -4.0);

    requirePoint(point.anchor, Vec2d{13.0, 16.0});
    requirePoint(point.left, Vec2d{8.0, 11.0});
    requirePoint(point.right, Vec2d{18.0, 21.0});

    REQUIRE(point.leftFlag);
    REQUIRE(point.rightFlag);
}


TEST_CASE("GraphicPathPoint translation moves all positions equally", "[GraphicPathPoint]")
{
    GraphicPathPoint point{
        10.0, 20.0,
        5.0, 15.0,
        15.0, 25.0
    };

    const Vec2d anchorBefore = point.anchor;
    const Vec2d leftBefore = point.left;
    const Vec2d rightBefore = point.right;

    const Vec2d offset{7.0, -3.0};

    point.translate(offset);

    requirePoint(point.anchor, anchorBefore + offset);
    requirePoint(point.left, leftBefore + offset);
    requirePoint(point.right, rightBefore + offset);
}


TEST_CASE("GraphicPathPoint rotate", "[GraphicPathPoint]")
{
    GraphicPathPoint point{
        10.0, 10.0,
        20.0, 10.0,
        10.0, 20.0
    };

    point.rotateDegrees(90.0);

    requirePoint(
        point.anchor,
        Vec2d{10.0, 10.0}
    );

    requirePoint(
        point.left,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        point.right,
        Vec2d{0.0, 10.0}
    );
}


TEST_CASE("GraphicPathPoint rotate preserves anchor", "[GraphicPathPoint]")
{
    GraphicPathPoint point{
        10.0, 20.0,
        20.0, 20.0,
        10.0, 30.0
    };

    const Vec2d anchorBefore = point.anchor;

    point.rotate(37.0);

    requirePoint(point.anchor, anchorBefore);
}


TEST_CASE("GraphicPathPoint rotate preserves handle distances", "[GraphicPathPoint]")
{
    GraphicPathPoint point{
        10.0, 20.0,
        20.0, 20.0,
        10.0, 30.0
    };

    const double leftDistance = point.anchor.distance(point.left);
    const double rightDistance = point.anchor.distance(point.right);

    point.rotate(123.0);

    REQUIRE_THAT(
        point.anchor.distance(point.left),
        Catch::Matchers::WithinAbs(leftDistance, epsilon)
    );

    REQUIRE_THAT(
        point.anchor.distance(point.right),
        Catch::Matchers::WithinAbs(rightDistance, epsilon)
    );
}


TEST_CASE("GraphicPathPoint projectToQuadrilateral", "[GraphicPathPoint]")
{
    const Quadrilateral quadrilateral{
        Vec2d{0.0, 0.0},
        Vec2d{100.0, 0.0},
        Vec2d{100.0, 100.0},
        Vec2d{0.0, 100.0}
    };

    GraphicPathPoint point{
        0.25, 0.25,
        0.10, 0.25,
        0.40, 0.25
    };

    point.projectToQuadrilateral(quadrilateral);

    requirePoint(
        point.anchor,
        Vec2d{25.0, 25.0}
    );

    requirePoint(
        point.left,
        Vec2d{10.0, 25.0}
    );

    requirePoint(
        point.right,
        Vec2d{40.0, 25.0}
    );
}


TEST_CASE("GraphicPathPoint projectToQuadrilateral with matrix", "[GraphicPathPoint]")
{
    const Quadrilateral quadrilateral{
        Vec2d{0.0, 0.0},
        Vec2d{100.0, 0.0},
        Vec2d{100.0, 100.0},
        Vec2d{0.0, 100.0}
    };

    const Mat3d matrix = Mat3d::translation(10.0, 20.0);

    GraphicPathPoint point{
        0.25, 0.25,
        0.10, 0.25,
        0.40, 0.25
    };

    point.projectToQuadrilateral(quadrilateral, &matrix);

    requirePoint(
        point.anchor,
        Vec2d{35.0, 45.0}
    );

    requirePoint(
        point.left,
        Vec2d{20.0, 45.0}
    );

    requirePoint(
        point.right,
        Vec2d{50.0, 45.0}
    );
}