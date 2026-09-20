#include <grain/Geometry/RemapRect.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace Grain::Test {

TEST_CASE("RemapRect default construction", "[RemapRect]")
{
    const RemapRectd remap;

    REQUIRE(remap.source().x == 0.0);
    REQUIRE(remap.source().y == 0.0);
    REQUIRE(remap.source().width == 0.0);
    REQUIRE(remap.source().height == 0.0);

    REQUIRE(remap.destination().x == 0.0);
    REQUIRE(remap.destination().y == 0.0);
    REQUIRE(remap.destination().width == 0.0);
    REQUIRE(remap.destination().height == 0.0);

    REQUIRE(remap.scaleX() == 1.0);
    REQUIRE(remap.scaleY() == 1.0);
}


TEST_CASE("RemapRect identity mapping", "[RemapRect]")
{
    const Rect<double> source{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const Rect<double> destination{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const RemapRectd remap{source, destination};

    REQUIRE(remap.scaleX() == 1.0);
    REQUIRE(remap.scaleY() == 1.0);

    REQUIRE(remap.mapX(10.0) == 10.0);
    REQUIRE(remap.mapX(60.0) == 60.0);
    REQUIRE(remap.mapX(110.0) == 110.0);

    REQUIRE(remap.mapY(20.0) == 20.0);
    REQUIRE(remap.mapY(120.0) == 120.0);
    REQUIRE(remap.mapY(220.0) == 220.0);
}


TEST_CASE("RemapRect scales and translates coordinates", "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        100.0,
        100.0
    };

    const Rect<double> destination{
        200.0,
        300.0,
        400.0,
        200.0
    };

    const RemapRectd remap{source, destination};

    REQUIRE(remap.scaleX() == 4.0);
    REQUIRE(remap.scaleY() == 2.0);

    REQUIRE(remap.mapX(0.0) == 200.0);
    REQUIRE(remap.mapX(50.0) == 400.0);
    REQUIRE(remap.mapX(100.0) == 600.0);

    REQUIRE(remap.mapY(0.0) == 300.0);
    REQUIRE(remap.mapY(50.0) == 400.0);
    REQUIRE(remap.mapY(100.0) == 500.0);
}


TEST_CASE("RemapRect maps source rectangle to destination rectangle", "[RemapRect]")
{
    const Rect<double> source{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const Rect<double> destination{
        100.0,
        300.0,
        400.0,
        600.0
    };

    const RemapRectd remap{source, destination};

    const auto result = remap.map(source);

    REQUIRE_THAT(result.x, WithinAbs(destination.x, 1e-12));
    REQUIRE_THAT(result.y, WithinAbs(destination.y, 1e-12));
    REQUIRE_THAT(result.width, WithinAbs(destination.width, 1e-12));
    REQUIRE_THAT(result.height, WithinAbs(destination.height, 1e-12));
}


TEST_CASE("RemapRect maps points", "[RemapRect]")
{
    const Rect<double> source{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const Rect<double> destination{
        100.0,
        300.0,
        400.0,
        600.0
    };

    const RemapRectd remap{source, destination};

    const Vec2<double> sourcePoint{
        60.0,
        120.0
    };

    const auto result = remap.map(sourcePoint);

    REQUIRE_THAT(result.x, WithinAbs(300.0, 1e-12));
    REQUIRE_THAT(result.y, WithinAbs(600.0, 1e-12));
}


TEST_CASE("RemapRect maps points in place", "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        100.0,
        100.0
    };

    const Rect<double> destination{
        100.0,
        200.0,
        300.0,
        400.0
    };

    const RemapRectd remap{source, destination};

    Vec2<double> point{
        50.0,
        25.0
    };

    remap.map(point);

    REQUIRE_THAT(point.x, WithinAbs(250.0, 1e-12));
    REQUIRE_THAT(point.y, WithinAbs(300.0, 1e-12));
}


TEST_CASE("RemapRect mapInPlace matches map", "[RemapRect]")
{
    const Rect<double> source{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const Rect<double> destination{
        100.0,
        300.0,
        400.0,
        600.0
    };

    const RemapRectd remap{source, destination};

    const Vec2<double> original{
        40.0,
        80.0
    };

    auto expected = remap.map(original);

    auto actual = original;
    remap.mapInPlace(actual);

    REQUIRE_THAT(actual.x, WithinAbs(expected.x, 1e-12));
    REQUIRE_THAT(actual.y, WithinAbs(expected.y, 1e-12));
}


TEST_CASE("RemapRect output parameter matches map", "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        100.0,
        100.0
    };

    const Rect<double> destination{
        50.0,
        75.0,
        200.0,
        300.0
    };

    const RemapRectd remap{source, destination};

    const Vec2<double> point{
        25.0,
        50.0
    };

    const auto expected = remap.map(point);

    Vec2<double> result{};
    remap.map(point, result);

    REQUIRE_THAT(result.x, WithinAbs(expected.x, 1e-12));
    REQUIRE_THAT(result.y, WithinAbs(expected.y, 1e-12));
}


TEST_CASE("RemapRect inverseMap reverses map", "[RemapRect]")
{
    const Rect<double> source{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const Rect<double> destination{
        100.0,
        300.0,
        400.0,
        600.0
    };

    const RemapRectd remap{source, destination};

    const Vec2<double> original{
        50.0,
        100.0
    };

    const auto mapped = remap.map(original);
    const auto result = remap.inverseMap(mapped);

    REQUIRE_THAT(result.x, WithinAbs(original.x, 1e-12));
    REQUIRE_THAT(result.y, WithinAbs(original.y, 1e-12));
}


TEST_CASE("RemapRect inverseMapX and inverseMapY reverse mapping",
          "[RemapRect]")
{
    const Rect<double> source{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const Rect<double> destination{
        100.0,
        300.0,
        400.0,
        600.0
    };

    const RemapRectd remap{source, destination};

    const double sourceX = 55.0;
    const double sourceY = 125.0;

    const double mappedX = remap.mapX(sourceX);
    const double mappedY = remap.mapY(sourceY);

    REQUIRE_THAT(
        remap.inverseMapX(mappedX),
        WithinAbs(sourceX, 1e-12)
    );

    REQUIRE_THAT(
        remap.inverseMapY(mappedY),
        WithinAbs(sourceY, 1e-12)
    );
}


TEST_CASE("RemapRect inverseMap in place", "[RemapRect]")
{
    const Rect<double> source{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const Rect<double> destination{
        100.0,
        300.0,
        400.0,
        600.0
    };

    const RemapRectd remap{source, destination};

    const Vec2<double> original{
        50.0,
        100.0
    };

    auto point = remap.map(original);

    remap.inverseMap(point);

    REQUIRE_THAT(point.x, WithinAbs(original.x, 1e-12));
    REQUIRE_THAT(point.y, WithinAbs(original.y, 1e-12));
}


TEST_CASE("RemapRect inverse mapping of rectangles", "[RemapRect]")
{
    const Rect<double> source{
        10.0,
        20.0,
        100.0,
        200.0
    };

    const Rect<double> destination{
        100.0,
        300.0,
        400.0,
        600.0
    };

    const RemapRectd remap{source, destination};

    const Rect<double> original{
        25.0,
        50.0,
        50.0,
        100.0
    };

    const auto mapped = remap.map(original);
    const auto result = remap.inverseMap(mapped);

    REQUIRE_THAT(result.x, WithinAbs(original.x, 1e-12));
    REQUIRE_THAT(result.y, WithinAbs(original.y, 1e-12));
    REQUIRE_THAT(result.width, WithinAbs(original.width, 1e-12));
    REQUIRE_THAT(result.height, WithinAbs(original.height, 1e-12));
}


TEST_CASE("RemapRect maps arbitrary rectangle", "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        100.0,
        100.0
    };

    const Rect<double> destination{
        10.0,
        20.0,
        200.0,
        300.0
    };

    const RemapRectd remap{source, destination};

    const Rect<double> rect{
        25.0,
        10.0,
        50.0,
        20.0
    };

    const auto result = remap.map(rect);

    REQUIRE_THAT(result.x, WithinAbs(60.0, 1e-12));
    REQUIRE_THAT(result.y, WithinAbs(50.0, 1e-12));
    REQUIRE_THAT(result.width, WithinAbs(100.0, 1e-12));
    REQUIRE_THAT(result.height, WithinAbs(60.0, 1e-12));
}


TEST_CASE("RemapRect Y flip maps source top to destination bottom",
          "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        100.0,
        100.0
    };

    const Rect<double> destination{
        200.0,
        300.0,
        400.0,
        200.0
    };

    const RemapRectd remap{
        source,
        destination,
        true
    };

    REQUIRE(remap.scaleX() == 4.0);
    REQUIRE(remap.scaleY() == -2.0);

    // Source top -> destination bottom.
    REQUIRE_THAT(
        remap.mapY(0.0),
        WithinAbs(500.0, 1e-12)
    );

    // Source center -> destination center.
    REQUIRE_THAT(
        remap.mapY(50.0),
        WithinAbs(400.0, 1e-12)
    );

    // Source bottom -> destination top.
    REQUIRE_THAT(
        remap.mapY(100.0),
        WithinAbs(300.0, 1e-12)
    );
}


TEST_CASE("RemapRect Y flip maps source rectangle correctly",
          "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        100.0,
        100.0
    };

    const Rect<double> destination{
        200.0,
        300.0,
        400.0,
        200.0
    };

    const RemapRectd remap{
        source,
        destination,
        true
    };

    const auto result = remap.map(source);

    REQUIRE_THAT(result.x, WithinAbs(200.0, 1e-12));
    REQUIRE_THAT(result.y, WithinAbs(500.0, 1e-12));
    REQUIRE_THAT(result.width, WithinAbs(400.0, 1e-12));
    REQUIRE_THAT(result.height, WithinAbs(-200.0, 1e-12));
}


TEST_CASE("RemapRect stores flipped destination correctly",
          "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        100.0,
        100.0
    };

    const Rect<double> destination{
        200.0,
        300.0,
        400.0,
        200.0
    };

    const RemapRectd remap{
        source,
        destination,
        true
    };

    REQUIRE(remap.destination().x == 200.0);
    REQUIRE(remap.destination().y == 500.0);
    REQUIRE(remap.destination().width == 400.0);
    REQUIRE(remap.destination().height == 200.0);
}


TEST_CASE("RemapRect inverse does not preserve Y flip",
          "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        100.0,
        100.0
    };

    const Rect<double> destination{
        200.0,
        300.0,
        400.0,
        200.0
    };

    const RemapRectd remap{
        source,
        destination,
        true
    };

    const auto inverse = remap.inverse();

    REQUIRE(inverse.source().x == 200.0);
    REQUIRE(inverse.source().y == 500.0);
    REQUIRE(inverse.source().width == 400.0);
    REQUIRE(inverse.source().height == 200.0);

    REQUIRE(inverse.destination().x == 0.0);
    REQUIRE(inverse.destination().y == 0.0);
    REQUIRE(inverse.destination().width == 100.0);
    REQUIRE(inverse.destination().height == 100.0);
}


