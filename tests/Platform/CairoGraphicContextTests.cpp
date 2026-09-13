#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../../src/Platform/Linux/CairoGraphicContext.hpp"

#include <cstdint>
#include <cstring>
#include <numbers>

namespace {

using Grain::Platform::CairoGraphicContext;

struct CairoCanvas {
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 64, 64);
    cairo_t* native = cairo_create(surface);
    CairoGraphicContext context{native};

    CairoCanvas() {
        REQUIRE(cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS);
        REQUIRE(cairo_status(native) == CAIRO_STATUS_SUCCESS);
        cairo_set_antialias(native, CAIRO_ANTIALIAS_NONE);
    }

    ~CairoCanvas() {
        cairo_destroy(native);
        cairo_surface_destroy(surface);
    }

    std::uint32_t pixel(int x, int y) const {
        cairo_surface_flush(surface);
        const auto* data = cairo_image_surface_get_data(surface);
        const int stride = cairo_image_surface_get_stride(surface);
        std::uint32_t value = 0;
        std::memcpy(&value, data + y * stride + x * 4, sizeof(value));
        return value;
    }
};

Grain::GraphicPath square() {
    Grain::GraphicPath path;
    path.addPoint(8, 8);
    path.addPoint(24, 8);
    path.addPoint(24, 24);
    path.addPoint(8, 24);
    path.setClosed(true);
    return path;
}

} // namespace

TEST_CASE("Cairo: fill and stroke colors are independent", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    auto& context = canvas.context;
    context.setFillColor({1, 0, 0});
    context.setStrokeColor({0, 0, 1});
    context.setLineWidth(4);
    context.fillRect({8, 8, 16, 16});
    context.strokeRect({8, 8, 16, 16});
    REQUIRE(canvas.pixel(16, 16) == 0xffff0000u);
    REQUIRE(canvas.pixel(8, 16) == 0xff0000ffu);
    REQUIRE(canvas.pixel(4, 16) == 0u);

    context.setFillColor({0, 1, 0});
    context.strokeRect({36, 8, 16, 16});
    context.fillRect({40, 12, 8, 8});
    REQUIRE(canvas.pixel(36, 16) == 0xff0000ffu);
    REQUIRE(canvas.pixel(44, 16) == 0xff00ff00u);
}

TEST_CASE("Cairo: default colors and straight alpha render correctly", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    auto& context = canvas.context;
    context.fillRect({0, 0, 8, 8});
    context.setLineWidth(2);
    context.strokeRect({16, 2, 8, 8});
    REQUIRE(canvas.pixel(4, 4) == 0xff000000u);
    REQUIRE(canvas.pixel(16, 4) == 0xff000000u);

    context.setFillColor({1, 0, 0, 0.5f});
    context.fillRect({32, 0, 8, 8});
    REQUIRE(canvas.pixel(36, 4) == 0x80800000u);
    context.setStrokeColor({0, 0, 1, 0.5f});
    context.strokeRect({48, 2, 8, 8});
    REQUIRE(canvas.pixel(48, 4) == 0x80000080u);
}

TEST_CASE("Cairo: nested saves restore colors and native state", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    auto& context = canvas.context;
    context.setFillColor({1, 0, 0});
    context.setStrokeColor({0, 0, 1});
    context.setLineWidth(4);
    context.save();
    context.translate(32, 0);
    context.setFillColor({0, 1, 0});
    context.setStrokeColor({1, 1, 0});
    context.setLineWidth(2);
    context.save();
    context.scale(2, 3);
    context.setFillColor(Grain::Color::white());
    context.setStrokeColor(Grain::Color::white());
    context.setLineWidth(8);
    context.restore();
    REQUIRE(cairo_get_line_width(canvas.native) == 2);
    context.fillRect({8, 8, 16, 16});
    context.strokeRect({8, 8, 16, 16});
    REQUIRE(canvas.pixel(48, 16) == 0xff00ff00u);
    REQUIRE(canvas.pixel(40, 16) == 0xffffff00u);

    context.restore();
    REQUIRE(cairo_get_line_width(canvas.native) == 4);
    context.fillRect({8, 8, 16, 16});
    context.strokeRect({8, 8, 16, 16});
    REQUIRE(canvas.pixel(16, 16) == 0xffff0000u);
    REQUIRE(canvas.pixel(7, 16) == 0xff0000ffu);

    // An unmatched restore must not put Cairo into a permanent error state.
    context.restore();
    REQUIRE(cairo_status(canvas.native) == CAIRO_STATUS_SUCCESS);
}

TEST_CASE("Cairo: GraphicPath can be filled then stroked", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    auto& context = canvas.context;
    const auto path = square();
    context.setFillColor({1, 0, 0});
    context.setStrokeColor({0, 0, 1});
    context.setLineWidth(4);
    context.fillPath(path);
    REQUIRE_FALSE(cairo_has_current_point(canvas.native));
    context.strokePath(path);
    REQUIRE_FALSE(cairo_has_current_point(canvas.native));
    REQUIRE(canvas.pixel(16, 16) == 0xffff0000u);
    REQUIRE(canvas.pixel(8, 16) == 0xff0000ffu);
    REQUIRE(canvas.pixel(16, 8) == 0xff0000ffu);
    REQUIRE(canvas.pixel(4, 16) == 0u);
}

