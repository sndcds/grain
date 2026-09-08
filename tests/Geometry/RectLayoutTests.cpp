#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <grain/Geometry/RectLayout.hpp>

using namespace Grain;

namespace {

template <typename T>
void requireRect(
    const Rect<T>& actual,
    const Rect<T>& expected
)
{
    REQUIRE(actual.x == expected.x);
    REQUIRE(actual.y == expected.y);
    REQUIRE(actual.width == expected.width);
    REQUIRE(actual.height == expected.height);
}

void requireRectApprox(
    const Rectd& actual,
    const Rectd& expected,
    double epsilon = 1e-10
)
{
    REQUIRE(actual.x == Catch::Approx(expected.x).margin(epsilon));
    REQUIRE(actual.y == Catch::Approx(expected.y).margin(epsilon));
    REQUIRE(actual.width == Catch::Approx(expected.width).margin(epsilon));
    REQUIRE(actual.height == Catch::Approx(expected.height).margin(epsilon));
}

} // namespace


TEST_CASE("RectLayout fitRect", "[RectLayout]")
{
    const Rectd source{0.0, 0.0, 100.0, 50.0};
    const Rectd destination{10.0, 20.0, 400.0, 300.0};

    SECTION("fit preserves aspect ratio")
    {
        const auto result =
            RectLayoutd::fitRect(source, destination, FitMode::Fit);

        requireRectApprox(
            result,
            Rectd{10.0, 70.0, 400.0, 200.0}
        );
    }

    SECTION("cover preserves aspect ratio")
    {
        const auto result =
            RectLayoutd::fitRect(source, destination, FitMode::Cover);

        requireRectApprox(
            result,
            Rectd{-90.0, 20.0, 600.0, 300.0}
        );
    }

    SECTION("stretch fills destination")
    {
        const auto result =
            RectLayoutd::fitRect(source, destination, FitMode::Stretch);

        requireRect(result, destination);
    }

    SECTION("center preserves source size")
    {
        const auto result =
            RectLayoutd::fitRect(source, destination, FitMode::Center);

        requireRect(
            result,
            Rectd{160.0, 145.0, 100.0, 50.0}
        );
    }

    SECTION("zero source width returns empty rect")
    {
        const auto result =
            RectLayoutd::fitRect(
                Rectd{0.0, 0.0, 0.0, 50.0},
                destination,
                FitMode::Fit
            );

        requireRect(result, Rectd{});
    }

    SECTION("zero source height returns empty rect")
    {
        const auto result =
            RectLayoutd::fitRect(
                Rectd{0.0, 0.0, 100.0, 0.0},
                destination,
                FitMode::Cover
            );

        requireRect(result, Rectd{});
    }
}


TEST_CASE("RectLayout centeredSquare", "[RectLayout]")
{
    SECTION("wide rectangle")
    {
        const auto result =
            RectLayoutd::centeredSquare(
                Rectd{10.0, 20.0, 200.0, 100.0}
            );

        requireRect(
            result,
            Rectd{60.0, 20.0, 100.0, 100.0}
        );
    }

    SECTION("tall rectangle")
    {
        const auto result =
            RectLayoutd::centeredSquare(
                Rectd{10.0, 20.0, 100.0, 200.0}
            );

        requireRect(
            result,
            Rectd{10.0, 70.0, 100.0, 100.0}
        );
    }

    SECTION("square remains unchanged")
    {
        const Rectd rect{10.0, 20.0, 100.0, 100.0};

        requireRect(
            RectLayoutd::centeredSquare(rect),
            rect
        );
    }
}


