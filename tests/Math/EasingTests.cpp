#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <grain/Math/Easing.hpp>

#include <cmath>
#include <numbers>
#include <string_view>
#include <vector>

namespace Grain::Easing::Tests {

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTolerance = 1e-12;
constexpr double kLooseTolerance = 1e-10;

struct EasingFunction {
    Type type;
    double (*function)(double) noexcept;
};

constexpr EasingFunction allFunctions[] = {
    {Type::InSine, easeInSine},
    {Type::OutSine, easeOutSine},
    {Type::InOutSine, easeInOutSine},

    {Type::InQuad, easeInQuad},
    {Type::OutQuad, easeOutQuad},
    {Type::InOutQuad, easeInOutQuad},

    {Type::InCubic, easeInCubic},
    {Type::OutCubic, easeOutCubic},
    {Type::InOutCubic, easeInOutCubic},

    {Type::InQuart, easeInQuart},
    {Type::OutQuart, easeOutQuart},
    {Type::InOutQuart, easeInOutQuart},

    {Type::InQuint, easeInQuint},
    {Type::OutQuint, easeOutQuint},
    {Type::InOutQuint, easeInOutQuint},

    {Type::InExpo, easeInExpo},
    {Type::OutExpo, easeOutExpo},
    {Type::InOutExpo, easeInOutExpo},

    {Type::InCirc, easeInCirc},
    {Type::OutCirc, easeOutCirc},
    {Type::InOutCirc, easeInOutCirc},

    {Type::InBack, easeInBack},
    {Type::OutBack, easeOutBack},
    {Type::InOutBack, easeInOutBack},

    {Type::InElastic, easeInElastic},
    {Type::OutElastic, easeOutElastic},
    {Type::InOutElastic, easeInOutElastic},

    {Type::InBounce, easeInBounce},
    {Type::OutBounce, easeOutBounce},
    {Type::InOutBounce, easeInOutBounce},
};

} // namespace


TEST_CASE("Easing functions have correct boundary values",
          "[Math][Easing]") {
    for (const auto& easing : allFunctions) {
        SECTION(std::string(typeName(easing.type))) {
            REQUIRE_THAT(
                easing.function(0.0),
                WithinAbs(0.0, kTolerance)
            );

            REQUIRE_THAT(
                easing.function(1.0),
                WithinAbs(1.0, kTolerance)
            );
        }
    }
}


TEST_CASE("Easing evaluate dispatches to the correct function",
          "[Math][Easing]") {
    constexpr double values[] = {
        0.0,
        0.1,
        0.25,
        0.5,
        0.75,
        0.9,
        1.0
    };

    for (const auto& easing : allFunctions) {
        SECTION(std::string(typeName(easing.type))) {
            for (const double t : values) {
                REQUIRE_THAT(
                    evaluate(easing.type, t),
                    WithinAbs(easing.function(t), kTolerance)
                );
            }
        }
    }
}


