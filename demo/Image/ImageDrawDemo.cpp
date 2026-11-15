#include "../../include/Grain.hpp"

#include <memory>
#include <iostream>

using namespace Grain;


//------------------------------------------------------------------------------
//  Configuration
//------------------------------------------------------------------------------

constexpr int32_t image_width = 1600;
constexpr int32_t image_height = 1200;
constexpr int32_t cell_size = 400;


//------------------------------------------------------------------------------
//  Helpers
//------------------------------------------------------------------------------

static auto drawCellFrame(
    GraphicContext& gc,
    int32_t column,
    int32_t row) -> void {

    const double x = column * cell_size;
    const double y = row * cell_size;

    gc.setStrokeColor(Color::fromRGBA32(0xD0D0D0FF));
    gc.setStrokeWidth(1.0);

    gc.beginPath();
    gc.moveTo({x, y});
    gc.lineTo({x + cell_size, y});
    gc.lineTo({x + cell_size, y + cell_size});
    gc.lineTo({x, y + cell_size});
    gc.closePath();
    gc.strokePath();
}


//------------------------------------------------------------------------------
//  1. Line
//------------------------------------------------------------------------------

static auto drawLine(
    GraphicContext& gc,
    double ox,
    double oy) -> void {

    gc.setStrokeColor(Color::fromRGBA32(0xE53935FF));
    gc.setStrokeWidth(8.0);

    gc.beginPath();
    gc.moveTo({ox + 70, oy + 320});
    gc.lineTo({ox + 330, oy + 80});
    gc.strokePath();
}


//------------------------------------------------------------------------------
//  2. Rectangle
//------------------------------------------------------------------------------

static auto drawRectangle(
    GraphicContext& gc,
    double ox,
    double oy) -> void {

    gc.setFillColor(Color::fromRGBA32(0x42A5F5FF));
    gc.setStrokeColor(Color::fromRGBA32(0x1565C0FF));
    gc.setStrokeWidth(6.0);

    gc.beginPath();
    gc.moveTo({ox + 80, oy + 100});
    gc.lineTo({ox + 320, oy + 100});
    gc.lineTo({ox + 320, oy + 300});
    gc.lineTo({ox + 80, oy + 300});
    gc.closePath();

    gc.fillPath();
    gc.strokePath();
}


//------------------------------------------------------------------------------
//  3. Circle
//------------------------------------------------------------------------------

static auto drawCircle(
    GraphicContext& gc,
    double ox,
    double oy) -> void {

    gc.setFillColor(Color::fromRGBA32(0x66BB6AFF));
    gc.setStrokeColor(Color::fromRGBA32(0x2E7D32FF));
    gc.setStrokeWidth(6.0);

    gc.fillCircle(Vec2d(ox + 200, oy + 200), 115.0);
    gc.strokeCircle(Vec2d(ox + 200, oy + 200), 115.0);
}


//------------------------------------------------------------------------------
//  4. Bézier curve
//------------------------------------------------------------------------------

static auto drawBezier(
    GraphicContext& gc,
    double ox,
    double oy) -> void {
    gc.setStrokeColor(Color::fromRGBA32(0x8E24AAFF));
    gc.setStrokeWidth(8.0);

    gc.beginPath();

    gc.moveTo({ox + 60, oy + 300});

    gc.curveTo(
        Vec2d({ox + 80, oy + 40}),
        Vec2d({ox + 320, oy + 40}),
        Vec2d({ox + 340, oy + 300}));

    gc.strokePath();
}


//------------------------------------------------------------------------------
//  5. Graphics path
//------------------------------------------------------------------------------

#include <grain/Graphics/GraphicPath.hpp>

#include <array>
#include <cmath>
#include <numbers>


