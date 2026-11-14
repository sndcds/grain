
#include <algorithm>
#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <grain/Color/Color.hpp>
#include <grain/Color/HSVColor.hpp>
#include <grain/Image/Image.hpp>
#include <grain/Image/ImageIO.hpp>


auto fromHex(
    std::uint32_t hex
) noexcept -> Grain::HSVColor {

    Grain::Color color{};

    color.red   = static_cast<float>((hex >> 16) & 0xFF) / 255.0f;
    color.green = static_cast<float>((hex >>  8) & 0xFF) / 255.0f;
    color.blue  = static_cast<float>(hex & 0xFF) / 255.0f;
    color.alpha = 1.0f;

    return Grain::HSVColor::fromColor(color);
}


auto toHex(
    const Grain::Color& color
) -> std::string {

    const std::uint32_t rgba = color.rgba32();

    const auto red   = (rgba >> 24) & 0xFF;
    const auto green = (rgba >> 16) & 0xFF;
    const auto blue  = (rgba >>  8) & 0xFF;

    std::ostringstream stream;

    stream << '#'
           << std::uppercase
           << std::hex
           << std::setfill('0')
           << std::setw(2) << red
           << std::setw(2) << green
           << std::setw(2) << blue;

    return stream.str();
}


auto fillRect(
    Grain::Image& image,
    int x,
    int y,
    int width,
    int height,
    const Grain::Color& color
) noexcept -> void {

    const auto red = static_cast<std::uint8_t>(
        std::clamp(color.red, 0.0f, 1.0f) * 255.0f + 0.5f
    );

    const auto green = static_cast<std::uint8_t>(
        std::clamp(color.green, 0.0f, 1.0f) * 255.0f + 0.5f
    );

    const auto blue = static_cast<std::uint8_t>(
        std::clamp(color.blue, 0.0f, 1.0f) * 255.0f + 0.5f
    );

    const auto alpha = static_cast<std::uint8_t>(
        std::clamp(color.alpha, 0.0f, 1.0f) * 255.0f + 0.5f
    );

    for (int py = y; py < y + height; ++py) {

        auto* row = image.rowData(py);

        for (int px = x; px < x + width; ++px) {

            const auto offset = static_cast<std::size_t>(px) * 4;

            row[offset + 0] = red;
            row[offset + 1] = green;
            row[offset + 2] = blue;
            row[offset + 3] = alpha;
        }
    }
}