TEST_CASE("RectLayout paddedRect", "[RectLayout]")
{
    const Rectd rect{10.0, 20.0, 200.0, 100.0};

    SECTION("applies individual padding")
    {
        const auto result =
            RectLayoutd::paddedRect(
                rect,
                5.0,
                10.0,
                15.0,
                20.0
            );

        requireRect(
            result,
            Rectd{30.0, 25.0, 170.0, 80.0}
        );
    }

    SECTION("symmetric padding")
    {
        const auto result =
            RectLayoutd::paddedRect(
                rect,
                10.0,
                10.0,
                10.0,
                10.0
            );

        requireRect(
            result,
            Rectd{20.0, 30.0, 180.0, 80.0}
        );
    }

    SECTION("padding cannot produce negative size")
    {
        const auto result =
            RectLayoutd::paddedRect(
                rect,
                100.0,
                100.0,
                100.0,
                100.0
            );

        requireRect(
            result,
            Rectd{110.0, 120.0, 0.0, 0.0}
        );
    }

    SECTION("top and right are not confused")
    {
        const auto result =
            RectLayoutd::paddedRect(
                rect,
                5.0,
                20.0,
                15.0,
                30.0
            );

        REQUIRE(result.x == 40.0);
        REQUIRE(result.y == 25.0);
    }
}


TEST_CASE("RectLayout rectInRectNormalized", "[RectLayout]")
{
    const Rectd container{10.0, 20.0, 200.0, 100.0};
    const Rectd rect{0.0, 0.0, 50.0, 20.0};

    SECTION("top left")
    {
        requireRect(
            RectLayoutd::rectInRectNormalized(
                container,
                rect,
                0.0,
                0.0
            ),
            Rectd{10.0, 20.0, 50.0, 20.0}
        );
    }

    SECTION("center")
    {
        requireRect(
            RectLayoutd::rectInRectNormalized(
                container,
                rect,
                0.5,
                0.5
            ),
            Rectd{85.0, 60.0, 50.0, 20.0}
        );
    }

    SECTION("bottom right")
    {
        requireRect(
            RectLayoutd::rectInRectNormalized(
                container,
                rect,
                1.0,
                1.0
            ),
            Rectd{160.0, 100.0, 50.0, 20.0}
        );
    }
}


TEST_CASE("RectLayout alignedPos", "[RectLayout]")
{
    const Recti rect{10, 20, 100, 60};

    SECTION("top left")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::TopLeft)
            == Vec2i{10, 20}
        );
    }

    SECTION("left")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::Left)
            == Vec2i{10, 50}
        );
    }

    SECTION("bottom left")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::BottomLeft)
            == Vec2i{10, 80}
        );
    }

    SECTION("top")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::Top)
            == Vec2i{60, 20}
        );
    }

    SECTION("center")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::Center)
            == Vec2i{60, 50}
        );
    }

    SECTION("bottom")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::Bottom)
            == Vec2i{60, 80}
        );
    }

    SECTION("top right")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::TopRight)
            == Vec2i{110, 20}
        );
    }

    SECTION("right")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::Right)
            == Vec2i{110, 50}
        );
    }

    SECTION("bottom right")
    {
        REQUIRE(
            RectLayouti::alignedPos(rect, Alignment::BottomRight)
            == Vec2i{110, 80}
        );
    }
}


TEST_CASE("RectLayout alignedRect", "[RectLayout]")
{
    const Recti container{10, 20, 100, 60};

    SECTION("top left")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::TopLeft,
                20,
                10
            ),
            Recti{10, 20, 20, 10}
        );
    }

    SECTION("top")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::Top,
                20,
                10
            ),
            Recti{50, 20, 20, 10}
        );
    }

    SECTION("top right")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::TopRight,
                20,
                10
            ),
            Recti{90, 20, 20, 10}
        );
    }

    SECTION("left")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::Left,
                20,
                10
            ),
            Recti{10, 45, 20, 10}
        );
    }

    SECTION("center")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::Center,
                20,
                10
            ),
            Recti{50, 45, 20, 10}
        );
    }

    SECTION("right")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::Right,
                20,
                10
            ),
            Recti{90, 45, 20, 10}
        );
    }

    SECTION("bottom left")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::BottomLeft,
                20,
                10
            ),
            Recti{10, 70, 20, 10}
        );
    }

    SECTION("bottom")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::Bottom,
                20,
                10
            ),
            Recti{50, 70, 20, 10}
        );
    }

    SECTION("bottom right")
    {
        requireRect(
            RectLayouti::alignedRect(
                container,
                Alignment::BottomRight,
                20,
                10
            ),
            Recti{90, 70, 20, 10}
        );
    }
}


