
#include <catch2/catch_test_macros.hpp>

#include <grain/Type/FixValue.hpp>

#include <cstdint>
#include <limits>
#include <string>

using Grain::FixValue;

namespace {

constexpr std::int64_t kScale = 1'000'000'000LL;

bool approximatelyEqual(
    double actual,
    double expected,
    double tolerance = 1e-9
) {
    return actual >= expected - tolerance &&
           actual <= expected + tolerance;
}

} // namespace

TEST_CASE("FixValue default construction", "[FixValue][construction]") {
    const FixValue value;

    CHECK(value.isZero());
    CHECK(value.raw() == 0);
    CHECK(value.asDouble() == 0.0);
    CHECK_FALSE(static_cast<bool>(value));
}

TEST_CASE("FixValue integer construction", "[FixValue][construction]") {
    SECTION("positive integer") {
        const FixValue value(42);

        CHECK(value.raw() == 42 * kScale);
        CHECK(value.asDouble() == 42.0);
    }

    SECTION("negative integer") {
        const FixValue value(-42);

        CHECK(value.raw() == -42 * kScale);
        CHECK(value.asDouble() == -42.0);
    }

    SECTION("zero") {
        const FixValue value(0);

        CHECK(value.isZero());
        CHECK_FALSE(static_cast<bool>(value));
    }
}

TEST_CASE("FixValue floating-point construction", "[FixValue][construction]") {
    SECTION("positive fraction") {
        const FixValue value(3.14159);

        CHECK(approximatelyEqual(value.asDouble(), 3.14159));
    }

    SECTION("negative fraction") {
        const FixValue value(-12.75);

        CHECK(approximatelyEqual(value.asDouble(), -12.75));
    }

    SECTION("small fraction") {
        const FixValue value(0.000000001);

        CHECK(value.raw() == 1);
    }

    SECTION("fractional precision is bounded by the scale") {
        const FixValue value(1.123456789);

        CHECK(approximatelyEqual(value.asDouble(), 1.123456789));
    }
}