auto main() -> int {

    //--------------------------------------------------------------------------
    //  Reference colors
    //--------------------------------------------------------------------------

    constexpr std::size_t colorCount = 7;

    std::array<Grain::HSVColor, colorCount> colors{};

    // Color 1: #F6F8FC
    colors[0] = fromHex(0xF6F8FC);

    // Color 5: #1A73E8
    colors[4] = fromHex(0x1A73E8);

    // Color 7: #003273
    colors[6] = fromHex(0x003273);


    //--------------------------------------------------------------------------
    //  Generate intermediate colors
    //--------------------------------------------------------------------------

    // Colors 1 -> 5: generate colors 2, 3 and 4.

    for (std::size_t i = 1; i < 4; ++i) {

        const float t =
            static_cast<float>(i) / 4.0f;

        colors[i] = Grain::HSVColor::lerp(
            colors[0],
            colors[4],
            t
        );
    }

    // Colors 5 -> 7: generate color 6.

    colors[5] = Grain::HSVColor::lerp(
        colors[4],
        colors[6],
        0.5f
    );


    //--------------------------------------------------------------------------
    //  Hover blend percentages
    //--------------------------------------------------------------------------

    constexpr float hover3Blend = 0.5f;
    constexpr float hover5Blend = 0.5f;
    constexpr float hover7Blend = 0.5f;
    constexpr float chip_blend = 0.2f;

    // Dark reference color.
    const Grain::HSVColor dark = fromHex(0x001A40);


    //--------------------------------------------------------------------------
    //  Calculate hover colors
    //--------------------------------------------------------------------------

    const Grain::HSVColor hover3 = Grain::HSVColor::lerp(
        colors[2],
        colors[3],
        hover3Blend
    );

    const Grain::HSVColor hover5 = Grain::HSVColor::lerp(
        colors[4],
        colors[5],
        hover5Blend
    );

    const Grain::HSVColor hover7 = Grain::HSVColor::lerp(
        colors[6],
        dark,
        hover7Blend
    );

    const Grain::HSVColor chip_bg = Grain::HSVColor::lerp(
        colors[0],
        colors[2],
        chip_blend
    );


    //--------------------------------------------------------------------------
    //  Output RGB hex values
    //--------------------------------------------------------------------------

    for (std::size_t i = 0; i < colors.size(); ++i) {

        std::cout
            << "--uw-color-" << i + 1 << ": "
            << toHex(colors[i].toColor())
            << ";\n";
    }

    std::cout
        << "--uw-color-hover-3: " << toHex(hover3.toColor()) << ";\n"
        << "--uw-color-hover-5: " << toHex(hover5.toColor()) << ";\n"
        << "--uw-color-hover-7: " << toHex(hover7.toColor()) << ";\n";

    std::cout << "--uw-color-chip-bg: " << toHex(chip_bg.toColor()) << ";\n";


    //--------------------------------------------------------------------------
    //  Image
    //--------------------------------------------------------------------------

    constexpr int imageWidth = 800;
    constexpr int imageHeight = 800;
    constexpr int columnWidth = imageWidth / 2;

    Grain::Image image;

    image.create(
        imageWidth,
        imageHeight,
        4,
        Grain::Image::PixelType::UInt8
    );


    //--------------------------------------------------------------------------
    //  Left column: seven reference colors
    //--------------------------------------------------------------------------

    for (std::size_t i = 0; i < colors.size(); ++i) {

        const int y0 = static_cast<int>(
            i * static_cast<std::size_t>(imageHeight) /
            colors.size()
        );

        const int y1 = static_cast<int>(
            (i + 1) * static_cast<std::size_t>(imageHeight) /
            colors.size()
        );

        fillRect(
            image,
            0,
            y0,
            columnWidth,
            y1 - y0,
            colors[i].toColor()
        );
    }

    fillRect(
        image,
        0,
        0,
        columnWidth / 3,
        20,
        chip_bg.toColor()
    );


    //--------------------------------------------------------------------------
    //  Right column: hover colors
    //--------------------------------------------------------------------------

    const std::array<Grain::HSVColor, 3> hoverColors = {
        hover3,
        hover5,
        hover7
    };

    constexpr std::array<std::size_t, 3> hoverIndices = {
        2,  // Color 3
        4,  // Color 5
        6   // Color 7
    };

    for (std::size_t i = 0; i < hoverColors.size(); ++i) {

        const std::size_t colorIndex = hoverIndices[i];

        const int y0 = static_cast<int>(
            colorIndex * static_cast<std::size_t>(imageHeight) /
            colors.size()
        );

        const int y1 = static_cast<int>(
            (colorIndex + 1) * static_cast<std::size_t>(imageHeight) /
            colors.size()
        );

        fillRect(
            image,
            columnWidth,
            y0,
            columnWidth,
            y1 - y0,
            hoverColors[i].toColor()
        );
    }


    //--------------------------------------------------------------------------
    //  Save PNG
    //--------------------------------------------------------------------------

    const auto result = Grain::ImageIO::save(
        image,
        "color_for_ui.png",
        Grain::ImageIO::Format::PNG
    );

    if (result != Grain::ImageIO::Error::None) {

        std::cerr << "Could not save color_for_ui.png\n";
    }
    else {

        std::cout << "Saved color_for_ui.png\n";
    }

    return 0;
}