TEST_CASE("RectLayout cellRect", "[RectLayout]")
{
    const Rectd rect{10.0, 20.0, 300.0, 200.0};

    SECTION("single cell without spacing")
    {
        const auto result =
            RectLayoutd::cellRect(
                rect,
                3,
                2,
                0.0,
                0.0,
                0,
                0
            );

        requireRectApprox(
            result,
            Rectd{10.0, 20.0, 100.0, 100.0}
        );
    }

    SECTION("cell with spacing")
    {
        const auto result =
            RectLayoutd::cellRect(
                rect,
                3,
                2,
                10.0,
                20.0,
                1,
                1
            );

        requireRectApprox(
            result,
            Rectd{113.333333333, 130.0, 93.333333333, 90.0}
        );
    }

    SECTION("column span")
    {
        const auto result =
            RectLayoutd::cellRect(
                rect,
                3,
                2,
                10.0,
                20.0,
                0,
                0,
                2,
                1
            );

        requireRectApprox(
                result,
                Rectd{
                    10.0,
                    20.0,
                    196.666666667,
                    90.0
                }
            );
    }

    SECTION("row span")
    {
        const auto result =
            RectLayoutd::cellRect(
                rect,
                3,
                2,
                10.0,
                20.0,
                0,
                0,
                1,
                2
            );

        requireRectApprox(
            result,
            Rectd{10.0, 20.0, 93.3333333333, 180.0}
        );
    }

    SECTION("span is clipped to grid")
    {
        const auto result =
            RectLayoutd::cellRect(
                rect,
                3,
                2,
                10.0,
                20.0,
                2,
                1,
                10,
                10
            );

        REQUIRE(result.x == Catch::Approx(216.6666666667));
        REQUIRE(result.y == Catch::Approx(140.0));
        REQUIRE(result.width == Catch::Approx(93.3333333333));
        REQUIRE(result.height == Catch::Approx(80.0));
    }

    SECTION("indices are clamped")
    {
        const auto result =
            RectLayoutd::cellRect(
                rect,
                3,
                2,
                10.0,
                20.0,
                -100,
                -100
            );

        requireRectApprox(
            result,
            Rectd{10.0, 20.0, 93.3333333333, 80.0}
        );
    }

    SECTION("rounding")
    {
        const auto result =
            RectLayoutd::cellRect(
                Rectd{0.0, 0.0, 101.0, 101.0},
                3,
                3,
                1.0,
                1.0,
                1,
                1,
                1,
                1,
                true
            );

        REQUIRE(result.x == 34.0);
        REQUIRE(result.y == 34.0);
        REQUIRE(result.width == 33.0);
        REQUIRE(result.height == 33.0);
    }
}


TEST_CASE("RectLayout edgeAlignedRectRelative", "[RectLayout]")
{
    const Rectd rect{100.0, 200.0, 300.0, 150.0};

    SECTION("center")
    {
        requireRectApprox(
            RectLayoutd::edgeAlignedRectRelative(
                rect,
                Alignment::Center,
                10.0,
                20.0,
                30.0,
                40.0
            ),
            Rectd{10.0, 20.0, 260.0, 140.0}
        );
    }

    SECTION("top left")
    {
        requireRectApprox(
            RectLayoutd::edgeAlignedRectRelative(
                rect,
                Alignment::TopLeft,
                10.0,
                20.0,
                30.0,
                40.0
            ),
            Rectd{40.0, 10.0, 240.0, 110.0}
        );
    }

    SECTION("bottom right")
    {
        requireRectApprox(
            RectLayoutd::edgeAlignedRectRelative(
                rect,
                Alignment::BottomRight,
                10.0,
                20.0,
                30.0,
                40.0
            ),
            Rectd{280.0, 90.0, 240.0, 110.0}
        );
    }

    SECTION("left")
    {
        requireRectApprox(
            RectLayoutd::edgeAlignedRectRelative(
                rect,
                Alignment::Left,
                10.0,
                20.0,
                30.0,
                40.0
            ),
            Rectd{40.0, 10.0, 20.0, 110.0}
        );
    }

    SECTION("right")
    {
        requireRectApprox(
            RectLayoutd::edgeAlignedRectRelative(
                rect,
                Alignment::Right,
                10.0,
                20.0,
                30.0,
                40.0
            ),
            Rectd{280.0, 10.0, 20.0, 110.0}
        );
    }
}


