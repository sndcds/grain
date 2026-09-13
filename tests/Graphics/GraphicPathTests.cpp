#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Graphics/GraphicPath.hpp>
#include <grain/Geometry/Bezier.hpp>
#include <grain/Math/Mat3.hpp>
#include <grain/Math/Quadrilateral.hpp>

#include <cmath>

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

void requireRect(
    const Rectd& actual,
    const Rectd& expected,
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

    REQUIRE_THAT(
        actual.width,
        Catch::Matchers::WithinAbs(expected.width, tolerance)
    );

    REQUIRE_THAT(
        actual.height,
        Catch::Matchers::WithinAbs(expected.height, tolerance)
    );
}

} // namespace

//------------------------------------------------------------------------------
// State
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath default state",
    "[GraphicPath]"
)
{
    GraphicPath path;

    REQUIRE_FALSE(path.hasPoints());
    REQUIRE(path.pointCount() == 0);
    REQUIRE(path.lastPointIndex() == -1);
    REQUIRE(path.segmentCount() == 0);
    REQUIRE_FALSE(path.isClosed());
    REQUIRE(path.bezierSegmentResolution() == 20);
    REQUIRE_THAT(
        path.length(),
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );
}

TEST_CASE(
    "GraphicPath point and segment counts",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    REQUIRE(path.pointCount() == 1);
    REQUIRE(path.lastPointIndex() == 0);
    REQUIRE(path.segmentCount() == 0);

    path.addPoint(10.0, 0.0);
    REQUIRE(path.pointCount() == 2);
    REQUIRE(path.lastPointIndex() == 1);
    REQUIRE(path.segmentCount() == 1);

    path.addPoint(10.0, 10.0);
    REQUIRE(path.pointCount() == 3);
    REQUIRE(path.lastPointIndex() == 2);
    REQUIRE(path.segmentCount() == 2);

    path.setClosed(true);

    REQUIRE(path.segmentCount() == 3);
}

TEST_CASE(
    "GraphicPath closed state",
    "[GraphicPath]"
)
{
    GraphicPath path;

    REQUIRE_FALSE(path.isClosed());

    path.setClosed(true);
    REQUIRE(path.isClosed());

    path.setClosed(false);
    REQUIRE_FALSE(path.isClosed());
}

//------------------------------------------------------------------------------
// Point access
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath pointPtrAtIndex",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(10.0, 20.0);
    path.addPoint(30.0, 40.0);

    auto* first = path.pointPtrAtIndex(0);
    auto* second = path.pointPtrAtIndex(1);

    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);

    requirePoint(
        first->anchor,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        second->anchor,
        Vec2d{30.0, 40.0}
    );

    REQUIRE(path.pointPtrAtIndex(-1) == nullptr);
    REQUIRE(path.pointPtrAtIndex(2) == nullptr);
}

TEST_CASE(
    "GraphicPath lastPointPtr",
    "[GraphicPath]"
)
{
    GraphicPath path;

    REQUIRE(path.lastPointPtr() == nullptr);

    path.addPoint(10.0, 20.0);
    path.addPoint(30.0, 40.0);

    auto* last = path.lastPointPtr();

    REQUIRE(last != nullptr);

    requirePoint(
        last->anchor,
        Vec2d{30.0, 40.0}
    );
}

TEST_CASE(
    "GraphicPath const point access",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(10.0, 20.0);

    const GraphicPath& constPath = path;

    const auto* point =
        constPath.pointPtrAtIndex(0);

    const auto* last =
        constPath.lastPointPtr();

    REQUIRE(point != nullptr);
    REQUIRE(last != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        last->anchor,
        Vec2d{10.0, 20.0}
    );
}

//------------------------------------------------------------------------------
// Point insertion
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath addPoint position",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(Vec2d{10.0, 20.0});

    REQUIRE(path.pointCount() == 1);

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );

    REQUIRE_FALSE(point->leftFlag);
    REQUIRE_FALSE(point->rightFlag);
}

TEST_CASE(
    "GraphicPath addPoint coordinate overload",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(10.0, 20.0);

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );
}

TEST_CASE(
    "GraphicPath addPoint with handles",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        Vec2d{10.0, 20.0},
        Vec2d{5.0, 20.0},
        Vec2d{15.0, 20.0}
    );

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        point->left,
        Vec2d{5.0, 20.0}
    );

    requirePoint(
        point->right,
        Vec2d{15.0, 20.0}
    );

    REQUIRE(point->leftFlag);
    REQUIRE(point->rightFlag);
}