TEST_CASE("Cairo: open paths retain their endpoints when stroked", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    Grain::GraphicPath path;
    path.addPoint(8, 8);
    path.addPoint(24, 8);
    path.addPoint(24, 24);
    canvas.context.setLineWidth(2);
    canvas.context.strokePath(path);
    REQUIRE(canvas.pixel(16, 8) == 0xff000000u);
    REQUIRE(canvas.pixel(24, 16) == 0xff000000u);
    REQUIRE(canvas.pixel(16, 16) == 0u);
}

TEST_CASE("Cairo: Bezier handles render curved path segments", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    Grain::GraphicPath path;
    SECTION("curve between consecutive points") {
        path.addPointRight(8, 32, 8, 0);
        path.addPointLeft(40, 32, 40, 0);
    }
    SECTION("curve closing the path") {
        path.addPointLeft(40, 32, 40, 0);
        path.addPointRight(8, 32, 8, 0);
    }
    path.setClosed(true);
    canvas.context.setFillColor({1, 0, 0});
    canvas.context.fillPath(path);
    canvas.context.setStrokeColor({0, 0, 1});
    canvas.context.setLineWidth(2);
    canvas.context.strokePath(path);
    REQUIRE(canvas.pixel(24, 20) == 0xffff0000u);
    REQUIRE(canvas.pixel(24, 8) == 0xff0000ffu);
    REQUIRE(canvas.pixel(24, 32) == 0xff0000ffu);
    REQUIRE(canvas.pixel(10, 10) == 0u);
}

TEST_CASE("Cairo: empty and single-point paths draw nothing", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    Grain::GraphicPath path;
    SECTION("empty") {}
    SECTION("single point") {
        path.addPoint(8, 8);
    }
    canvas.context.fillPath(path);
    canvas.context.strokePath(path);
    REQUIRE(cairo_status(canvas.native) == CAIRO_STATUS_SUCCESS);
    REQUIRE_FALSE(cairo_has_current_point(canvas.native));
    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {
            REQUIRE(canvas.pixel(x, y) == 0u);
        }
    }
}

TEST_CASE("Cairo: manually constructed paths append and are consumed", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    auto& context = canvas.context;
    context.moveTo({48, 48});
    context.lineTo({60, 60});
    context.beginPath();
    REQUIRE_FALSE(cairo_has_current_point(canvas.native));
    context.moveTo({8, 32});
    context.curveTo({8, 0}, {40, 0}, {40, 32});
    context.lineTo({8, 32});
    context.closePath();
    // Saving graphics state must not save the current path.
    context.save();
    context.moveTo({48, 8});
    context.lineTo({56, 8});
    context.lineTo({56, 24});
    context.closePath();
    context.restore();
    context.fillPath(Grain::GraphicPath{});
    REQUIRE(canvas.pixel(24, 20) == 0xff000000u);
    REQUIRE(canvas.pixel(54, 12) == 0xff000000u);
    REQUIRE(canvas.pixel(52, 52) == 0u);
    REQUIRE_FALSE(cairo_has_current_point(canvas.native));
}

TEST_CASE("Cairo: rectangles replace pending paths", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    canvas.context.addPath(square());
    SECTION("fill") {
        canvas.context.fillRect({40, 8, 16, 16});
        REQUIRE(canvas.pixel(48, 16) == 0xff000000u);
    }
    SECTION("stroke") {
        canvas.context.setLineWidth(2);
        canvas.context.strokeRect({40, 8, 16, 16});
        REQUIRE(canvas.pixel(40, 16) == 0xff000000u);
    }
    REQUIRE(canvas.pixel(16, 16) == 0u);
    REQUIRE(canvas.pixel(8, 16) == 0u);
    REQUIRE_FALSE(cairo_has_current_point(canvas.native));
}

TEST_CASE("Cairo: transforms compose in drawing order", "[CairoGraphicContext]") {
    CairoCanvas canvas;
    // Dispatch through GraphicContext to cover the degree-rotation override.
    Grain::GraphicContext& context = canvas.context;
    context.translate(32, 16);
    context.scale(2, 3);
    SECTION("radians") {
        context.rotate(std::numbers::pi / 2);
    }
    SECTION("degrees") {
        context.rotateDegrees(90);
    }
    double x = 4;
    double y = 2;
    cairo_user_to_device(canvas.native, &x, &y);
    REQUIRE_THAT(x, Catch::Matchers::WithinAbs(28, 1e-9));
    REQUIRE_THAT(y, Catch::Matchers::WithinAbs(28, 1e-9));
    context.fillRect({0, 0, 8, 4});
    REQUIRE(canvas.pixel(28, 28) == 0xff000000u);
    REQUIRE(canvas.pixel(34, 28) == 0u);
}

TEST_CASE("Cairo: null contexts are safe no-ops", "[CairoGraphicContext]") {
    CairoGraphicContext context{nullptr};
    context.save();
    context.setFillColor(Grain::Color::white());
    context.setStrokeColor(Grain::Color::black());
    context.setLineWidth(2);
    context.beginPath();
    context.moveTo({0.0, 0.0});
    context.lineTo({8, 8});
    context.curveTo({8.0, 0.0}, {0.0, 8.0}, {8.0, 8.0});
    context.closePath();
    context.fillRect({0, 0, 8, 8});
    context.strokeRect({0, 0, 8, 8});
    context.fillPath(square());
    context.strokePath(square());
    context.translate(1, 2);
    context.scale(2, 3);
    context.rotate(1);
    context.rotateDegrees(90);
    context.restore();
    context.restore();
    SUCCEED();
}