TEST_CASE("Easing typeName returns correct names",
          "[Math][Easing]") {
    REQUIRE(typeName(Type::InSine) == "InSine");
    REQUIRE(typeName(Type::OutSine) == "OutSine");
    REQUIRE(typeName(Type::InOutSine) == "InOutSine");

    REQUIRE(typeName(Type::InQuad) == "InQuad");
    REQUIRE(typeName(Type::OutQuad) == "OutQuad");
    REQUIRE(typeName(Type::InOutQuad) == "InOutQuad");

    REQUIRE(typeName(Type::InCubic) == "InCubic");
    REQUIRE(typeName(Type::OutCubic) == "OutCubic");
    REQUIRE(typeName(Type::InOutCubic) == "InOutCubic");

    REQUIRE(typeName(Type::InQuart) == "InQuart");
    REQUIRE(typeName(Type::OutQuart) == "OutQuart");
    REQUIRE(typeName(Type::InOutQuart) == "InOutQuart");

    REQUIRE(typeName(Type::InQuint) == "InQuint");
    REQUIRE(typeName(Type::OutQuint) == "OutQuint");
    REQUIRE(typeName(Type::InOutQuint) == "InOutQuint");

    REQUIRE(typeName(Type::InExpo) == "InExpo");
    REQUIRE(typeName(Type::OutExpo) == "OutExpo");
    REQUIRE(typeName(Type::InOutExpo) == "InOutExpo");

    REQUIRE(typeName(Type::InCirc) == "InCirc");
    REQUIRE(typeName(Type::OutCirc) == "OutCirc");
    REQUIRE(typeName(Type::InOutCirc) == "InOutCirc");

    REQUIRE(typeName(Type::InBack) == "InBack");
    REQUIRE(typeName(Type::OutBack) == "OutBack");
    REQUIRE(typeName(Type::InOutBack) == "InOutBack");

    REQUIRE(typeName(Type::InElastic) == "InElastic");
    REQUIRE(typeName(Type::OutElastic) == "OutElastic");
    REQUIRE(typeName(Type::InOutElastic) == "InOutElastic");

    REQUIRE(typeName(Type::InBounce) == "InBounce");
    REQUIRE(typeName(Type::OutBounce) == "OutBounce");
    REQUIRE(typeName(Type::InOutBounce) == "InOutBounce");
}


TEST_CASE("Sine easing functions", "[Math][Easing]") {
    SECTION("easeInSine") {
        REQUIRE_THAT(
            easeInSine(0.25),
            WithinAbs(1.0 - std::cos(std::numbers::pi / 8.0), kTolerance)
        );

        REQUIRE_THAT(
            easeInSine(0.5),
            WithinAbs(1.0 - std::numbers::sqrt2 / 2.0, kTolerance)
        );
    }

    SECTION("easeOutSine") {
        REQUIRE_THAT(
            easeOutSine(0.25),
            WithinAbs(std::sin(std::numbers::pi / 8.0), kTolerance)
        );

        REQUIRE_THAT(
            easeOutSine(0.5),
            WithinAbs(std::numbers::sqrt2 / 2.0, kTolerance)
        );
    }

    SECTION("easeInOutSine") {
        REQUIRE_THAT(
            easeInOutSine(0.25),
            WithinAbs(
                -(std::cos(std::numbers::pi / 4.0) - 1.0) / 2.0,
                kTolerance
            )
        );

        REQUIRE_THAT(
            easeInOutSine(0.5),
            WithinAbs(0.5, kTolerance)
        );
    }
}


TEST_CASE("Quadratic easing functions",
          "[Math][Easing]") {
    SECTION("easeInQuad") {
        REQUIRE_THAT(easeInQuad(0.25), WithinAbs(0.0625, kTolerance));
        REQUIRE_THAT(easeInQuad(0.5), WithinAbs(0.25, kTolerance));
    }

    SECTION("easeOutQuad") {
        REQUIRE_THAT(easeOutQuad(0.25), WithinAbs(0.4375, kTolerance));
        REQUIRE_THAT(easeOutQuad(0.5), WithinAbs(0.75, kTolerance));
    }

    SECTION("easeInOutQuad") {
        REQUIRE_THAT(easeInOutQuad(0.25), WithinAbs(0.125, kTolerance));
        REQUIRE_THAT(easeInOutQuad(0.5), WithinAbs(0.5, kTolerance));
        REQUIRE_THAT(easeInOutQuad(0.75), WithinAbs(0.875, kTolerance));
    }
}


TEST_CASE("Cubic easing functions",
          "[Math][Easing]") {
    SECTION("easeInCubic") {
        REQUIRE_THAT(easeInCubic(0.25), WithinAbs(0.015625, kTolerance));
        REQUIRE_THAT(easeInCubic(0.5), WithinAbs(0.125, kTolerance));
    }

    SECTION("easeOutCubic") {
        REQUIRE_THAT(easeOutCubic(0.25), WithinAbs(0.578125, kTolerance));
        REQUIRE_THAT(easeOutCubic(0.5), WithinAbs(0.875, kTolerance));
    }

    SECTION("easeInOutCubic") {
        REQUIRE_THAT(easeInOutCubic(0.25), WithinAbs(0.0625, kTolerance));
        REQUIRE_THAT(easeInOutCubic(0.5), WithinAbs(0.5, kTolerance));
        REQUIRE_THAT(easeInOutCubic(0.75), WithinAbs(0.9375, kTolerance));
    }
}