TEST_CASE("RectLayout innerRect", "[RectLayout]")
{
    const Rectd rect{100.0, 200.0, 400.0, 200.0};

    SECTION("fits aspect ratio inside padded rectangle")
    {
        const auto result =
            RectLayoutd::innerRect(
                rect,
                20.0,
                1.0
            );

        requireRectApprox(
            result,
            Rectd{200.0, 220.0, 200.0, 160.0}
        );
    }

    SECTION("wide aspect ratio")
    {
        const auto result =
            RectLayoutd::innerRect(
                rect,
                20.0,
                2.0
            );

        requireRectApprox(
            result,
            Rectd{120.0, 240.0, 360.0, 180.0}
        );
    }

    SECTION("invalid aspect ratio returns padded area")
    {
        const auto result =
            RectLayoutd::innerRect(
                rect,
                20.0,
                0.0
            );

        requireRect(
            result,
            Rectd{120.0, 220.0, 360.0, 160.0}
        );
    }

    SECTION("preserves rectangle position")
    {
        const auto result =
            RectLayoutd::innerRect(
                Rectd{500.0, 700.0, 400.0, 200.0},
                20.0,
                1.0
            );

        REQUIRE(result.x >= 500.0);
        REQUIRE(result.y >= 700.0);
    }
}


TEST_CASE("RectLayout inset", "[RectLayout]")
{
    const Recti rect{10, 20, 200, 100};

    SECTION("uniform")
    {
        requireRect(
            RectLayouti::inset(rect, 10),
            Recti{20, 30, 180, 80}
        );
    }

    SECTION("horizontal and vertical")
    {
        requireRect(
            RectLayouti::inset(rect, 20, 10),
            Recti{30, 30, 160, 80}
        );
    }

    SECTION("individual edges")
    {
        requireRect(
            RectLayouti::inset(rect, 1, 2, 3, 4),
            Recti{14, 21, 194, 96}
        );
    }

    SECTION("left")
    {
        requireRect(
            RectLayouti::insetLeft(rect, 10),
            Recti{20, 20, 190, 100}
        );
    }

    SECTION("right")
    {
        requireRect(
            RectLayouti::insetRight(rect, 10),
            Recti{10, 20, 190, 100}
        );
    }

    SECTION("top")
    {
        requireRect(
            RectLayouti::insetTop(rect, 10),
            Recti{10, 30, 200, 90}
        );
    }

    SECTION("bottom")
    {
        requireRect(
            RectLayouti::insetBottom(rect, 10),
            Recti{10, 20, 200, 90}
        );
    }

    SECTION("horizontal")
    {
        requireRect(
            RectLayouti::insetHorizontal(rect, 10),
            Recti{20, 20, 180, 100}
        );
    }

    SECTION("vertical")
    {
        requireRect(
            RectLayouti::insetVertical(rect, 10),
            Recti{10, 30, 200, 80}
        );
    }
}


TEST_CASE("RectLayout expand", "[RectLayout]")
{
    const Recti rect{20, 30, 100, 50};

    requireRect(
        RectLayouti::expand(rect, 10),
        Recti{10, 20, 120, 70}
    );
}


