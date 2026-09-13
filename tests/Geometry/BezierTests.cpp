#include <grain/Geometry/Bezier.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <array>
#include <span>
#include <vector>

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

TEST_CASE("Bezier default constructor", "[Bezier]")
{
    const Bezier bezier;

    requirePoint(bezier.startPos(), Vec2d{0.0, 0.0});
    requirePoint(bezier.controlPos1(), Vec2d{0.0, 0.0});
    requirePoint(bezier.controlPos2(), Vec2d{0.0, 0.0});
    requirePoint(bezier.endPos(), Vec2d{0.0, 0.0});
}

TEST_CASE("Bezier constructor from coordinates", "[Bezier]")
{
    const Bezier bezier{
        0.0, 1.0,
        2.0, 3.0,
        4.0, 5.0,
        6.0, 7.0
    };

    requirePoint(bezier.startPos(), Vec2d{0.0, 1.0});
    requirePoint(bezier.controlPos1(), Vec2d{2.0, 3.0});
    requirePoint(bezier.controlPos2(), Vec2d{4.0, 5.0});
    requirePoint(bezier.endPos(), Vec2d{6.0, 7.0});
}

TEST_CASE("Bezier constructor from points", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 1.0},
        Vec2d{2.0, 3.0},
        Vec2d{4.0, 5.0},
        Vec2d{6.0, 7.0}
    };

    requirePoint(bezier.startPos(), Vec2d{0.0, 1.0});
    requirePoint(bezier.controlPos1(), Vec2d{2.0, 3.0});
    requirePoint(bezier.controlPos2(), Vec2d{4.0, 5.0});
    requirePoint(bezier.endPos(), Vec2d{6.0, 7.0});
}

TEST_CASE("Bezier quadratic constructor", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 0.0}
    };

    requirePoint(
        bezier.startPos(),
        Vec2d{0.0, 0.0}
    );

    requirePoint(
        bezier.controlPos1(),
        Vec2d{20.0 / 3.0, 20.0 / 3.0}
    );

    requirePoint(
        bezier.controlPos2(),
        Vec2d{40.0 / 3.0, 20.0 / 3.0}
    );

    requirePoint(
        bezier.endPos(),
        Vec2d{20.0, 0.0}
    );
}

TEST_CASE("Bezier set", "[Bezier]")
{
    Bezier bezier;

    bezier.set(
        Vec2d{1.0, 2.0},
        Vec2d{3.0, 4.0},
        Vec2d{5.0, 6.0},
        Vec2d{7.0, 8.0}
    );

    requirePoint(bezier.startPos(), Vec2d{1.0, 2.0});
    requirePoint(bezier.controlPos1(), Vec2d{3.0, 4.0});
    requirePoint(bezier.controlPos2(), Vec2d{5.0, 6.0});
    requirePoint(bezier.endPos(), Vec2d{7.0, 8.0});
}

TEST_CASE("Bezier setPointAtIndex", "[Bezier]")
{
    Bezier bezier;

    bezier.setPointAtIndex(0, Vec2d{1.0, 2.0});
    bezier.setPointAtIndex(1, Vec2d{3.0, 4.0});
    bezier.setPointAtIndex(2, Vec2d{5.0, 6.0});
    bezier.setPointAtIndex(3, Vec2d{7.0, 8.0});

    requirePoint(bezier.posAtPointIndex(0), Vec2d{1.0, 2.0});
    requirePoint(bezier.posAtPointIndex(1), Vec2d{3.0, 4.0});
    requirePoint(bezier.posAtPointIndex(2), Vec2d{5.0, 6.0});
    requirePoint(bezier.posAtPointIndex(3), Vec2d{7.0, 8.0});
}

TEST_CASE("Bezier posAtPointIndex", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 1.0},
        Vec2d{2.0, 3.0},
        Vec2d{4.0, 5.0},
        Vec2d{6.0, 7.0}
    };

    requirePoint(bezier.posAtPointIndex(0), Vec2d{0.0, 1.0});
    requirePoint(bezier.posAtPointIndex(1), Vec2d{2.0, 3.0});
    requirePoint(bezier.posAtPointIndex(2), Vec2d{4.0, 5.0});
    requirePoint(bezier.posAtPointIndex(3), Vec2d{6.0, 7.0});
}