TEST_CASE("RemapRect set replaces existing mapping", "[RemapRect]")
{
    RemapRectd remap{
        Rect<double>{0.0, 0.0, 100.0, 100.0},
        Rect<double>{0.0, 0.0, 200.0, 200.0}
    };

    REQUIRE(remap.scaleX() == 2.0);
    REQUIRE(remap.scaleY() == 2.0);

    remap.set(
        Rect<double>{10.0, 20.0, 50.0, 100.0},
        Rect<double>{100.0, 200.0, 300.0, 400.0}
    );

    REQUIRE(remap.source().x == 10.0);
    REQUIRE(remap.source().y == 20.0);
    REQUIRE(remap.source().width == 50.0);
    REQUIRE(remap.source().height == 100.0);

    REQUIRE(remap.destination().x == 100.0);
    REQUIRE(remap.destination().y == 200.0);
    REQUIRE(remap.destination().width == 300.0);
    REQUIRE(remap.destination().height == 400.0);

    REQUIRE(remap.scaleX() == 6.0);
    REQUIRE(remap.scaleY() == 4.0);
}


TEST_CASE("RemapRect zero source width uses unit X scale",
          "[RemapRect]")
{
    const RemapRectd remap{
        Rect<double>{10.0, 20.0, 0.0, 100.0},
        Rect<double>{100.0, 200.0, 300.0, 400.0}
    };

    REQUIRE(remap.scaleX() == 1.0);

    REQUIRE(remap.mapX(10.0) == 100.0);
}