TEST_CASE("RectLayout size positioning", "[RectLayout]")
{
    const Recti rect{10, 20, 100, 60};

    SECTION("set width from center")
    {
        requireRect(
            RectLayouti::setWidthFromCenter(rect, 40),
            Recti{40, 20, 40, 60}
        );
    }

    SECTION("set width from max")
    {
        requireRect(
            RectLayouti::setWidthFromMax(rect, 40),
            Recti{70, 20, 40, 60}
        );
    }

    SECTION("set height from center")
    {
        requireRect(
            RectLayouti::setHeightFromCenter(rect, 20),
            Recti{10, 40, 100, 20}
        );
    }

    SECTION("set height from max")
    {
        requireRect(
            RectLayouti::setHeightFromMax(rect, 20),
            Recti{10, 60, 100, 20}
        );
    }

    SECTION("set size from center")
    {
        requireRect(
            RectLayouti::setSizeFromCenter(rect, 40, 20),
            Recti{40, 40, 40, 20}
        );
    }
}


TEST_CASE("RectLayout translate", "[RectLayout]")
{
    const Recti rect{10, 20, 100, 50};

    SECTION("positive translation")
    {
        requireRect(
            RectLayouti::translate(rect, 5, 10),
            Recti{15, 30, 100, 50}
        );
    }

    SECTION("negative translation")
    {
        requireRect(
            RectLayouti::translate(rect, -5, -10),
            Recti{5, 10, 100, 50}
        );
    }

    SECTION("original remains unchanged")
    {
        const auto result =
            RectLayouti::translate(rect, 5, 10);

        requireRect(rect, Recti{10, 20, 100, 50});
        requireRect(result, Recti{15, 30, 100, 50});
    }
}


TEST_CASE("RectLayout scale", "[RectLayout]")
{
    const Rectd rect{10.0, 20.0, 100.0, 50.0};

    SECTION("uniformly scales position and size")
    {
        requireRectApprox(
            RectLayoutd::scale(rect, 2.0),
            Rectd{20.0, 40.0, 200.0, 100.0}
        );
    }

    SECTION("non-uniformly scales position and size")
    {
        requireRectApprox(
            RectLayoutd::scale(rect, 2.0, 3.0),
            Rectd{20.0, 60.0, 200.0, 150.0}
        );
    }

    SECTION("scaleSize keeps position")
    {
        requireRectApprox(
            RectLayoutd::scaleSize(rect, 2.0),
            Rectd{10.0, 20.0, 200.0, 100.0}
        );
    }

    SECTION("non-uniform scaleSize keeps position")
    {
        requireRectApprox(
            RectLayoutd::scaleSize(rect, 2.0, 3.0),
            Rectd{10.0, 20.0, 200.0, 150.0}
        );
    }

    SECTION("scaleCentered keeps center")
    {
        requireRectApprox(
            RectLayoutd::scaleCentered(rect, 2.0),
            Rectd{-40.0, -5.0, 200.0, 100.0}
        );
    }

    SECTION("scaleCentered by one is unchanged")
    {
        requireRectApprox(
            RectLayoutd::scaleCentered(rect, 1.0),
            rect
        );
    }
}


TEST_CASE("RectLayout alignInRect", "[RectLayout]")
{
    const Rectd source{0.0, 0.0, 40.0, 20.0};
    const Rectd container{100.0, 200.0, 200.0, 100.0};

    SECTION("center")
    {
        requireRectApprox(
            RectLayoutd::alignInRect(
                source,
                Alignment::Center,
                container
            ),
            Rectd{180.0, 240.0, 40.0, 20.0}
        );
    }

    SECTION("top left")
    {
        requireRectApprox(
            RectLayoutd::alignInRect(
                source,
                Alignment::TopLeft,
                container
            ),
            Rectd{100.0, 200.0, 40.0, 20.0}
        );
    }

    SECTION("bottom right")
    {
        requireRectApprox(
            RectLayoutd::alignInRect(
                source,
                Alignment::BottomRight,
                container
            ),
            Rectd{260.0, 280.0, 40.0, 20.0}
        );
    }

    SECTION("source position is ignored")
    {
        const Rectd movedSource{500.0, 700.0, 40.0, 20.0};

        requireRectApprox(
            RectLayoutd::alignInRect(
                movedSource,
                Alignment::Center,
                container
            ),
            Rectd{180.0, 240.0, 40.0, 20.0}
        );
    }
}


