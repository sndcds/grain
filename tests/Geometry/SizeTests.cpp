#include <catch2/catch_test_macros.hpp>

#include <grain/Geometry/Size.hpp>

#include <sstream>


using namespace Grain;


TEST_CASE("Size default construction", "[Size]")
{
    Sizei size;

    CHECK(size.width == 0);
    CHECK(size.height == 0);
}


TEST_CASE("Size construction with width and height", "[Size]")
{
    Sizei size{100, 200};

    CHECK(size.width == 100);
    CHECK(size.height == 200);
}


TEST_CASE("Size copy construction", "[Size]")
{
    Sizei original{100, 200};
    Sizei copy(original);

    CHECK(copy.width == 100);
    CHECK(copy.height == 200);
}


TEST_CASE("Size move construction", "[Size]")
{
    Sizei original{100, 200};
    Sizei moved(std::move(original));

    CHECK(moved.width == 100);
    CHECK(moved.height == 200);
}


TEST_CASE("Size assignment", "[Size]")
{
    Sizei a{100, 200};
    Sizei b;

    b = a;

    CHECK(b.width == 100);
    CHECK(b.height == 200);
}


TEST_CASE("Size cross type construction", "[Size]")
{
    Sizef source{100.5f, 200.5f};
    Sizei target(source);

    CHECK(target.width == 100);
    CHECK(target.height == 200);
}


TEST_CASE("Size cross type assignment", "[Size]")
{
    Sizef source{100.5f, 200.5f};
    Sizei target;

    target = source;

    CHECK(target.width == 100);
    CHECK(target.height == 200);
}


TEST_CASE("Size assignment from Rect", "[Size]")
{
    Recti rect{
        10,
        20,
        300,
        400
    };

    Sizei size;
    size = rect;

    CHECK(size.width == 300);
    CHECK(size.height == 400);
}


TEST_CASE("Size centerX and centerY", "[Size]")
{
    Sizei size{100, 200};

    CHECK(size.centerX() == 50);
    CHECK(size.centerY() == 100);
}


TEST_CASE("Size center", "[Size]")
{
    Sizei size{100, 200};

    const auto center = size.center();

    CHECK(center.x == 50);
    CHECK(center.y == 100);
}


TEST_CASE("Size rounded dimensions", "[Size]")
{
    Sizef size{100.4f, 200.6f};

    CHECK(size.roundedWidth() == 100);
    CHECK(size.roundedHeight() == 201);
}


TEST_CASE("Size area", "[Size]")
{
    Sizei size{100, 200};

    CHECK(size.area() == 20000);
}


TEST_CASE("Size aspect ratio", "[Size]")
{
    Sizei size{200, 100};

    CHECK(size.aspectRatio() == 0.5);
}


TEST_CASE("Size landscape", "[Size]")
{
    Sizei size{200, 100};

    CHECK(size.isLandscape());
    CHECK_FALSE(size.isPortrait());
    CHECK_FALSE(size.isSquare());
}


TEST_CASE("Size portrait", "[Size]")
{
    Sizei size{100, 200};

    CHECK_FALSE(size.isLandscape());
    CHECK(size.isPortrait());
    CHECK_FALSE(size.isSquare());
}


TEST_CASE("Size square", "[Size]")
{
    Sizei size{200, 200};

    CHECK_FALSE(size.isLandscape());
    CHECK_FALSE(size.isPortrait());
    CHECK(size.isSquare());
}


TEST_CASE("Size set scalar", "[Size]")
{
    Sizei size{100, 200};

    size.set(300);

    CHECK(size.width == 300);
    CHECK(size.height == 300);
}


TEST_CASE("Size set width and height", "[Size]")
{
    Sizei size;

    size.set(300, 400);

    CHECK(size.width == 300);
    CHECK(size.height == 400);
}


TEST_CASE("Size zero", "[Size]")
{
    Sizei size{300, 400};

    size.zero();

    CHECK(size.width == 0);
    CHECK(size.height == 0);
}


TEST_CASE("Size flip", "[Size]")
{
    Sizei size{100, 200};

    size.flip();

    CHECK(size.width == 200);
    CHECK(size.height == 100);
}


TEST_CASE("Size scale", "[Size]")
{
    Sizei size{100, 200};

    size.scale(2);

    CHECK(size.width == 200);
    CHECK(size.height == 400);
}


TEST_CASE("Size equality", "[Size]")
{
    Sizei a{100, 200};
    Sizei b{100, 200};
    Sizei c{200, 100};

    CHECK(a == b);
    CHECK_FALSE(a != b);

    CHECK(a != c);
    CHECK_FALSE(a == c);
}


TEST_CASE("Size stream output", "[Size]")
{
    Sizei size{100, 200};

    std::ostringstream stream;
    stream << size;

    CHECK(stream.str() == "100, 200");
}


TEST_CASE("Size pointer stream output", "[Size]")
{
    Sizei size{100, 200};

    std::ostringstream stream;
    stream << &size;

    CHECK(stream.str() == "100, 200");
}


TEST_CASE("Size null pointer stream output", "[Size]")
{
    const Sizei* size = nullptr;

    std::ostringstream stream;
    stream << size;

    CHECK(stream.str() == "Size nullptr");
}


TEST_CASE("Size floating point values", "[Size]")
{
    Sized size{100.5, 200.25};

    CHECK(size.width == 100.5);
    CHECK(size.height == 200.25);
    CHECK(size.area() == 20125.125);
}


TEST_CASE("Size floating point center", "[Size]")
{
    Sizef size{101.0f, 201.0f};

    CHECK(size.centerX() == 50.5f);
    CHECK(size.centerY() == 100.5f);

    const auto center = size.center();

    CHECK(center.x == 50.5f);
    CHECK(center.y == 100.5f);
}


TEST_CASE("Size aliases", "[Size]")
{
    Sizei integer_size{100, 200};
    Sizel long_size{100, 200};
    Sizef float_size{100.0f, 200.0f};
    Sized double_size{100.0, 200.0};

    CHECK(integer_size.width == 100);
    CHECK(long_size.width == 100);
    CHECK(float_size.width == 100.0f);
    CHECK(double_size.width == 100.0);
}