TEST_CASE("RemapRect zero source height uses unit Y scale",
          "[RemapRect]")
{
    const RemapRectd remap{
        Rect<double>{10.0, 20.0, 100.0, 0.0},
        Rect<double>{100.0, 200.0, 300.0, 400.0}
    };

    REQUIRE(remap.scaleY() == 1.0);

    REQUIRE(remap.mapY(20.0) == 200.0);
}


TEST_CASE("RemapRect zero source dimensions with Y flip",
          "[RemapRect]")
{
    const RemapRectd remap{
        Rect<double>{10.0, 20.0, 0.0, 0.0},
        Rect<double>{100.0, 200.0, 300.0, 400.0},
        true
    };

    REQUIRE(remap.scaleX() == 1.0);
    REQUIRE(remap.scaleY() == 1.0);

    REQUIRE(remap.destination().x == 100.0);
    REQUIRE(remap.destination().y == 600.0);
}


TEST_CASE("RemapRect float precision", "[RemapRect]")
{
    const Rect<float> source{
        0.0f,
        0.0f,
        100.0f,
        100.0f
    };

    const Rect<float> destination{
        10.0f,
        20.0f,
        200.0f,
        300.0f
    };

    const RemapRectf remap{source, destination};

    const Vec2<float> point{
        25.0f,
        50.0f
    };

    const auto result = remap.map(point);

    REQUIRE_THAT(result.x, WithinAbs(60.0f, 1e-5f));
    REQUIRE_THAT(result.y, WithinAbs(170.0f, 1e-5f));
}


