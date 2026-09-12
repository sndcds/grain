#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <grain/Math/Vec2.hpp>
#include <grain/Geometry/Rect.hpp>

#include <cstdint>
#include <sstream>

using namespace Grain;

TEST_CASE("Rect construction", "[Rect]")
{
    SECTION("default construction") {
        const Recti rect;

        REQUIRE(rect.x == 0);
        REQUIRE(rect.y == 0);
        REQUIRE(rect.width == 0);
        REQUIRE(rect.height == 0);
    }

    SECTION("full construction") {
        const Recti rect{10, 20, 100, 50};

        REQUIRE(rect.x == 10);
        REQUIRE(rect.y == 20);
        REQUIRE(rect.width == 100);
        REQUIRE(rect.height == 50);
    }

    SECTION("square construction") {
        const Recti rect{50};

        REQUIRE(rect.x == 0);
        REQUIRE(rect.y == 0);
        REQUIRE(rect.width == 50);
        REQUIRE(rect.height == 50);
    }

    SECTION("position and size") {
        const Vec2i position{10, 20};
        const Vec2i size{100, 50};

        const Recti rect{position, size};

        REQUIRE(rect.position() == position);
        REQUIRE(rect.size() == size);
    }

    SECTION("center and radius") {
        const Vec2d center{50.0, 40.0};
        const Rectd rect{center, 10.0};

        REQUIRE(rect.x == Catch::Approx(40.0));
        REQUIRE(rect.y == Catch::Approx(30.0));
        REQUIRE(rect.width == Catch::Approx(20.0));
        REQUIRE(rect.height == Catch::Approx(20.0));
        REQUIRE(rect.center() == center);
    }

    SECTION("center and dimensions") {
        const Vec2d center{50.0, 40.0};
        const Rectd rect{center, 100.0, 60.0};

        REQUIRE(rect.x == Catch::Approx(0.0));
        REQUIRE(rect.y == Catch::Approx(10.0));
        REQUIRE(rect.width == Catch::Approx(100.0));
        REQUIRE(rect.height == Catch::Approx(60.0));
    }

    SECTION("type conversion") {
        const Recti source{10, 20, 100, 50};
        const Rectd rect{source};

        REQUIRE(rect.x == Catch::Approx(10.0));
        REQUIRE(rect.y == Catch::Approx(20.0));
        REQUIRE(rect.width == Catch::Approx(100.0));
        REQUIRE(rect.height == Catch::Approx(50.0));
    }
}


TEST_CASE("Rect comparison", "[Rect]")
{
    const Recti a{10, 20, 100, 50};
    const Recti b{10, 20, 100, 50};
    const Recti c{10, 20, 101, 50};

    REQUIRE(a == b);
    REQUIRE_FALSE(a != b);

    REQUIRE(a != c);
    REQUIRE_FALSE(a == c);
}


TEST_CASE("Rect position and bounds", "[Rect]")
{
    const Recti rect{10, 20, 100, 50};

    const Vec2i position{10, 20};
    const Vec2i size{100, 50};

    const Vec2i top_left{10, 20};
    const Vec2i top_right{110, 20};
    const Vec2i bottom_right{110, 70};
    const Vec2i bottom_left{10, 70};

    REQUIRE(rect.position() == position);
    REQUIRE(rect.size() == size);

    REQUIRE(rect.right() == 110);
    REQUIRE(rect.bottom() == 70);

    REQUIRE(rect.centerX() == 60);
    REQUIRE(rect.centerY() == 45);

    const Vec2i expected_center{60, 45};
    REQUIRE(rect.center() == expected_center);

    REQUIRE(rect.topLeft() == top_left);
    REQUIRE(rect.topRight() == top_right);
    REQUIRE(rect.bottomRight() == bottom_right);
    REQUIRE(rect.bottomLeft() == bottom_left);
}


TEST_CASE("Rect geometry", "[Rect]")
{
    SECTION("sides") {
        const Recti rect{0, 0, 100, 50};

        REQUIRE(rect.shortSide() == 50);
        REQUIRE(rect.longSide() == 100);
    }

    SECTION("area") {
        const Recti rect{0, 0, 100, 50};

        REQUIRE(rect.area() == Catch::Approx(5000.0));
    }

    SECTION("aspect ratio") {
        const Rectd rect{0.0, 0.0, 100.0, 50.0};

        REQUIRE(rect.aspectRatio() == Catch::Approx(0.5));
    }

    SECTION("square") {
        const Rectd rect{0.0, 0.0, 100.0, 100.0};

        REQUIRE(rect.isSquare());
        REQUIRE(rect.absRatio() == Catch::Approx(1.0));
    }

    SECTION("horizontal") {
        const Recti rect{0, 0, 100, 50};

        REQUIRE(rect.isHorizontal());
        REQUIRE_FALSE(rect.isVertical());
    }

    SECTION("vertical") {
        const Recti rect{0, 0, 50, 100};

        REQUIRE(rect.isVertical());
        REQUIRE_FALSE(rect.isHorizontal());
    }

    SECTION("empty") {
        const Recti empty_width{0, 0, 0, 50};
        const Recti empty_height{0, 0, 100, 0};
        const Recti valid{0, 0, 100, 50};

        REQUIRE(empty_width.isEmpty());
        REQUIRE(empty_height.isEmpty());
        REQUIRE_FALSE(valid.isEmpty());

        REQUIRE_FALSE(empty_width.isValid());
        REQUIRE_FALSE(empty_height.isValid());
        REQUIRE(valid.isValid());
    }
}