TEST_CASE("Quartic easing functions",
          "[Math][Easing]") {
    SECTION("easeInQuart") {
        REQUIRE_THAT(easeInQuart(0.5), WithinAbs(0.0625, kTolerance));
    }

    SECTION("easeOutQuart") {
        REQUIRE_THAT(easeOutQuart(0.5), WithinAbs(0.9375, kTolerance));
    }

    SECTION("easeInOutQuart") {
        REQUIRE_THAT(easeInOutQuart(0.25), WithinAbs(0.03125, kTolerance));
        REQUIRE_THAT(easeInOutQuart(0.5), WithinAbs(0.5, kTolerance));
        REQUIRE_THAT(easeInOutQuart(0.75), WithinAbs(0.96875, kTolerance));
    }
}


TEST_CASE("Quintic easing functions",
          "[Math][Easing]") {
    SECTION("easeInQuint") {
        REQUIRE_THAT(easeInQuint(0.5), WithinAbs(0.03125, kTolerance));
    }

    SECTION("easeOutQuint") {
        REQUIRE_THAT(easeOutQuint(0.5), WithinAbs(0.96875, kTolerance));
    }

    SECTION("easeInOutQuint") {
        REQUIRE_THAT(easeInOutQuint(0.25), WithinAbs(0.015625, kTolerance));
        REQUIRE_THAT(easeInOutQuint(0.5), WithinAbs(0.5, kTolerance));
        REQUIRE_THAT(easeInOutQuint(0.75), WithinAbs(0.984375, kTolerance));
    }
}


TEST_CASE("Exponential easing functions",
          "[Math][Easing]") {
    SECTION("easeInExpo") {
        REQUIRE_THAT(
            easeInExpo(0.0),
            WithinAbs(0.0, kTolerance)
        );

        REQUIRE_THAT(
            easeInExpo(0.5),
            WithinAbs(0.03125, kTolerance)
        );
    }

    SECTION("easeOutExpo") {
        REQUIRE_THAT(
            easeOutExpo(0.5),
            WithinAbs(0.96875, kTolerance)
        );

        REQUIRE_THAT(
            easeOutExpo(1.0),
            WithinAbs(1.0, kTolerance)
        );
    }

    SECTION("easeInOutExpo") {
        REQUIRE_THAT(
            easeInOutExpo(0.0),
            WithinAbs(0.0, kTolerance)
        );

        REQUIRE_THAT(
            easeInOutExpo(0.5),
            WithinAbs(0.5, kTolerance)
        );

        REQUIRE_THAT(
            easeInOutExpo(1.0),
            WithinAbs(1.0, kTolerance)
        );
    }
}


TEST_CASE("Circular easing functions",
          "[Math][Easing]") {
    SECTION("easeInCirc") {
        REQUIRE_THAT(
            easeInCirc(0.5),
            WithinAbs(1.0 - std::sqrt(0.75), kTolerance)
        );
    }

    SECTION("easeOutCirc") {
        REQUIRE_THAT(
            easeOutCirc(0.5),
            WithinAbs(std::sqrt(0.75), kTolerance)
        );
    }

    SECTION("easeInOutCirc") {
        REQUIRE_THAT(
            easeInOutCirc(0.25),
            WithinAbs((1.0 - std::sqrt(0.75)) / 2.0, kTolerance)
        );

        REQUIRE_THAT(
            easeInOutCirc(0.5),
            WithinAbs(0.5, kTolerance)
        );

        REQUIRE_THAT(
            easeInOutCirc(0.75),
            WithinAbs((std::sqrt(0.75) + 1.0) / 2.0, kTolerance)
        );
    }
}