TEST_CASE(
    "GraphicPath addPoint explicit handles and flags",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        10.0, 20.0,
        false,
        5.0, 20.0,
        true,
        15.0, 20.0
    );

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    REQUIRE_FALSE(point->leftFlag);
    REQUIRE(point->rightFlag);

    requirePoint(
        point->left,
        Vec2d{5.0, 20.0}
    );

    requirePoint(
        point->right,
        Vec2d{15.0, 20.0}
    );
}

TEST_CASE(
    "GraphicPath addPoint pointer copies point",
    "[GraphicPath]"
)
{
    GraphicPath path;

    GraphicPathPoint source{
        10.0, 20.0,
        5.0, 15.0,
        30.0, 25.0
    };

    path.addPoint(&source);

    REQUIRE(path.pointCount() == 1);

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        source.anchor
    );

    requirePoint(
        point->left,
        source.left
    );

    requirePoint(
        point->right,
        source.right
    );

    REQUIRE(point->leftFlag == source.leftFlag);
    REQUIRE(point->rightFlag == source.rightFlag);
}

TEST_CASE(
    "GraphicPath addPoint null pointer does nothing",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        static_cast<const GraphicPathPoint*>(nullptr)
    );

    REQUIRE(path.pointCount() == 0);
}

//------------------------------------------------------------------------------
// Left/right points
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath addPointLeft",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPointLeft(
        Vec2d{10.0, 20.0},
        Vec2d{5.0, 15.0}
    );

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        point->left,
        Vec2d{5.0, 15.0}
    );

    REQUIRE(point->leftFlag);
    REQUIRE_FALSE(point->rightFlag);
}

TEST_CASE(
    "GraphicPath addPointRight",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPointRight(
        Vec2d{10.0, 20.0},
        Vec2d{15.0, 25.0}
    );

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        point->right,
        Vec2d{15.0, 25.0}
    );

    REQUIRE_FALSE(point->leftFlag);
    REQUIRE(point->rightFlag);
}

//------------------------------------------------------------------------------
// Point by angle
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath addPointByAngle symmetric handles",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPointByAngleDegrees(
        Vec2d{10.0, 20.0},
        0.0,
        5.0,
        10.0
    );

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        point->left,
        Vec2d{5.0, 20.0}
    );

    requirePoint(
        point->right,
        Vec2d{20.0, 20.0}
    );

    REQUIRE(point->leftFlag);
    REQUIRE(point->rightFlag);
}

TEST_CASE(
    "GraphicPath addPointByAngle rotates handles",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPointByAngleDegrees(
        Vec2d{10.0, 20.0},
        90.0,
        5.0,
        10.0
    );

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->left,
        Vec2d{10.0, 15.0}
    );

    requirePoint(
        point->right,
        Vec2d{10.0, 30.0}
    );
}

TEST_CASE(
    "GraphicPath addPointByAngle independent handle angles",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPointByAngleDegrees(
        Vec2d{10.0, 20.0},
        90.0,
        5.0,
        180.0,
        10.0
    );

    const auto* point = path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->left,
        Vec2d{10.0, 15.0}
    );

    requirePoint(
        point->right,
        Vec2d{0.0, 20.0}
    );
}

//------------------------------------------------------------------------------
// Bézier creation
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath bezierAtIndex creates line segment",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(20.0, 10.0);

    Bezier bezier;

    REQUIRE(path.bezierAtIndex(0, bezier));

    requirePoint(
        bezier.startPos(),
        Vec2d{0.0, 0.0}
    );

    requirePoint(
        bezier.controlPos1(),
        Vec2d{0.0, 0.0}
    );

    requirePoint(
        bezier.controlPos2(),
        Vec2d{20.0, 10.0}
    );

    requirePoint(
        bezier.endPos(),
        Vec2d{20.0, 10.0}
    );
}

TEST_CASE(
    "GraphicPath bezierAtIndex uses right and left handles",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        0.0, 0.0,
        false,
        0.0, 0.0,
        true,
        5.0, 10.0
    );

    path.addPoint(
        20.0, 20.0,
        true,
        15.0, 10.0,
        false,
        20.0, 20.0
    );

    Bezier bezier;

    REQUIRE(path.bezierAtIndex(0, bezier));

    requirePoint(
        bezier.startPos(),
        Vec2d{0.0, 0.0}
    );

    requirePoint(
        bezier.controlPos1(),
        Vec2d{5.0, 10.0}
    );

    requirePoint(
        bezier.controlPos2(),
        Vec2d{15.0, 10.0}
    );

    requirePoint(
        bezier.endPos(),
        Vec2d{20.0, 20.0}
    );
}