TEST_CASE("Rect position modification", "[Rect]")
{
    Recti rect{10, 20, 100, 50};

    SECTION("set") {
        rect.set(1, 2, 3, 4);

        REQUIRE(rect.x == 1);
        REQUIRE(rect.y == 2);
        REQUIRE(rect.width == 3);
        REQUIRE(rect.height == 4);
    }

    SECTION("set position") {
        rect.setPosition(30, 40);

        REQUIRE(rect.x == 30);
        REQUIRE(rect.y == 40);
        REQUIRE(rect.width == 100);
        REQUIRE(rect.height == 50);
    }

    SECTION("set position from vector") {
        const Vec2i position{30, 40};

        rect.setPosition(position);

        REQUIRE(rect.position() == position);
    }

    SECTION("set size") {
        rect.setSize(200, 80);

        REQUIRE(rect.x == 10);
        REQUIRE(rect.y == 20);
        REQUIRE(rect.width == 200);
        REQUIRE(rect.height == 80);
    }

    SECTION("set size from vector") {
        const Vec2i size{200, 80};

        rect.setSize(size);

        REQUIRE(rect.size() == size);
    }

    SECTION("zero") {
        rect.zero();

        REQUIRE(rect.x == 0);
        REQUIRE(rect.y == 0);
        REQUIRE(rect.width == 0);
        REQUIRE(rect.height == 0);
    }
}


TEST_CASE("Rect translation", "[Rect]")
{
    Recti rect{10, 20, 100, 50};

    SECTION("translate by components") {
        rect.translate(5, -10);

        const Recti expected{15, 10, 100, 50};
        REQUIRE(rect == expected);
    }

    SECTION("translate by vector") {
        const Vec2i offset{5, -10};

        rect.translate(offset);

        const Recti expected{15, 10, 100, 50};
        REQUIRE(rect == expected);
    }

    SECTION("operator plus") {
        const Vec2i offset{5, -10};

        const Recti result = rect + offset;
        const Recti expected{15, 10, 100, 50};

        REQUIRE(result == expected);
        REQUIRE(rect.x == 10);
        REQUIRE(rect.y == 20);
    }

    SECTION("operator minus") {
        const Vec2i offset{5, 10};

        const Recti result = rect - offset;
        const Recti expected{5, 10, 100, 50};

        REQUIRE(result == expected);
    }

    SECTION("compound operators") {
        const Vec2i offset{5, -10};

        rect += offset;

        const Recti expected_after_add{15, 10, 100, 50};
        REQUIRE(rect == expected_after_add);

        rect -= offset;

        const Recti expected_after_subtract{10, 20, 100, 50};
        REQUIRE(rect == expected_after_subtract);
    }
}


TEST_CASE("Rect scaling", "[Rect]")
{
    SECTION("uniform scale") {
        Recti rect{10, 20, 100, 50};

        rect.scale(2);

        const Recti expected{20, 40, 200, 100};
        REQUIRE(rect == expected);
    }

    SECTION("non-uniform scale") {
        Recti rect{10, 20, 100, 50};

        rect.scale(2, 3);

        const Recti expected{20, 60, 200, 150};
        REQUIRE(rect == expected);
    }

    SECTION("scale size") {
        Recti rect{10, 20, 100, 50};

        rect.scaleSize(2);

        const Recti expected{10, 20, 200, 100};
        REQUIRE(rect == expected);
    }

    SECTION("scale size non-uniform") {
        Recti rect{10, 20, 100, 50};

        rect.scaleSize(2, 3);

        const Recti expected{10, 20, 200, 150};
        REQUIRE(rect == expected);
    }

    SECTION("vector scaling") {
        const Recti rect{10, 20, 100, 50};
        const Vec2i scale{2, 3};

        const Recti result = rect * scale;
        const Recti expected{20, 60, 100, 50};

        REQUIRE(result == expected);
    }
}


TEST_CASE("Rect inset and expansion", "[Rect]")
{
    SECTION("inset") {
        Recti rect{10, 20, 100, 50};

        rect.inset(5);

        const Recti expected{15, 25, 90, 40};
        REQUIRE(rect == expected);
    }

    SECTION("expand") {
        Recti rect{10, 20, 100, 50};

        rect.expand(5);

        const Recti expected{5, 15, 110, 60};
        REQUIRE(rect == expected);
    }

    SECTION("constructor with inset") {
        const Recti source{10, 20, 100, 50};
        const Recti rect{source, 5};

        const Recti expected{15, 25, 90, 40};
        REQUIRE(rect == expected);
    }
}