TEST_CASE("Bezier posOnCurve", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    requirePoint(bezier.posOnCurve(0.0), Vec2d{0.0, 0.0});
    requirePoint(bezier.posOnCurve(1.0), Vec2d{20.0, 10.0});
    requirePoint(bezier.posOnCurve(0.5), Vec2d{10.0, 5.0});
}

TEST_CASE("Bezier bounds", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    const Rectd bounds = bezier.bounds();

    REQUIRE_THAT(
        bounds.x,
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );

    REQUIRE_THAT(
        bounds.y,
        Catch::Matchers::WithinAbs(0.0, epsilon)
    );

    REQUIRE_THAT(
        bounds.width,
        Catch::Matchers::WithinAbs(20.0, epsilon)
    );

    REQUIRE_THAT(
        bounds.height,
        Catch::Matchers::WithinAbs(10.0, epsilon)
    );
}

TEST_CASE("Bezier tangent1 and tangent2", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    requirePoint(
        bezier.tangent1(),
        Vec2d{10.0, 0.0}
    );

    requirePoint(
        bezier.tangent2(),
        Vec2d{-10.0, 0.0}
    );
}

TEST_CASE("Bezier translate", "[Bezier]")
{
    Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    bezier.translate(Vec2d{5.0, -3.0});

    requirePoint(bezier.startPos(), Vec2d{5.0, -3.0});
    requirePoint(bezier.controlPos1(), Vec2d{15.0, -3.0});
    requirePoint(bezier.controlPos2(), Vec2d{15.0, 7.0});
    requirePoint(bezier.endPos(), Vec2d{25.0, 7.0});
}

TEST_CASE("Bezier translateX", "[Bezier]")
{
    Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    bezier.translateX(5.0);

    requirePoint(bezier.startPos(), Vec2d{5.0, 0.0});
    requirePoint(bezier.controlPos1(), Vec2d{15.0, 0.0});
    requirePoint(bezier.controlPos2(), Vec2d{15.0, 10.0});
    requirePoint(bezier.endPos(), Vec2d{25.0, 10.0});
}

TEST_CASE("Bezier translateY", "[Bezier]")
{
    Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    bezier.translateY(-5.0);

    requirePoint(bezier.startPos(), Vec2d{0.0, -5.0});
    requirePoint(bezier.controlPos1(), Vec2d{10.0, -5.0});
    requirePoint(bezier.controlPos2(), Vec2d{10.0, 5.0});
    requirePoint(bezier.endPos(), Vec2d{20.0, 5.0});
}

TEST_CASE("Bezier scale", "[Bezier]")
{
    Bezier bezier{
        Vec2d{1.0, 2.0},
        Vec2d{3.0, 4.0},
        Vec2d{5.0, 6.0},
        Vec2d{7.0, 8.0}
    };

    bezier.scale(2.0, 3.0);

    requirePoint(bezier.startPos(), Vec2d{2.0, 6.0});
    requirePoint(bezier.controlPos1(), Vec2d{6.0, 12.0});
    requirePoint(bezier.controlPos2(), Vec2d{10.0, 18.0});
    requirePoint(bezier.endPos(), Vec2d{14.0, 24.0});
}

TEST_CASE("Bezier scaleX", "[Bezier]")
{
    Bezier bezier{
        Vec2d{1.0, 2.0},
        Vec2d{3.0, 4.0},
        Vec2d{5.0, 6.0},
        Vec2d{7.0, 8.0}
    };

    bezier.scaleX(2.0);

    requirePoint(bezier.startPos(), Vec2d{2.0, 2.0});
    requirePoint(bezier.controlPos1(), Vec2d{6.0, 4.0});
    requirePoint(bezier.controlPos2(), Vec2d{10.0, 6.0});
    requirePoint(bezier.endPos(), Vec2d{14.0, 8.0});
}