TEST_CASE(
    "GraphicPath bezierAtIndex invalid indices",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 10.0);

    Bezier bezier;

    REQUIRE_FALSE(path.bezierAtIndex(-1, bezier));
    REQUIRE(path.bezierAtIndex(0, bezier));
    REQUIRE_FALSE(path.bezierAtIndex(1, bezier));
    REQUIRE_FALSE(path.bezierAtIndex(100, bezier));
}

TEST_CASE(
    "GraphicPath closed path provides closing Bezier",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);
    path.addPoint(10.0, 10.0);

    path.setClosed(true);

    Bezier bezier;

    REQUIRE(path.bezierAtIndex(2, bezier));

    requirePoint(
        bezier.startPos(),
        Vec2d{10.0, 10.0}
    );

    requirePoint(
        bezier.endPos(),
        Vec2d{0.0, 0.0}
    );
}

//------------------------------------------------------------------------------
// addBezier
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath addBezier",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);

    path.addBezier(
        Vec2d{5.0, 0.0},
        Vec2d{15.0, 10.0},
        Vec2d{20.0, 10.0}
    );

    REQUIRE(path.pointCount() == 2);

    const auto* start = path.pointPtrAtIndex(0);
    const auto* end = path.pointPtrAtIndex(1);

    REQUIRE(start != nullptr);
    REQUIRE(end != nullptr);

    REQUIRE(start->rightFlag);
    REQUIRE(end->leftFlag);

    requirePoint(
        start->right,
        Vec2d{5.0, 0.0}
    );

    requirePoint(
        end->left,
        Vec2d{15.0, 10.0}
    );

    requirePoint(
        end->anchor,
        Vec2d{20.0, 10.0}
    );
}

TEST_CASE(
    "GraphicPath addBezier on empty path does nothing",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addBezier(
        Vec2d{5.0, 0.0},
        Vec2d{15.0, 10.0},
        Vec2d{20.0, 10.0}
    );

    REQUIRE(path.pointCount() == 0);
}

//------------------------------------------------------------------------------
// Quadratic Bézier
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath addQuadraticBezier",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);

    path.addQuadraticBezier(
        Vec2d{10.0, 20.0},
        Vec2d{20.0, 0.0}
    );

    REQUIRE(path.pointCount() == 2);

    Bezier bezier;

    REQUIRE(path.bezierAtIndex(0, bezier));

    requirePoint(
        bezier.startPos(),
        Vec2d{0.0, 0.0}
    );

    requirePoint(
        bezier.endPos(),
        Vec2d{20.0, 0.0}
    );

    requirePoint(
        bezier.controlPos1(),
        Vec2d{
            20.0 / 3.0,
            40.0 / 3.0
        }
    );

    requirePoint(
        bezier.controlPos2(),
        Vec2d{
            40.0 / 3.0,
            40.0 / 3.0
        }
    );
}

TEST_CASE(
    "GraphicPath addQuadraticBezier on empty path does nothing",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addQuadraticBezier(
        Vec2d{10.0, 20.0},
        Vec2d{20.0, 0.0}
    );

    REQUIRE(path.pointCount() == 0);
}

//------------------------------------------------------------------------------
// Smooth Bézier
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath addSmoothBezier reflects previous left handle",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        0.0, 0.0,
        false,
        0.0, 0.0,
        true,
        10.0, 0.0
    );

    path.addPoint(20.0, 0.0);

    path.setLastLeft(Vec2d{15.0, 5.0});

    path.addSmoothBezier(
        Vec2d{25.0, 10.0},
        Vec2d{30.0, 0.0}
    );

    REQUIRE(path.pointCount() == 3);

    Bezier bezier;

    REQUIRE(path.bezierAtIndex(1, bezier));

    requirePoint(
        bezier.controlPos1(),
        Vec2d{25.0, -5.0}
    );

    requirePoint(
        bezier.controlPos2(),
        Vec2d{25.0, 10.0}
    );

    requirePoint(
        bezier.endPos(),
        Vec2d{30.0, 0.0}
    );
}

TEST_CASE(
    "GraphicPath addSmoothBezier without previous handle uses anchor",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);

    path.addSmoothBezier(
        Vec2d{15.0, 5.0},
        Vec2d{20.0, 0.0}
    );

    Bezier bezier;

    REQUIRE(path.bezierAtIndex(1, bezier));

    requirePoint(
        bezier.controlPos1(),
        Vec2d{10.0, 0.0}
    );
}