TEST_CASE("Rect normalization", "[Rect]")
{
    SECTION("positive size") {
        Recti rect{10, 20, 100, 50};

        rect.normalize();

        const Recti expected{10, 20, 100, 50};
        REQUIRE(rect == expected);
    }

    SECTION("negative width") {
        Recti rect{110, 20, -100, 50};

        rect.normalize();

        const Recti expected{10, 20, 100, 50};
        REQUIRE(rect == expected);
    }

    SECTION("negative height") {
        Recti rect{10, 70, 100, -50};

        rect.normalize();

        const Recti expected{10, 20, 100, 50};
        REQUIRE(rect == expected);
    }

    SECTION("negative width and height") {
        Recti rect{110, 70, -100, -50};

        rect.normalize();

        const Recti expected{10, 20, 100, 50};
        REQUIRE(rect == expected);
    }
}


TEST_CASE("Rect containment", "[Rect]")
{
    const Recti rect{10, 20, 100, 50};

    SECTION("point inside") {
        const Vec2i point{50, 40};

        REQUIRE(rect.contains(point));
    }

    SECTION("point outside") {
        const Vec2i point{200, 100};

        REQUIRE_FALSE(rect.contains(point));
    }

    SECTION("left edge") {
        const Vec2i point{10, 40};

        REQUIRE(rect.contains(point));
    }

    SECTION("right edge is exclusive") {
        const Vec2i point{110, 40};

        REQUIRE_FALSE(rect.contains(point));
    }

    SECTION("top edge") {
        const Vec2i point{50, 20};

        REQUIRE(rect.contains(point));
    }

    SECTION("bottom edge is exclusive") {
        const Vec2i point{50, 70};

        REQUIRE_FALSE(rect.contains(point));
    }

    SECTION("rectangle inside") {
        const Recti inner{20, 30, 20, 20};

        REQUIRE(rect.contains(inner));
    }

    SECTION("rectangle outside") {
        const Recti outer{0, 0, 200, 100};

        REQUIRE_FALSE(rect.contains(outer));
    }

    SECTION("contains x") {
        REQUIRE(rect.containsX(10));
        REQUIRE(rect.containsX(50));
        REQUIRE_FALSE(rect.containsX(110));
    }

    SECTION("contains y") {
        REQUIRE(rect.containsY(20));
        REQUIRE(rect.containsY(40));
        REQUIRE_FALSE(rect.containsY(70));
    }
}


TEST_CASE("Rect intersection", "[Rect]")
{
    const Recti rect{10, 20, 100, 50};

    SECTION("overlapping rectangles") {
        const Recti other{50, 40, 100, 50};

        REQUIRE(rect.intersects(other));

        const Recti result = rect.intersection(other);
        const Recti expected{50, 40, 60, 30};

        REQUIRE(result == expected);
    }

    SECTION("no intersection") {
        const Recti other{200, 200, 50, 50};

        REQUIRE_FALSE(rect.intersects(other));

        const Recti result = rect.intersection(other);
        const Recti expected{};

        REQUIRE(result == expected);
    }

    SECTION("touching edge") {
        const Recti other{110, 20, 50, 50};

        REQUIRE_FALSE(rect.intersects(other));

        const Recti result = rect.intersection(other);
        const Recti expected{};

        REQUIRE(result == expected);
    }

    SECTION("rectangle completely inside") {
        const Recti other{30, 30, 20, 20};

        REQUIRE(rect.intersects(other));

        const Recti result = rect.intersection(other);

        REQUIRE(result == other);
    }

    SECTION("in-place intersection") {
        const Recti other{50, 40, 100, 50};

        Recti result = rect;
        result.intersect(other);

        const Recti expected{50, 40, 60, 30};
        REQUIRE(result == expected);
    }
}


TEST_CASE("Rect union", "[Rect]")
{
    const Recti rect{10, 20, 100, 50};
    const Recti other{50, 40, 100, 50};

    SECTION("united") {
        const Recti result = rect.united(other);
        const Recti expected{10, 20, 140, 70};

        REQUIRE(result == expected);
    }

    SECTION("in-place union") {
        Recti result = rect;
        result.unite(other);

        const Recti expected{10, 20, 140, 70};
        REQUIRE(result == expected);
    }

    SECTION("operator plus rectangle") {
        Recti result = rect;
        result += other;

        const Recti expected{10, 20, 140, 70};
        REQUIRE(result == expected);
    }
}


TEST_CASE("Rect rounding", "[Rect]")
{
    const Rectd rect{
        10.25,
        20.75,
        100.49,
        50.51
    };

    REQUIRE(rect.roundedWidth() == 100.0);
    REQUIRE(rect.roundedHeight() == 51.0);
}


TEST_CASE("Rect stream output", "[Rect]")
{
    const Recti rect{10, 20, 100, 50};

    std::ostringstream stream;
    stream << rect;

    REQUIRE(stream.str() == "10, 20 .. 100 x 50");
}