TEST_CASE("FixValue raw representation", "[FixValue][raw]") {
    SECTION("construct from raw value") {
        const FixValue value = FixValue::fromRaw(1'500'000'000LL);

        CHECK(value.raw() == 1'500'000'000LL);
        CHECK(value.asDouble() == 1.5);
    }

    SECTION("set raw value") {
        FixValue value;
        value.setRaw(-2'500'000'000LL);

        CHECK(value.raw() == -2'500'000'000LL);
        CHECK(value.asDouble() == -2.5);
    }
}

TEST_CASE("FixValue conversions", "[FixValue][conversion]") {
    SECTION("convert to double") {
        const FixValue value(12.5);

        CHECK(value.asDouble() == 12.5);
    }

    SECTION("convert to float") {
        const FixValue value(12.5);

        CHECK(value.asFloat() == 12.5F);
    }

    SECTION("boolean conversion") {
        CHECK(static_cast<bool>(FixValue(1)));
        CHECK(static_cast<bool>(FixValue(-1)));
        CHECK_FALSE(static_cast<bool>(FixValue(0)));
    }
}

TEST_CASE("FixValue arithmetic", "[FixValue][arithmetic]") {
    const FixValue a(10);
    const FixValue b(3);

    SECTION("addition") {
        CHECK((a + b).asDouble() == 13.0);
    }

    SECTION("subtraction") {
        CHECK((a - b).asDouble() == 7.0);
    }

    SECTION("multiplication") {
        CHECK((a * b).asDouble() == 30.0);
    }

    SECTION("division") {
        CHECK(approximatelyEqual((a / b).asDouble(), 10.0 / 3.0));
    }

    SECTION("fractional arithmetic") {
        const FixValue x(1.5);
        const FixValue y(2.25);

        CHECK((x + y).asDouble() == 3.75);
        CHECK((y - x).asDouble() == 0.75);
        CHECK((x * y).asDouble() == 3.375);
        CHECK((y / x).asDouble() == 1.5);
    }

    SECTION("negative arithmetic") {
        const FixValue x(-10);
        const FixValue y(3);

        CHECK((x + y).asDouble() == -7.0);
        CHECK((x - y).asDouble() == -13.0);
        CHECK((x * y).asDouble() == -30.0);
        CHECK(approximatelyEqual((x / y).asDouble(), -10.0 / 3.0));
    }
}

TEST_CASE("FixValue compound assignment", "[FixValue][arithmetic]") {
    FixValue value(10);

    value += FixValue(3);
    CHECK(value.asDouble() == 13.0);

    value -= FixValue(2);
    CHECK(value.asDouble() == 11.0);

    value *= FixValue(2);
    CHECK(value.asDouble() == 22.0);

    value /= FixValue(2);
    CHECK(value.asDouble() == 11.0);
}

TEST_CASE("FixValue unary operators", "[FixValue][arithmetic]") {
    SECTION("negation") {
        const FixValue value(12.5);

        CHECK((-value).asDouble() == -12.5);
        CHECK((-FixValue(-12.5)).asDouble() == 12.5);
    }

    SECTION("absolute value") {
        CHECK(FixValue(-12.5).abs().asDouble() == 12.5);
        CHECK(FixValue(12.5).abs().asDouble() == 12.5);
        CHECK(FixValue(0).abs().isZero());
    }
}

TEST_CASE("FixValue comparisons", "[FixValue][comparison]") {
    const FixValue a(10);
    const FixValue b(20);
    const FixValue c(10);

    CHECK(a < b);
    CHECK(b > a);
    CHECK(a <= c);
    CHECK(a >= c);
    CHECK(a == c);
    CHECK(a != b);
}

TEST_CASE("FixValue integer detection", "[FixValue][classification]") {
    CHECK(FixValue(10).isInt());
    CHECK(FixValue(-10).isInt());
    CHECK(FixValue(0).isInt());

    CHECK_FALSE(FixValue(10.5).isInt());
    CHECK_FALSE(FixValue(-0.25).isInt());
}

TEST_CASE("FixValue floor", "[FixValue][rounding]") {
    CHECK(FixValue(3.7).floor().asDouble() == 3.0);
    CHECK(FixValue(3.0).floor().asDouble() == 3.0);
    CHECK(FixValue(-3.7).floor().asDouble() == -4.0);
    CHECK(FixValue(-3.0).floor().asDouble() == -3.0);
}

TEST_CASE("FixValue ceil", "[FixValue][rounding]") {
    CHECK(FixValue(3.7).ceil().asDouble() == 4.0);
    CHECK(FixValue(3.0).ceil().asDouble() == 3.0);
    CHECK(FixValue(-3.7).ceil().asDouble() == -3.0);
    CHECK(FixValue(-3.0).ceil().asDouble() == -3.0);
}

TEST_CASE("FixValue round", "[FixValue][rounding]") {
    CHECK(FixValue(3.2).round().asDouble() == 3.0);
    CHECK(FixValue(3.5).round().asDouble() == 4.0);
    CHECK(FixValue(3.8).round().asDouble() == 4.0);

    CHECK(FixValue(-3.2).round().asDouble() == -3.0);
    CHECK(FixValue(-3.5).round().asDouble() == -4.0);
    CHECK(FixValue(-3.8).round().asDouble() == -4.0);
}

TEST_CASE("FixValue square root", "[FixValue][math]") {
    CHECK(FixValue(0).sqrt().isZero());
    CHECK(FixValue(4).sqrt().asDouble() == 2.0);
    CHECK(FixValue(9).sqrt().asDouble() == 3.0);
    CHECK(approximatelyEqual(FixValue(2).sqrt().asDouble(), 1.414213562));
}

TEST_CASE("FixValue clamp", "[FixValue][math]") {
    const FixValue minimum(0);
    const FixValue maximum(10);

    FixValue below(-5);
    below.clamp(minimum, maximum);
    CHECK(below.asDouble() == 0.0);

    FixValue within(5);
    within.clamp(minimum, maximum);
    CHECK(within.asDouble() == 5.0);

    FixValue above(15);
    above.clamp(minimum, maximum);
    CHECK(above.asDouble() == 10.0);
}

TEST_CASE("FixValue string parsing", "[FixValue][string]") {
    SECTION("valid values") {
        CHECK(FixValue::parse("123").value().asDouble() == 123.0);
        CHECK(FixValue::parse("-123").value().asDouble() == -123.0);
        CHECK(FixValue::parse("12.5").value().asDouble() == 12.5);
        CHECK(FixValue::parse("-0.75").value().asDouble() == -0.75);
        CHECK(FixValue::parse("0.000000001").value().raw() == 1);
    }

    SECTION("invalid values") {
        CHECK_FALSE(FixValue::parse(""));
        CHECK_FALSE(FixValue::parse("abc"));
        CHECK_FALSE(FixValue::parse("12.3.4"));
    }
}

TEST_CASE("FixValue string formatting", "[FixValue][string]") {
    CHECK(FixValue(0).toString() == "0");
    CHECK(FixValue(42).toString() == "42");
    CHECK(FixValue(12.5).toString() == "12.5");
    CHECK(FixValue(-12.5).toString() == "-12.5");
    CHECK(FixValue(1.125).toString() == "1.125");
}

TEST_CASE("FixValue precision", "[FixValue][precision]") {
    FixValue value(3.141592653);

    value.setPrecision(2);

    // Adjust this assertion if setPrecision() only affects formatting.
    CHECK(approximatelyEqual(value.asDouble(), 3.14));
}