static auto drawGraphicsPath(
    GraphicContext& gc,
    double ox,
    double oy) -> void {

    GraphicPath path(5);

    const std::array<Vec2d, 5> points = {{
        {ox +  50.0, oy + 270.0},
        {ox + 120.0, oy + 100.0},
        {ox + 200.0, oy + 290.0},
        {ox + 280.0, oy + 120.0},
        {ox + 350.0, oy + 250.0}
    }};

    // Create smooth Bézier handles.
    for (int32_t i = 0; i < 5; ++i) {

        const Vec2d& p = points[i];

        const Vec2d& previous =
            points[i == 0 ? 0 : i - 1];

        const Vec2d& next =
            points[i == 4 ? 4 : i + 1];

        const double dx = next.x - previous.x;
        const double dy = next.y - previous.y;

        const double angle =
            std::atan2(dy, dx) * 180.0 / std::numbers::pi;

        const double distance =
            std::sqrt(dx * dx + dy * dy);

        const double handleLength = distance / 6.0;

        path.addPointByAngleDegrees(
            p,
            angle,
            handleLength,
            handleLength);
    }

    // Draw the path.
    gc.setStrokeColor(Color::fromRGBA32(0xFB8C00FF));
    gc.setStrokeWidth(8.0);

    gc.beginPath();
    gc.addPath(path);
    gc.strokePath();

    // Draw the five anchor points.
    gc.setFillColor(Color::fromRGBA32(0xFB8C00FF));

    for (const auto& point : points) {
        gc.fillCircle(point, 7.0);
    }
}


// ----------------------------------------------------------------------
// 6. Graphics compound path
// ----------------------------------------------------------------------

static auto drawGraphicsCompoundPath(
    GraphicContext& gc,
    double ox,
    double oy) -> void {

    constexpr double pi = std::numbers::pi;

    const double centerX = ox + 200.0;
    const double centerY = oy + 200.0;

    GraphicCompoundPath compound;

    // ------------------------------------------------------------------
    // 1. Outer contour
    // ------------------------------------------------------------------

    compound.addEmptyPath(16);

    GraphicPath* outer = compound.lastPathPtr();

    if (outer == nullptr) {
        return;
    }

    constexpr int32_t outerPointCount = 12;

    for (int32_t i = 0; i < outerPointCount; ++i) {

        const double angle =
            static_cast<double>(i) * 2.0 * pi /
            static_cast<double>(outerPointCount);

        const double radius =
            155.0 *
            (1.0 +
                0.10 * std::sin(angle * 3.0) +
                0.05 * std::cos(angle * 5.0));

        const double x = centerX + std::cos(angle) * radius;
        const double y = centerY + std::sin(angle) * radius;

        outer->addPointByAngleDegrees(
            {x, y},
            angle * 180.0 / pi + 90.0,
            radius * 0.30,
            radius * 0.30);
    }

    outer->setClosed(true);


    // ------------------------------------------------------------------
    // 2. First inner contour
    // ------------------------------------------------------------------

    compound.addEmptyPath(12);

    GraphicPath* inner = compound.lastPathPtr();

    if (inner == nullptr) {
        return;
    }

    constexpr int32_t innerPointCount = 9;

    for (int32_t i = 0; i < innerPointCount; ++i) {

        const double angle =
            static_cast<double>(i) * 2.0 * pi /
            static_cast<double>(innerPointCount);

        const double radius =
            78.0 *
            (1.0 + 0.15 * std::sin(angle * 3.0));

        const double x = centerX + std::cos(angle) * radius;
        const double y = centerY + std::sin(angle) * radius;

        inner->addPointByAngleDegrees(
            {x, y},
            angle * 180.0 / pi + 90.0,
            radius * 0.30,
            radius * 0.30);
    }

    inner->setClosed(true);


    // ------------------------------------------------------------------
    // 3. Center contour
    // ------------------------------------------------------------------

    compound.addEmptyPath(8);

    GraphicPath* center = compound.lastPathPtr();

    if (center == nullptr) {
        return;
    }

    constexpr int32_t centerPointCount = 8;

    for (int32_t i = 0; i < centerPointCount; ++i) {

        const double angle =
            static_cast<double>(i) * 2.0 * pi /
            static_cast<double>(centerPointCount);

        const double radius =
            27.0 *
            (1.0 + 0.15 * std::sin(angle * 2.0));

        const double x = centerX + std::cos(angle) * radius;
        const double y = centerY + std::sin(angle) * radius;

        center->addPointByAngleDegrees(
            {x, y},
            angle * 180.0 / pi + 90.0,
            radius * 0.35,
            radius * 0.35);
    }

    center->setClosed(true);


    // ------------------------------------------------------------------
    // 4. Gradient
    // ------------------------------------------------------------------

    Gradient gradient;

    gradient.addStop(0.0, {1.0, 0.05, 0.25, 1.0});
    gradient.addStop(0.3, {1.0, 0.65, 0.05, 1.0});
    gradient.addStop(0.6, {0.05, 0.55, 1.0, 1.0});
    gradient.addStop(1.0, {0.55, 0.1, 0.9, 1.0});


    // ------------------------------------------------------------------
    // 5. Fill compound path using even-odd clipping
    // ------------------------------------------------------------------

    gc.save();

    gc.beginPath();
    gc.addPath(compound);

    gc.clipPathEvenOdd();

    gc.fillGradient(
        &gradient,
        {centerX - 180.0, centerY - 180.0},
        {centerX + 180.0, centerY + 180.0},
        true,
        true);

    gc.restore();


    // ------------------------------------------------------------------
    // 6. Draw compound path outlines
    // ------------------------------------------------------------------

    gc.save();

    gc.beginPath();
    gc.addPath(compound);

    gc.setStrokeColor({0.0, 0.3, 0.3, 1.0});
    gc.setStrokeWidth(3.0);

    gc.strokePath();

    gc.restore();
}