//------------------------------------------------------------------------------
// Smooth quadratic Bézier
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath addSmoothQuadraticBezier",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);

    path.addSmoothQuadraticBezier(
        Vec2d{20.0, 0.0}
    );

    REQUIRE(path.pointCount() == 2);

    Bezier bezier;

    REQUIRE(path.bezierAtIndex(0, bezier));

    requirePoint(
        bezier.startPos(),
        Vec2d{0.0, 0.0}
    );

    requirePoint(
        bezier.endPos(),
        Vec2d{20.0, 0.0}
    );

    requirePoint(
        bezier.controlPos1(),
        Vec2d{10.0, 0.0}
    );

    requirePoint(
        bezier.controlPos2(),
        Vec2d{10.0, 0.0}
    );
}

//------------------------------------------------------------------------------
// Control points
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath setLastLeft",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(10.0, 20.0);

    path.setLastLeft(
        Vec2d{5.0, 15.0}
    );

    const auto* point = path.lastPointPtr();

    REQUIRE(point != nullptr);
    REQUIRE(point->leftFlag);

    requirePoint(
        point->left,
        Vec2d{5.0, 15.0}
    );
}

TEST_CASE(
    "GraphicPath setLastRight",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(10.0, 20.0);

    path.setLastRight(
        Vec2d{15.0, 25.0}
    );

    const auto* point = path.lastPointPtr();

    REQUIRE(point != nullptr);
    REQUIRE(point->rightFlag);

    requirePoint(
        point->right,
        Vec2d{15.0, 25.0}
    );
}

TEST_CASE(
    "GraphicPath setLastControlPoint on empty path does nothing",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.setLastLeft(Vec2d{1.0, 2.0});
    path.setLastRight(Vec2d{3.0, 4.0});

    REQUIRE(path.pointCount() == 0);
}

//------------------------------------------------------------------------------
// Geometry
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath simplePolygonCentroid",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);
    path.addPoint(10.0, 10.0);
    path.addPoint(0.0, 10.0);

    requirePoint(
        path.simplePolygonCentroid(),
        Vec2d{5.0, 5.0}
    );
}

TEST_CASE(
    "GraphicPath simplePolygonCentroid empty",
    "[GraphicPath]"
)
{
    const GraphicPath path;

    requirePoint(
        path.simplePolygonCentroid(),
        Vec2d{0.0, 0.0}
    );
}

TEST_CASE(
    "GraphicPath polygonCentroid",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);
    path.addPoint(10.0, 10.0);
    path.addPoint(0.0, 10.0);

    Vec2d centroid;

    const double area =
        path.polygonCentroid(centroid);

    REQUIRE_THAT(
        area,
        Catch::Matchers::WithinAbs(100.0, epsilon)
    );

    requirePoint(
        centroid,
        Vec2d{5.0, 5.0}
    );
}

TEST_CASE(
    "GraphicPath polygonCentroid triangle",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);
    path.addPoint(0.0, 10.0);

    Vec2d centroid;

    const double area =
        path.polygonCentroid(centroid);

    REQUIRE_THAT(
        area,
        Catch::Matchers::WithinAbs(50.0, epsilon)
    );

    requirePoint(
        centroid,
        Vec2d{
            10.0 / 3.0,
            10.0 / 3.0
        }
    );
}

TEST_CASE(
    "GraphicPath polygonCentroid insufficient points",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);

    Vec2d centroid{100.0, 100.0};

    const double area =
        path.polygonCentroid(centroid);

    REQUIRE_THAT(
        area,
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );

    requirePoint(
        centroid,
        Vec2d{0.0, 0.0}
    );
}

TEST_CASE(
    "GraphicPath polygonCentroid degenerate polygon",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);
    path.addPoint(20.0, 0.0);

    Vec2d centroid{100.0, 100.0};

    const double area =
        path.polygonCentroid(centroid);

    REQUIRE_THAT(
        area,
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );

    requirePoint(
        centroid,
        Vec2d{0.0, 0.0}
    );
}

TEST_CASE(
    "GraphicPath bounds for line",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(10.0, 20.0);
    path.addPoint(30.0, 50.0);

    requireRect(
        path.bounds(),
        Rectd{
            10.0,
            20.0,
            20.0,
            30.0
        }
    );
}

