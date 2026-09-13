#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../../src/Platform/LinuxGTK4/Gtk4GraphicContext.hpp"

#include <array>
#include <cstdint>
#include <numbers>

namespace {

using Grain::Platform::Gtk4GraphicContext;

struct TestRenderer {
    GdkSurface* surface;
    GskRenderer* renderer;

    TestRenderer() {
        gtk_init();
        surface = gdk_surface_new_toplevel(gdk_display_get_default());
        renderer = gsk_renderer_new_for_surface(surface);
        REQUIRE(renderer != nullptr);
    }
    ~TestRenderer() {
        gsk_renderer_unrealize(renderer);
        g_object_unref(renderer);
        gdk_surface_destroy(surface);
        g_object_unref(surface);
    }
};

// Verify that Grain submits native paths, including when GTK selects its
// software renderer in headless CI.
void checkNativeNodes(GskRenderNode* node, unsigned& fills, unsigned& strokes) {
    switch (gsk_render_node_get_node_type(node)) {
    case GSK_FILL_NODE:
        ++fills;
        checkNativeNodes(gsk_fill_node_get_child(node), fills, strokes);
        break;
    case GSK_STROKE_NODE:
        ++strokes;
        checkNativeNodes(gsk_stroke_node_get_child(node), fills, strokes);
        break;
    case GSK_COLOR_NODE:
        break;
    case GSK_CONTAINER_NODE:
        for (guint i = 0; i < gsk_container_node_get_n_children(node); ++i) {
            checkNativeNodes(gsk_container_node_get_child(node, i), fills, strokes);
        }
        break;
    case GSK_TRANSFORM_NODE:
        checkNativeNodes(gsk_transform_node_get_child(node), fills, strokes);
        break;
    default:
        FAIL("Unexpected render node type: " << gsk_render_node_get_node_type(node));
    }
}

struct NativeCanvas {
    GtkSnapshot* snapshot = gtk_snapshot_new();
    Gtk4GraphicContext context{snapshot};
    std::array<std::uint32_t, 64 * 64> pixels{};
    unsigned fills = 0;
    unsigned strokes = 0;

    ~NativeCanvas() {
        if (snapshot != nullptr) {
            g_object_unref(snapshot);
        }
    }

    void finish() {
        if (snapshot == nullptr) {
            return;
        }
        g_autoptr(GskRenderNode) node = gtk_snapshot_free_to_node(snapshot);
        snapshot = nullptr;
        if (node == nullptr) {
            return;
        }
        checkNativeNodes(node, fills, strokes);
        static TestRenderer renderer;
        const graphene_rect_t viewport = {{0, 0}, {64, 64}};
        g_autoptr(GdkTexture) texture = gsk_renderer_render_texture(renderer.renderer, node, &viewport);
        REQUIRE(texture != nullptr);
        gdk_texture_download(texture, reinterpret_cast<guchar*>(pixels.data()), 64 * 4);
    }