TEST_CASE("RectLayout makePositiveSize", "[RectLayout]")
{
    SECTION("already positive")
    {
        const Recti rect{10, 20, 100, 50};

        requireRect(
            RectLayouti::makePositiveSize(rect),
            rect
        );
    }

    SECTION("negative width moves x")
    {
        requireRect(
            RectLayouti::makePositiveSize(
                Recti{110, 20, -100, 50}
            ),
            Recti{10, 20, 100, 50}
        );
    }

    SECTION("negative height moves y")
    {
        requireRect(
            RectLayouti::makePositiveSize(
                Recti{10, 70, 100, -50}
            ),
            Recti{10, 20, 100, 50}
        );
    }

    SECTION("negative width and height")
    {
        requireRect(
            RectLayouti::makePositiveSize(
                Recti{110, 70, -100, -50}
            ),
            Recti{10, 20, 100, 50}
        );
    }
}


TEST_CASE("RectLayout avoidNegativeSize", "[RectLayout]")
{
    SECTION("positive size remains unchanged")
    {
        const Recti rect{10, 20, 100, 50};

        requireRect(
            RectLayouti::avoidNegativeSize(rect),
            rect
        );
    }

    SECTION("negative width becomes zero")
    {
        requireRect(
            RectLayouti::avoidNegativeSize(
                Recti{10, 20, -100, 50}
            ),
            Recti{10, 20, 0, 50}
        );
    }

    SECTION("negative height becomes zero")
    {
        requireRect(
            RectLayouti::avoidNegativeSize(
                Recti{10, 20, 100, -50}
            ),
            Recti{10, 20, 100, 0}
        );
    }

    SECTION("negative width and height")
    {
        requireRect(
            RectLayouti::avoidNegativeSize(
                Recti{10, 20, -100, -50}
            ),
            Recti{10, 20, 0, 0}
        );
    }
}


TEST_CASE("RectLayout makeValidForArea", "[RectLayout]")
{
    SECTION("already valid")
    {
        const Recti rect{10, 20, 50, 30};

        requireRect(
            RectLayouti::makeValidForArea(rect, 100, 100),
            rect
        );
    }

    SECTION("clips right edge")
    {
        requireRect(
            RectLayouti::makeValidForArea(
                Recti{80, 20, 50, 30},
                100,
                100
            ),
            Recti{80, 20, 20, 30}
        );
    }

    SECTION("clips bottom edge")
    {
        requireRect(
            RectLayouti::makeValidForArea(
                Recti{20, 80, 30, 50},
                100,
                100
            ),
            Recti{20, 80, 30, 20}
        );
    }

    SECTION("clips negative x")
    {
        requireRect(
            RectLayouti::makeValidForArea(
                Recti{-20, 10, 50, 30},
                100,
                100
            ),
            Recti{0, 10, 30, 30}
        );
    }

    SECTION("clips negative y")
    {
        requireRect(
            RectLayouti::makeValidForArea(
                Recti{10, -20, 30, 50},
                100,
                100
            ),
            Recti{10, 0, 30, 30}
        );
    }

    SECTION("completely outside returns empty")
    {
        requireRect(
            RectLayouti::makeValidForArea(
                Recti{150, 150, 20, 20},
                100,
                100
            ),
            Recti{}
        );
    }

    SECTION("invalid area returns empty")
    {
        requireRect(
            RectLayouti::makeValidForArea(
                Recti{10, 10, 20, 20},
                0,
                100
            ),
            Recti{}
        );
    }

    SECTION("negative size is normalized first")
    {
        requireRect(
            RectLayouti::makeValidForArea(
                Recti{60, 60, -30, -30},
                100,
                100
            ),
            Recti{30, 30, 30, 30}
        );
    }
}