TEST_CASE(
    "GraphicPath bounds includes Bezier extrema",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        0.0, 0.0,
        false,
        0.0, 0.0,
        true,
        0.0, 100.0
    );

    path.addPoint(
        100.0, 0.0,
        true,
        100.0, 100.0,
        false,
        100.0, 0.0
    );

    const Rectd bounds = path.bounds();

    REQUIRE(bounds.x <= 0.0);
    REQUIRE(bounds.y <= 0.0);
    REQUIRE(bounds.x + bounds.width >= 100.0);
    REQUIRE(bounds.y + bounds.height > 0.0);
}

TEST_CASE(
    "GraphicPath bounds empty",
    "[GraphicPath]"
)
{
    GraphicPath path;

    Rectd bounds{
        100.0,
        200.0,
        300.0,
        400.0
    };

    REQUIRE_FALSE(path.bounds(bounds));

    requireRect(
        bounds,
        Rectd{}
    );

    requireRect(
        path.bounds(),
        Rectd{}
    );
}

//------------------------------------------------------------------------------
// Length
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath length of straight line",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(30.0, 40.0);

    REQUIRE_THAT(
        path.length(),
        Catch::Matchers::WithinAbs(50.0, 1e-6)
    );
}

TEST_CASE(
    "GraphicPath length of multiple straight segments",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);
    path.addPoint(10.0, 20.0);

    REQUIRE_THAT(
        path.length(),
        Catch::Matchers::WithinAbs(30.0, 1e-6)
    );
}

TEST_CASE(
    "GraphicPath closed path length includes closing segment",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);
    path.addPoint(10.0, 10.0);
    path.addPoint(0.0, 10.0);

    path.setClosed(true);

    REQUIRE_THAT(
        path.length(),
        Catch::Matchers::WithinAbs(40.0, 1e-6)
    );
}

TEST_CASE(
    "GraphicPath Bezier length exceeds chord",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        0.0, 0.0,
        false,
        0.0, 0.0,
        true,
        0.0, 100.0
    );

    path.addPoint(
        100.0, 0.0,
        true,
        100.0, 100.0,
        false,
        100.0, 0.0
    );

    const double length = path.length();

    REQUIRE(length > 100.0);
}

//------------------------------------------------------------------------------
// Transform
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath translatePoint",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        10.0, 20.0,
        5.0, 15.0,
        30.0, 25.0
    );

    path.translatePoint(
        0,
        3.0,
        -7.0
    );

    const auto* point =
        path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{13.0, 13.0}
    );

    requirePoint(
        point->left,
        Vec2d{8.0, 8.0}
    );

    requirePoint(
        point->right,
        Vec2d{33.0, 18.0}
    );
}

TEST_CASE(
    "GraphicPath translatePoint invalid index does nothing",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(10.0, 20.0);

    path.translatePoint(
        -1,
        100.0,
        100.0
    );

    path.translatePoint(
        1,
        100.0,
        100.0
    );

    const auto* point =
        path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );
}

TEST_CASE(
    "GraphicPath rotatePoint",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        10.0, 10.0,
        20.0, 10.0,
        10.0, 20.0
    );

    path.rotatePoint(0, 90.0);

    const auto* point =
        path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 10.0}
    );

    requirePoint(
        point->left,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        point->right,
        Vec2d{0.0, 10.0}
    );
}

TEST_CASE(
    "GraphicPath rotatePoint invalid index does nothing",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(
        10.0, 20.0,
        5.0, 15.0,
        30.0, 25.0
    );

    path.rotatePoint(-1, 90.0);
    path.rotatePoint(1, 90.0);

    const auto* point =
        path.pointPtrAtIndex(0);

    REQUIRE(point != nullptr);

    requirePoint(
        point->anchor,
        Vec2d{10.0, 20.0}
    );

    requirePoint(
        point->left,
        Vec2d{5.0, 15.0}
    );

    requirePoint(
        point->right,
        Vec2d{30.0, 25.0}
    );
}

//------------------------------------------------------------------------------
// Projection
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath projectToQuadrilateral",
    "[GraphicPath]"
)
{
    const Quadrilateral quadrilateral{
        Vec2d{0.0, 0.0},
        Vec2d{100.0, 0.0},
        Vec2d{100.0, 100.0},
        Vec2d{0.0, 100.0}
    };

    GraphicPath path;

    path.addPoint(0.25, 0.25);
    path.addPoint(0.75, 0.25);

    path.projectToQuadrilateral(
        quadrilateral
    );

    requirePoint(
        path.pointPtrAtIndex(0)->anchor,
        Vec2d{25.0, 25.0}
    );

    requirePoint(
        path.pointPtrAtIndex(1)->anchor,
        Vec2d{75.0, 25.0}
    );
}