//------------------------------------------------------------------------------
//  7. Catmull-Rom curve
//------------------------------------------------------------------------------

#include <random>

static auto drawCatmullRomCurve(
    GraphicContext& gc,
    double ox,
    double oy) -> void {

    CatmullRomCurve curve;

    curve.setAlpha(0.5);

    // Reproducible random values.
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> xOffset(-25.0, 25.0);
    std::uniform_real_distribution<double> yOffset(-65.0, 65.0);

    constexpr int32_t pointCount = 10;

    // Generate ten control points.
    for (int32_t i = 0; i < pointCount; ++i) {

        const double x = ox + 25.0 + i * 38.0 + xOffset(rng);
        const double y = oy + 200.0 + yOffset(rng);

        curve.addPoint(Vec2d(x, y));
    }

    // Draw the curve.
    gc.setStrokeColor(Color::fromRGBA32(0x3949ABFF));
    gc.setStrokeWidth(7.0);

    gc.beginPath();

    Vec2d point;

    if (curve.pointOnCurve(0.0, point)) {

        gc.moveTo(point);

        constexpr int32_t steps = 200;

        for (int32_t i = 1; i <= steps; ++i) {

            const double t =
                static_cast<double>(i) / steps;

            if (curve.pointOnCurve(t, point)) {
                gc.lineTo(point);
            }
        }

        gc.strokePath();
    }

    // Draw the control points.
    gc.setFillColor(Color::fromRGBA32(0x3949ABFF));

    for (int32_t i = 0; i < curve.pointCount(); ++i) {

        if (curve.pointAtIndex(i, point)) {
            gc.fillCircle(point, 7.0);
        }
    }
}


//------------------------------------------------------------------------------
//  Superellipses
//------------------------------------------------------------------------------

static auto drawSuperellipses(
    GraphicContext& gc,
    double ox,
    double oy) -> void {

    gc.save();
    gc.translate(ox + 200, oy + 200);

    constexpr int32_t steps = 120;

    constexpr double radius_x = 145.0;
    constexpr double radius_y = 125.0;

    Superellipse superellipse(radius_x, radius_y, 3.5);

    // -------------------------------------------------------------------------
    // First superellipse: filled, forward direction.
    // -------------------------------------------------------------------------

    GraphicPath filled_path = superellipse.toGraphicPath(12);
    gc.setFillColor(Color::fromRGBA32(0x3949ABFF));
    gc.beginPath();
    gc.addPath(filled_path);
    gc.fillPath();

    // -------------------------------------------------------------------------
    // Second superellipse: stroked, reverse direction.
    // -------------------------------------------------------------------------

    superellipse.setExponent(0.8);

    GraphicPath stroked_path = superellipse.toGraphicPath(12);

    gc.setStrokeColor(Color::fromRGBA32(0xFB8C00FF));
    gc.setStrokeWidth(7.0);
    gc.beginPath();
    gc.addPath(stroked_path);
    gc.strokePath();

    gc.restore();
}


//------------------------------------------------------------------------------
//  TextWordLayout
//------------------------------------------------------------------------------