TEST_CASE("Bezier scaleY", "[Bezier]")
{
    Bezier bezier{
        Vec2d{1.0, 2.0},
        Vec2d{3.0, 4.0},
        Vec2d{5.0, 6.0},
        Vec2d{7.0, 8.0}
    };

    bezier.scaleY(2.0);

    requirePoint(bezier.startPos(), Vec2d{1.0, 4.0});
    requirePoint(bezier.controlPos1(), Vec2d{3.0, 8.0});
    requirePoint(bezier.controlPos2(), Vec2d{5.0, 12.0});
    requirePoint(bezier.endPos(), Vec2d{7.0, 16.0});
}

TEST_CASE("Bezier approximatedCurveLength", "[Bezier]")
{
    const Bezier line{
        Vec2d{0.0, 0.0},
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 0.0}
    };

    REQUIRE_THAT(
        line.approximatedCurveLength(100),
        Catch::Matchers::WithinAbs(10.0, epsilon)
    );
}

TEST_CASE("Bezier split", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    Bezier left;
    Bezier right;

    REQUIRE(bezier.split(0.5, left, right));

    requirePoint(left.startPos(), bezier.startPos());
    requirePoint(right.endPos(), bezier.endPos());

    requirePoint(
        left.endPos(),
        right.startPos()
    );

    requirePoint(
        left.endPos(),
        bezier.posOnCurve(0.5)
    );
}

TEST_CASE("Bezier split parameter", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    Bezier left;
    Bezier right;

    REQUIRE(bezier.split(0.25, left, right));

    requirePoint(
        left.endPos(),
        bezier.posOnCurve(0.25)
    );

    requirePoint(
        right.startPos(),
        bezier.posOnCurve(0.25)
    );
}

TEST_CASE("Bezier truncate", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    Bezier result;

    REQUIRE(bezier.truncate(0.25, 0.75, result));

    requirePoint(
        result.startPos(),
        bezier.posOnCurve(0.25)
    );

    requirePoint(
        result.endPos(),
        bezier.posOnCurve(0.75)
    );
}

TEST_CASE("Bezier buildVec2LUT", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    std::array<Vec2d, 5> lut{};

    bezier.buildVec2LUT(lut.data(), static_cast<int32_t>(lut.size()));

    requirePoint(lut[0], bezier.posOnCurve(0.0));
    requirePoint(lut[1], bezier.posOnCurve(0.25));
    requirePoint(lut[2], bezier.posOnCurve(0.5));
    requirePoint(lut[3], bezier.posOnCurve(0.75));
    requirePoint(lut[4], bezier.posOnCurve(1.0));
}

TEST_CASE("Bezier hitPoint", "[Bezier]")
{
    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    REQUIRE(bezier.hitPoint(Vec2d{0.0, 0.0}, 1.0) == 0);
    REQUIRE(bezier.hitPoint(Vec2d{10.0, 0.0}, 1.0) == 1);
    REQUIRE(bezier.hitPoint(Vec2d{10.0, 10.0}, 1.0) == 2);
    REQUIRE(bezier.hitPoint(Vec2d{20.0, 10.0}, 1.0) == 3);
}

TEST_CASE("Bezier fitCubicBezierToPoints", "[Bezier]")
{
    const std::array<Vec2d, 5> points{
        Vec2d{0.0, 0.0},
        Vec2d{2.5, 0.0},
        Vec2d{5.0, 0.0},
        Vec2d{7.5, 0.0},
        Vec2d{10.0, 0.0}
    };

    const auto result =
        Bezier::fitCubicBezierToPoints(std::span<const Vec2d>{points});

    REQUIRE(result.has_value());

    requirePoint(result->startPos(), points.front());
    requirePoint(result->endPos(), points.back());

    REQUIRE_THAT(
        result->approximatedCurveLength(100),
        Catch::Matchers::WithinAbs(10.0, epsilon)
    );
}

TEST_CASE("Bezier fitCubicBezierToPoints rejects invalid input", "[Bezier]")
{
    const std::array<Vec2d, 1> points{
        Vec2d{0.0, 0.0}
    };

    const auto result =
        Bezier::fitCubicBezierToPoints(std::span<const Vec2d>{points});

    REQUIRE_FALSE(result.has_value());
}