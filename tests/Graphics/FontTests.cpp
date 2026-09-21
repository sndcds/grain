#include <catch2/catch_test_macros.hpp>

#include <grain/Graphics/Font.hpp>

#include <cmath>
#include <utility>

namespace {

constexpr double kEpsilon = 1.0e-6;

void checkPositive(double value)
{
    CHECK(std::isfinite(value));
    CHECK(value >= 0.0);
}

} // namespace

TEST_CASE("Font construction", "[Font]")
{
    SECTION("construct with size")
    {
        Grain::Font font(24.0f);

        CHECK(font.isValid());
        CHECK(font.size() == 24.0f);
        CHECK(font.fontNameUtf8() != nullptr);
        CHECK(font.displayNameUtf8() != nullptr);
    }

    SECTION("construct with C string name")
    {
        Grain::Font font("Helvetica", 24.0f);

        CHECK(font.isValid());
        CHECK(font.size() == 24.0f);
        CHECK(font.fontNameUtf8() != nullptr);
        CHECK(font.displayNameUtf8() != nullptr);
    }

    SECTION("construct with Grain String name")
    {
        Grain::String name("Helvetica");
        Grain::Font font(name, 24.0f);

        CHECK(font.isValid());
        CHECK(font.size() == 24.0f);
        CHECK(font.fontNameUtf8() != nullptr);
        CHECK(font.displayNameUtf8() != nullptr);
    }

    SECTION("construct from null font")
    {
        Grain::Font font(static_cast<const Grain::Font*>(nullptr));

        CHECK(font.isValid());
        CHECK(font.size() > 0.0f);
        CHECK(font.fontNameUtf8() != nullptr);
        CHECK(font.displayNameUtf8() != nullptr);
    }

    SECTION("construct from null font with size")
    {
        Grain::Font font(
            static_cast<const Grain::Font*>(nullptr),
            24.0f);

        CHECK(font.isValid());
        CHECK(font.size() == 24.0f);
        CHECK(font.fontNameUtf8() != nullptr);
        CHECK(font.displayNameUtf8() != nullptr);
    }
}


TEST_CASE("Font move construction", "[Font]")
{
    Grain::Font source("Helvetica", 24.0f);

    REQUIRE(source.isValid());

    const float size = source.size();

    Grain::Font moved(std::move(source));

    CHECK(moved.isValid());
    CHECK(moved.size() == size);
    CHECK(moved.fontNameUtf8() != nullptr);
    CHECK(moved.displayNameUtf8() != nullptr);
}


TEST_CASE("Font move assignment", "[Font]")
{
    Grain::Font source("Helvetica", 24.0f);
    Grain::Font destination("Times New Roman", 12.0f);

    REQUIRE(source.isValid());
    REQUIRE(destination.isValid());

    const float size = source.size();

    destination = std::move(source);

    CHECK(destination.isValid());
    CHECK(destination.size() == size);
    CHECK(destination.fontNameUtf8() != nullptr);
    CHECK(destination.displayNameUtf8() != nullptr);
}


TEST_CASE("Font copy constructor is deleted", "[Font]")
{
    STATIC_REQUIRE_FALSE(
        std::is_copy_constructible_v<Grain::Font>);
}


TEST_CASE("Font copy assignment is deleted", "[Font]")
{
    STATIC_REQUIRE_FALSE(
        std::is_copy_assignable_v<Grain::Font>);
}


TEST_CASE("Font set", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    font.set(
        Grain::String("Times New Roman"),
        32.0f);

    CHECK(font.isValid());
    CHECK(font.size() == 32.0f);
    CHECK(font.fontNameUtf8() != nullptr);
    CHECK(font.displayNameUtf8() != nullptr);
}


TEST_CASE("Font metrics", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    SECTION("ascent")
    {
        checkPositive(font.ascent());
    }

    SECTION("descent")
    {
        checkPositive(font.descent());
    }

    SECTION("x height")
    {
        checkPositive(font.xHeight());
    }

    SECTION("cap height")
    {
        checkPositive(font.capHeight());
    }

    SECTION("leading")
    {
        CHECK(std::isfinite(font.leading()));
    }

    SECTION("cell height")
    {
        checkPositive(font.cellHeight());
    }

    SECTION("line height")
    {
        checkPositive(font.lineHeight());
    }

    SECTION("italic angle")
    {
        CHECK(std::isfinite(font.italicAngle()));
    }

    SECTION("underline position")
    {
        CHECK(std::isfinite(font.underlinePosition()));
    }

    SECTION("underline thickness")
    {
        checkPositive(font.underlineThickness());
    }

    SECTION("units per em")
    {
        CHECK(font.unitsPerEm() > 0);
    }

    SECTION("glyph count")
    {
        CHECK(font.glyphCount() > 0);
    }

    SECTION("bounding box")
    {
        const Grain::Rectd& bounds = font.boundingBox();

        CHECK(std::isfinite(bounds.x));
        CHECK(std::isfinite(bounds.y));
        CHECK(std::isfinite(bounds.width));
        CHECK(std::isfinite(bounds.height));

        CHECK(bounds.width >= 0.0);
        CHECK(bounds.height >= 0.0);
    }
}


TEST_CASE("Font metrics have consistent values", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    const double expected =
        font.ascent() +
        font.descent() +
        font.leading();

    CHECK(
        std::fabs(font.cellHeight() - expected)
        <= 1.0e-6);

    CHECK(font.lineHeight() > 0.0);
    CHECK(font.cellHeight() > 0.0);
}