static auto drawTextWordLayout(
    GraphicContext& gc,
    const Font& font,
    double ox,
    double oy
) -> void {

    const char* text =
        "TextWordLayout calculates the position and dimensions "
        "of individual words and arranges them into lines "
        "that fit within a given width.";

    TextWordLayout layout(text, &font);
    layout.layout(340.0, 250.0);

    // Cell title
    /*
    gc.setFillColor(Color::fromRGBA32(0x202020FF));
    gc.drawText(
        "Text Word Layout",
        {ox + 24.0, oy + 32.0},
        &font,
        Color::fromRGBA32(0x202020FF)
    );
    */

    // Layout boundary
    gc.setStrokeColor(Color::fromRGBA32(0xCCCCCCFF));
    gc.setStrokeWidth(1.0);
    gc.beginPath();
    gc.moveTo({ox + 24.0, oy + 65.0});
    gc.lineTo({ox + 376.0, oy + 65.0});
    gc.lineTo({ox + 376.0, oy + 350.0});
    gc.lineTo({ox + 24.0, oy + 350.0});
    gc.closePath();
    gc.strokePath();

    // Draw individual words at their calculated positions
    for (int32_t i = 0; i < layout.wordCount(); ++i) {

        const auto* word = layout.word(i);

        if (word == nullptr) {
            continue;
        }

        gc.drawText(
            word->text.c_str(),
            {ox + 30.0 + word->x, oy + 75.0 + word->y},
            &font,
            Color::fromRGBA32(0x303030FF)
        );
    }
}


//------------------------------------------------------------------------------
//  Image
//------------------------------------------------------------------------------

auto drawImage(
    GraphicContext& gc,
    const Image& image,
    double ox,
    double oy
) -> void {

    if (!image.isUsable()) {
        return;
    }

    constexpr double margin = 20.0;
    constexpr double top = 60.0;

    const double availableWidth = cell_size - 2.0 * margin;
    const double availableHeight = cell_size - top - margin;

    // Preserve the original aspect ratio.
    const double scale = std::min(
        availableWidth / image.width(),
        availableHeight / image.height()
    );

    const double width = image.width() * scale;
    const double height = image.height() * scale;

    // Center the image inside the cell.
    const double x = ox + (cell_size - width) * 0.5;
    const double y = oy + top + (availableHeight - height) * 0.5;

    gc.drawImage(
        image,
        Rectd(x, y, width, height),
        1.0f
    );
}


//------------------------------------------------------------------------------
//  Main
//------------------------------------------------------------------------------

auto main() -> int {

    auto font = new Font("SFPro-Regular", 30.0f);

    // Load demo image.
    Image demo_image;
    const auto load_result = ImageIO::load(
        demo_image,
        "/Users/roaldchristesen/Documents/Developer/Projects/CLionProjects/grain/demo/Image/byte-magazine-cover.jpg"
    );

    if (load_result != ImageIO::Error::None) {
        std::cout << "image byte-magazine-cover.jpg not found" << std::endl;
        return 1;
    }

    // Create image.

    Image image;

    if (!image.create(
        image_width,
        image_height,
        4,
        Image::PixelType::Float32)) {

        return 1;
    }

    // Create graphics context.

    auto gc = GraphicContext::createGraphicContext(image);

    if (!gc) {
        return 1;
    }

    // Background.

    gc->setFillColor(Color::white());
    gc->fillRect(Rectd(0.0, 0.0, image_width, image_height));

    // Cell frames.

    for (int32_t row = 0; row < 3; ++row) {
        for (int32_t column = 0; column < 4; ++column) {
            drawCellFrame(*gc, column, row);
        }
    }

    // First row.

    drawLine(*gc, 0, 0);
    drawRectangle(*gc, 400, 0);
    drawCircle(*gc, 800, 0);
    drawBezier(*gc, 1200, 0);

    // Second row.

    drawGraphicsPath(*gc, 0, 400);
    drawGraphicsCompoundPath(*gc, 400, 400);
    drawCatmullRomCurve(*gc, 800, 400);
    drawSuperellipses(*gc, 1200, 400);

    // Third row.

    drawTextWordLayout(*gc, font, 0, 800);
    drawImage(*gc, demo_image, 400, 800);


    // Save image.

    const auto error = ImageIO::save(
        image,
        "catmull-rom-demo.png",
        ImageIO::Format::PNG,
        100,
        true);

    if (error == ImageIO::Error::None) {
        return 1;
    }

    std::cout << "Done" << std::endl;

    return 0;
}