    std::uint32_t pixel(int x, int y) {
        finish();
        return pixels[static_cast<std::size_t>(y * 64 + x)];
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

TEST_CASE("GTK4: fill and stroke colors are independent", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    context.setFillColor({1, 0, 0});
    context.setStrokeColor({0, 0, 1});
    context.setLineWidth(4);
    context.fillRect({8, 8, 16, 16});
    context.strokeRect({8, 8, 16, 16});

    context.setFillColor({0, 1, 0});
    context.strokeRect({36, 8, 16, 16});
    context.fillRect({40, 12, 8, 8});
    REQUIRE(canvas.pixel(16, 16) == 0xffff0000u);
    REQUIRE(canvas.pixel(8, 16) == 0xff0000ffu);
    REQUIRE(canvas.pixel(4, 16) == 0u);
    REQUIRE(canvas.pixel(36, 16) == 0xff0000ffu);
    REQUIRE(canvas.pixel(44, 16) == 0xff00ff00u);
}

TEST_CASE("GTK4: default colors and straight alpha render correctly", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    context.fillRect({0, 0, 8, 8});
    context.setLineWidth(2);
    context.strokeRect({16, 2, 8, 8});

    context.setFillColor({1, 0, 0, 0.5f});
    context.fillRect({32, 0, 8, 8});
    context.setStrokeColor({0, 0, 1, 0.5f});
    context.strokeRect({48, 2, 8, 8});
    REQUIRE(canvas.pixel(4, 4) == 0xff000000u);
    REQUIRE(canvas.pixel(16, 4) == 0xff000000u);
    REQUIRE(canvas.pixel(36, 4) == 0x80800000u);
    REQUIRE(canvas.pixel(48, 4) == 0x80000080u);
}

TEST_CASE("GTK4: nested saves restore colors and native state", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
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
    context.fillRect({8, 8, 16, 16});
    context.strokeRect({8, 8, 16, 16});

    context.restore();
    context.fillRect({8, 8, 16, 16});
    context.strokeRect({8, 8, 16, 16});
    REQUIRE(canvas.pixel(48, 16) == 0xff00ff00u);
    REQUIRE(canvas.pixel(40, 16) == 0xffffff00u);
    REQUIRE(canvas.pixel(16, 16) == 0xffff0000u);
    REQUIRE(canvas.pixel(7, 16) == 0xff0000ffu);

    // An unmatched restore is harmless.
    context.restore();
}

TEST_CASE("GTK4: GraphicPath can be filled then stroked", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    const auto path = square();
    context.setFillColor({1, 0, 0});
    context.setStrokeColor({0, 0, 1});
    context.setLineWidth(4);
    context.fillPath(path);
    context.strokePath(path);
    context.fillPath(Grain::GraphicPath{});
    context.strokePath(Grain::GraphicPath{});
    REQUIRE(canvas.pixel(16, 16) == 0xffff0000u);
    REQUIRE(canvas.pixel(8, 16) == 0xff0000ffu);
    REQUIRE(canvas.pixel(16, 8) == 0xff0000ffu);
    REQUIRE(canvas.pixel(4, 16) == 0u);
    REQUIRE(canvas.fills == 1);
    REQUIRE(canvas.strokes == 1);
}

TEST_CASE("GTK4: open paths retain their endpoints when stroked", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
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

TEST_CASE("GTK4: Bezier handles render curved path segments", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
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

TEST_CASE("GTK4: empty and single-point paths draw nothing", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    Grain::GraphicPath path;
    SECTION("empty") {}
    SECTION("single point") {
        path.addPoint(8, 8);
    }
    canvas.context.fillPath(path);
    canvas.context.strokePath(path);
    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {
            REQUIRE(canvas.pixel(x, y) == 0u);
        }
    }
}

TEST_CASE("GTK4: manually constructed paths append and are consumed", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    context.moveTo({48, 48});
    context.lineTo({60, 60});
    context.beginPath();
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
}

TEST_CASE("GTK4: rectangles replace pending paths", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
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
}

TEST_CASE("GTK4: transforms compose in drawing order", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    // Dispatch through GraphicContext to cover the degree-rotation override.
    Grain::GraphicContext& context = canvas.context;
    context.translate(32, 16);
    context.scale(2, 3);
    const bool degrees = GENERATE(false, true);
    CAPTURE(degrees);
    if (degrees) {
        context.rotateDegrees(90);
    } else {
        context.rotate(std::numbers::pi / 2);
    }
    SECTION("fill") {
        context.fillRect({0, 0, 8, 4});
        REQUIRE(canvas.pixel(28, 28) == 0xff000000u);
        REQUIRE(canvas.pixel(34, 28) == 0u);
    }
    SECTION("stroke") {
        context.setLineWidth(2);
        context.strokeRect({0, 0, 8, 4});
        REQUIRE(canvas.pixel(23, 28) == 0xff000000u);
        REQUIRE(canvas.pixel(33, 28) == 0xff000000u);
        REQUIRE(canvas.pixel(28, 28) == 0u);
        REQUIRE(canvas.pixel(35, 28) == 0u);
    }
}

TEST_CASE("GTK4: paths retain construction transforms across restore", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    context.setLineWidth(2);
    context.save();
    context.translate(24, 0);
    context.scale(2, 1);
    context.moveTo({4, 8});
    context.lineTo({12, 8});
    context.restore();
    context.lineTo({48, 24});
    context.strokePath(Grain::GraphicPath{});
    REQUIRE(canvas.pixel(40, 8) == 0xff000000u);
    REQUIRE(canvas.pixel(48, 16) == 0xff000000u);
    REQUIRE(canvas.pixel(8, 8) == 0u);
    REQUIRE(canvas.pixel(40, 10) == 0u);
}

TEST_CASE("GTK4: nonuniform scaling transforms the stroke width", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    context.translate(8, 8);
    context.scale(3, 2);
    context.setLineWidth(4);
    context.strokeRect({4, 4, 8, 12});
    REQUIRE(canvas.pixel(15, 28) == 0xff000000u);
    REQUIRE(canvas.pixel(13, 28) == 0u);
    REQUIRE(canvas.pixel(32, 13) == 0xff000000u);
    REQUIRE(canvas.pixel(32, 11) == 0u);
    REQUIRE(canvas.pixel(32, 28) == 0u);
}

TEST_CASE("GTK4: strokes apply the transform active at paint time", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    context.moveTo({8, 24});
    context.lineTo({40, 24});
    context.setLineWidth(2);
    context.scale(2, 3);
    context.strokePath(Grain::GraphicPath{});
    REQUIRE(canvas.pixel(24, 21) == 0xff000000u);
    REQUIRE(canvas.pixel(24, 26) == 0xff000000u);
    REQUIRE(canvas.pixel(24, 20) == 0u);
    REQUIRE(canvas.pixel(44, 24) == 0u);
}

TEST_CASE("GTK4: degenerate strokes do not poison later drawing", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    context.save();
    SECTION("singular transform") { context.scale(0, 1); }
    SECTION("zero width") { context.setLineWidth(0); }
    context.strokePath(square());
    context.restore();
    context.strokePath(Grain::GraphicPath{});
    context.fillRect({40, 8, 8, 8});
    REQUIRE(canvas.pixel(44, 12) == 0xff000000u);
    REQUIRE(canvas.pixel(8, 16) == 0u);
    REQUIRE(canvas.strokes == 0);
}

TEST_CASE("GTK4: rotated nonuniform pens and reflections retain their width", "[Gtk4GraphicContext]") {
    NativeCanvas canvas;
    auto& context = canvas.context;
    context.moveTo({32, 8});
    context.lineTo({32, 56});
    context.rotateDegrees(45);
    const double direction = GENERATE(-1.0, 1.0);
    CAPTURE(direction);
    context.scale(3 * direction, 1);
    context.setLineWidth(4);
    context.strokePath(Grain::GraphicPath{});
    REQUIRE(canvas.pixel(28, 32) == 0xff000000u);
    REQUIRE(canvas.pixel(35, 32) == 0xff000000u);
    REQUIRE(canvas.pixel(26, 32) == 0u);
    REQUIRE(canvas.pixel(37, 32) == 0u);
}