TEST_CASE("Font glyph advance width", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    SECTION("single character")
    {
        const double width =
            font.glyphAdvanceWidth("A");

        CHECK(std::isfinite(width));
        CHECK(width > 0.0);
    }

    SECTION("character with explicit length")
    {
        const double width =
            font.glyphAdvanceWidth("A", 1);

        CHECK(std::isfinite(width));
        CHECK(width > 0.0);
    }

    SECTION("space")
    {
        const double width =
            font.glyphAdvanceWidth(" ");

        CHECK(std::isfinite(width));
        CHECK(width > 0.0);
    }

    SECTION("invalid null string")
    {
        const double width =
            font.glyphAdvanceWidth(nullptr);

        CHECK(std::isfinite(width));
        CHECK(width == 0.0);
    }
}


TEST_CASE("Font text dimension", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    SECTION("simple text")
    {
        const Grain::Sized size =
            font.textDimension("Hello");

        CHECK(std::isfinite(size.width));
        CHECK(std::isfinite(size.height));

        CHECK(size.width > 0.0);
        CHECK(size.height > 0.0);
    }

    SECTION("explicit byte length")
    {
        const char* text = "Hello";

        const Grain::Sized size =
            font.textDimension(text, 5);

        CHECK(std::isfinite(size.width));
        CHECK(std::isfinite(size.height));

        CHECK(size.width > 0.0);
        CHECK(size.height > 0.0);
    }

    SECTION("empty string")
    {
        const Grain::Sized size =
            font.textDimension("");

        CHECK(std::isfinite(size.width));
        CHECK(std::isfinite(size.height));

        CHECK(size.width >= 0.0);
        CHECK(size.height >= 0.0);
    }

    SECTION("null string")
    {
        const Grain::Sized size =
            font.textDimension(nullptr);

        CHECK(std::isfinite(size.width));
        CHECK(std::isfinite(size.height));

        CHECK(size.width == 0.0);
        CHECK(size.height == 0.0);
    }
}


TEST_CASE("Font character index at x", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    double delta = 0.0;
    double cursorX = 0.0;

    const int32_t index =
        font.charIndexAtX(
            "Hello",
            10.0,
            delta,
            cursorX);

    CHECK(index >= 0);
    CHECK(index <= 5);

    CHECK(std::isfinite(delta));
    CHECK(std::isfinite(cursorX));
}


TEST_CASE("Font character index at beginning", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    double delta = 0.0;
    double cursorX = 0.0;

    const int32_t index =
        font.charIndexAtX(
            "Hello",
            0.0,
            delta,
            cursorX);

    CHECK(index == 0);
    CHECK(std::isfinite(delta));
    CHECK(std::isfinite(cursorX));
}


TEST_CASE("Font character index at different positions", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    double previousCursorX = -1.0;

    for (double x = 0.0; x <= 200.0; x += 10.0) {
        double delta = 0.0;
        double cursorX = 0.0;

        const int32_t index =
            font.charIndexAtX(
                "Hello World",
                x,
                delta,
                cursorX);

        CHECK(index >= 0);
        CHECK(index <= 11);

        CHECK(std::isfinite(delta));
        CHECK(std::isfinite(cursorX));

        CHECK(cursorX >= previousCursorX);

        previousCursorX = cursorX;
    }
}


TEST_CASE("Font null character index input", "[Font]")
{
    Grain::Font font("Helvetica", 24.0f);

    REQUIRE(font.isValid());

    double delta = 123.0;
    double cursorX = 456.0;

    const int32_t index =
        font.charIndexAtX(
            nullptr,
            10.0,
            delta,
            cursorX);

    CHECK(index < 0);
}


TEST_CASE("Font copy constructor with size", "[Font]")
{
    Grain::Font source("Helvetica", 24.0f);

    REQUIRE(source.isValid());

    Grain::Font copy(
        &source,
        36.0f);

    CHECK(copy.isValid());
    CHECK(copy.size() == 36.0f);
    CHECK(copy.fontNameUtf8() != nullptr);
    CHECK(copy.displayNameUtf8() != nullptr);
}


TEST_CASE("Font copy constructor preserves font", "[Font]")
{
    Grain::Font source("Helvetica", 24.0f);

    REQUIRE(source.isValid());

    Grain::Font copy(&source);

    CHECK(copy.isValid());
    CHECK(copy.size() == source.size());
    CHECK(copy.fontNameUtf8() != nullptr);
    CHECK(copy.displayNameUtf8() != nullptr);
}


TEST_CASE("Font invalid name", "[Font]")
{
    Grain::Font font(
        "ThisFontDefinitelyDoesNotExist_12345",
        24.0f);

    CHECK_FALSE(font.isValid());
}


TEST_CASE("Font default font", "[Font]")
{
    Grain::Font font(24.0f);

    CHECK(font.isValid());
    CHECK(font.size() == 24.0f);

    CHECK(font.fontNameUtf8() != nullptr);
    CHECK(font.displayNameUtf8() != nullptr);

    CHECK(font.unitsPerEm() > 0);
    CHECK(font.glyphCount() > 0);
}


TEST_CASE("Font destructor", "[Font]")
{
    {
        Grain::Font font("Helvetica", 24.0f);

        REQUIRE(font.isValid());
    }

    // Reaching this point verifies that destruction does not crash.
    SUCCEED();
}