TEST_CASE("RemapRect double precision", "[RemapRect]")
{
    const Rect<double> source{
        0.0,
        0.0,
        3.0,
        7.0
    };

    const Rect<double> destination{
        11.0,
        13.0,
        17.0,
        19.0
    };

    const RemapRectd remap{source, destination};

    REQUIRE_THAT(
        remap.mapX(1.5),
        WithinAbs(19.5, 1e-12)
    );

    REQUIRE_THAT(
        remap.mapY(3.5),
        WithinAbs(22.5, 1e-12)
    );
}


TEST_CASE("RemapRect map and inverseMap are consistent for multiple points",
          "[RemapRect]")
{
    const Rect<double> source{
        -100.0,
        50.0,
        250.0,
        400.0
    };

    const Rect<double> destination{
        800.0,
        -200.0,
        1250.0,
        900.0
    };

    const RemapRectd remap{source, destination};

    const Vec2<double> points[] = {
        {-100.0, 50.0},
        {-50.0, 100.0},
        {0.0, 250.0},
        {75.0, 400.0},
        {150.0, 450.0}
    };

    for (const auto& point : points) {
        const auto mapped = remap.map(point);
        const auto result = remap.inverseMap(mapped);

        REQUIRE_THAT(
            result.x,
            WithinAbs(point.x, 1e-10)
        );

        REQUIRE_THAT(
            result.y,
            WithinAbs(point.y, 1e-10)
        );
    }
}

} // namespace Grain::Test