TEST_CASE(
    "GraphicPath projectToQuadrilateral with matrix",
    "[GraphicPath]"
)
{
    const Quadrilateral quadrilateral{
        Vec2d{0.0, 0.0},
        Vec2d{100.0, 0.0},
        Vec2d{100.0, 100.0},
        Vec2d{0.0, 100.0}
    };

    const Mat3d matrix =
        Mat3d::translation(10.0, 20.0);

    GraphicPath path;

    path.addPoint(0.25, 0.25);

    path.projectToQuadrilateral(
        quadrilateral,
        &matrix
    );

    requirePoint(
        path.pointPtrAtIndex(0)->anchor,
        Vec2d{35.0, 45.0}
    );
}

//------------------------------------------------------------------------------
// Split
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath split single line",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(100.0, 0.0);

    GraphicPathSplitParam split;

    path.split(
        0.25,
        0.75,
        split
    );

    REQUIRE(split.valid);
    REQUIRE_THAT(
        split.start,
        Catch::Matchers::WithinAbs(0.25, epsilon)
    );
    REQUIRE_THAT(
        split.end,
        Catch::Matchers::WithinAbs(0.75, epsilon)
    );

    REQUIRE(split.startIndex == 0);
    REQUIRE(split.endIndex == 0);

    REQUIRE_THAT(
        split.t0,
        Catch::Matchers::WithinAbs(0.25, epsilon)
    );

    REQUIRE_THAT(
        split.t1,
        Catch::Matchers::WithinAbs(0.75, epsilon)
    );
}

TEST_CASE(
    "GraphicPath split across two segments",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(100.0, 0.0);
    path.addPoint(100.0, 100.0);

    GraphicPathSplitParam split;

    path.split(
        0.25,
        0.75,
        split
    );

    REQUIRE(split.valid);

    REQUIRE(split.startIndex == 0);
    REQUIRE(split.endIndex == 1);

    REQUIRE_THAT(
        split.t0,
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );

    REQUIRE_THAT(
        split.t1,
        Catch::Matchers::WithinAbs(0.5, epsilon)
    );
}

TEST_CASE(
    "GraphicPath split clamps parameters",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(100.0, 0.0);

    GraphicPathSplitParam split;

    path.split(
        -1.0,
        2.0,
        split
    );

    REQUIRE(split.valid);

    REQUIRE_THAT(
        split.start,
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );

    REQUIRE_THAT(
        split.end,
        Catch::Matchers::WithinAbs(1.0, epsilon)
    );

    REQUIRE_THAT(
        split.t0,
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );

    REQUIRE_THAT(
        split.t1,
        Catch::Matchers::WithinAbs(1.0, epsilon)
    );
}

TEST_CASE(
    "GraphicPath split equal parameters is invalid",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(100.0, 0.0);

    GraphicPathSplitParam split;

    path.split(
        0.5,
        0.5,
        split
    );

    REQUIRE_FALSE(split.valid);
}

TEST_CASE(
    "GraphicPath split empty path is invalid",
    "[GraphicPath]"
)
{
    GraphicPath path;

    GraphicPathSplitParam split;

    path.split(
        0.25,
        0.75,
        split
    );

    REQUIRE_FALSE(split.valid);
}

TEST_CASE(
    "GraphicPath split single point path is invalid",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(10.0, 20.0);

    GraphicPathSplitParam split;

    path.split(
        0.25,
        0.75,
        split
    );

    REQUIRE_FALSE(split.valid);
}

//------------------------------------------------------------------------------
// Clear / resolution
//------------------------------------------------------------------------------

TEST_CASE(
    "GraphicPath clear",
    "[GraphicPath]"
)
{
    GraphicPath path;

    path.addPoint(0.0, 0.0);
    path.addPoint(10.0, 0.0);

    REQUIRE(path.hasPoints());

    path.clear();

    REQUIRE_FALSE(path.hasPoints());
    REQUIRE(path.pointCount() == 0);
    REQUIRE(path.segmentCount() == 0);
    REQUIRE_THAT(
        path.length(),
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );
}

TEST_CASE(
    "GraphicPath setBezierSegmentResolution",
    "[GraphicPath]"
)
{
    GraphicPath path;

    REQUIRE(path.bezierSegmentResolution() == 20);

    path.setBezierSegmentResolution(50);

    REQUIRE(path.bezierSegmentResolution() == 50);
}