TEST_CASE("Back easing functions",
          "[Math][Easing]") {
    constexpr double c1 = 1.70158;
    constexpr double c2 = c1 * 1.525;
    constexpr double c3 = c1 + 1.0;

    SECTION("easeInBack") {
        constexpr double t = 0.25;

        const double expected =
            c3 * t * t * t - c1 * t * t;

        REQUIRE_THAT(
            easeInBack(t),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("easeOutBack") {
        constexpr double t = 0.75;
        constexpr double x = t - 1.0;

        const double expected =
            1.0 + c3 * x * x * x + c1 * x * x;

        REQUIRE_THAT(
            easeOutBack(t),
            WithinAbs(expected, kTolerance)
        );
    }

    SECTION("easeInOutBack") {
        constexpr double t = 0.25;
        constexpr double x = 2.0 * t;

        const double expected =
            (x * x * ((c2 + 1.0) * x - c2)) / 2.0;

        REQUIRE_THAT(
            easeInOutBack(t),
            WithinAbs(expected, kTolerance)
        );

        REQUIRE_THAT(
            easeInOutBack(0.5),
            WithinAbs(0.5, kTolerance)
        );
    }
}


TEST_CASE("Elastic easing functions",
          "[Math][Easing]") {
    SECTION("easeInElastic boundaries") {
        REQUIRE_THAT(easeInElastic(0.0), WithinAbs(0.0, kTolerance));
        REQUIRE_THAT(easeInElastic(1.0), WithinAbs(1.0, kTolerance));
    }

    SECTION("easeOutElastic boundaries") {
        REQUIRE_THAT(easeOutElastic(0.0), WithinAbs(0.0, kTolerance));
        REQUIRE_THAT(easeOutElastic(1.0), WithinAbs(1.0, kTolerance));
    }

    SECTION("easeInOutElastic boundaries") {
        REQUIRE_THAT(easeInOutElastic(0.0), WithinAbs(0.0, kTolerance));
        REQUIRE_THAT(easeInOutElastic(1.0), WithinAbs(1.0, kTolerance));
    }

    SECTION("easeInElastic known value") {
        constexpr double t = 0.5;
        constexpr double c4 = 2.0 * std::numbers::pi / 3.0;

        const double expected =
            -std::pow(2.0, 10.0 * t - 10.0)
            * std::sin((t * 10.0 - 10.75) * c4);

        REQUIRE_THAT(
            easeInElastic(t),
            WithinAbs(expected, kLooseTolerance)
        );
    }

    SECTION("easeOutElastic known value") {
        constexpr double t = 0.5;
        constexpr double c4 = 2.0 * std::numbers::pi / 3.0;

        const double expected =
            std::pow(2.0, -10.0 * t)
            * std::sin((t * 10.0 - 0.75) * c4)
            + 1.0;

        REQUIRE_THAT(
            easeOutElastic(t),
            WithinAbs(expected, kLooseTolerance)
        );
    }

    SECTION("easeInOutElastic midpoint") {
        REQUIRE_THAT(
            easeInOutElastic(0.5),
            WithinAbs(0.5, kLooseTolerance)
        );
    }
}


TEST_CASE("Bounce easing functions",
          "[Math][Easing]") {
    SECTION("easeOutBounce at zero") {
        REQUIRE_THAT(
            easeOutBounce(0.0),
            WithinAbs(0.0, kTolerance)
        );
    }

    SECTION("easeOutBounce at one") {
        REQUIRE_THAT(
            easeOutBounce(1.0),
            WithinAbs(1.0, kTolerance)
        );
    }

    SECTION("easeInBounce at zero and one") {
        REQUIRE_THAT(
            easeInBounce(0.0),
            WithinAbs(0.0, kTolerance)
        );

        REQUIRE_THAT(
            easeInBounce(1.0),
            WithinAbs(1.0, kTolerance)
        );
    }

    SECTION("easeInOutBounce at midpoint") {
        REQUIRE_THAT(
            easeInOutBounce(0.5),
            WithinAbs(0.5, kTolerance)
        );
    }

    SECTION("easeInBounce is the reverse of easeOutBounce") {
        constexpr double values[] = {
            0.1,
            0.25,
            0.4,
            0.6,
            0.75,
            0.9
        };

        for (const double t : values) {
            REQUIRE_THAT(
                easeInBounce(t),
                WithinAbs(1.0 - easeOutBounce(1.0 - t), kTolerance)
            );
        }
    }

    SECTION("easeInOutBounce is symmetric") {
        constexpr double values[] = {
            0.1,
            0.2,
            0.3,
            0.4,
            0.6,
            0.7,
            0.8,
            0.9
        };

        for (const double t : values) {
            REQUIRE_THAT(
                easeInOutBounce(t),
                WithinAbs(1.0 - easeInOutBounce(1.0 - t), kLooseTolerance)
            );
        }
    }
}


TEST_CASE("In and out easing functions are complementary",
          "[Math][Easing]") {
    constexpr double values[] = {
        0.0,
        0.1,
        0.25,
        0.4,
        0.5,
        0.6,
        0.75,
        0.9,
        1.0
    };

    SECTION("Sine") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInSine(t),
                WithinAbs(1.0 - easeOutSine(1.0 - t), kLooseTolerance)
            );
        }
    }

    SECTION("Quad") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInQuad(t),
                WithinAbs(1.0 - easeOutQuad(1.0 - t), kLooseTolerance)
            );
        }
    }

    SECTION("Cubic") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInCubic(t),
                WithinAbs(1.0 - easeOutCubic(1.0 - t), kLooseTolerance)
            );
        }
    }

    SECTION("Quart") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInQuart(t),
                WithinAbs(1.0 - easeOutQuart(1.0 - t), kLooseTolerance)
            );
        }
    }

    SECTION("Quint") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInQuint(t),
                WithinAbs(1.0 - easeOutQuint(1.0 - t), kLooseTolerance)
            );
        }
    }

    SECTION("Expo") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInExpo(t),
                WithinAbs(1.0 - easeOutExpo(1.0 - t), kLooseTolerance)
            );
        }
    }

    SECTION("Circ") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInCirc(t),
                WithinAbs(1.0 - easeOutCirc(1.0 - t), kLooseTolerance)
            );
        }
    }

    SECTION("Back") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInBack(t),
                WithinAbs(1.0 - easeOutBack(1.0 - t), kLooseTolerance)
            );
        }
    }

    SECTION("Elastic") {
        for (const double t : values) {
            REQUIRE_THAT(
                easeInElastic(t),
                WithinAbs(
                    1.0 - easeOutElastic(1.0 - t),
                    kLooseTolerance
                )
            );
        }
    }
}


TEST_CASE("InOut easing functions are centered at 0.5",
          "[Math][Easing]") {
    REQUIRE_THAT(easeInOutSine(0.5), WithinAbs(0.5, kTolerance));
    REQUIRE_THAT(easeInOutQuad(0.5), WithinAbs(0.5, kTolerance));
    REQUIRE_THAT(easeInOutCubic(0.5), WithinAbs(0.5, kTolerance));
    REQUIRE_THAT(easeInOutQuart(0.5), WithinAbs(0.5, kTolerance));
    REQUIRE_THAT(easeInOutQuint(0.5), WithinAbs(0.5, kTolerance));
    REQUIRE_THAT(easeInOutExpo(0.5), WithinAbs(0.5, kTolerance));
    REQUIRE_THAT(easeInOutCirc(0.5), WithinAbs(0.5, kTolerance));
    REQUIRE_THAT(easeInOutBack(0.5), WithinAbs(0.5, kTolerance));
    REQUIRE_THAT(easeInOutElastic(0.5), WithinAbs(0.5, kLooseTolerance));
    REQUIRE_THAT(easeInOutBounce(0.5), WithinAbs(0.5, kTolerance));
}

} // namespace Grain::Easing::Tests