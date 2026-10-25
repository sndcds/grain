#include <cstdint>
#include <cstdio>

#include <grain/Graphics/Color.hpp>
#include <grain/Image/Image.hpp>
#include <grain/Image/ImageAccess.hpp>

auto main() -> int {
    constexpr uint32_t width = 800;
    constexpr uint32_t height = 600;

    Grain::Image image;

    // Create an 8-bit grayscale image.
    if (!image.create(
        width,
        height,
        1,
        Grain::Image::PixelType::UInt8
    )) {
        std::fprintf(stderr, "Failed to create image.\n");
        return 1;
    }

    // Fill with a horizontal gradient: 0 to 255.
    for (uint32_t y = 0; y < height; ++y) {
        auto* row = image.rowData(y);

        for (uint32_t x = 0; x < width; ++x) {
            row[x] = static_cast<uint8_t>(
                x * 255 / (width - 1)
            );
        }
    }

    // Set up pixel access.
    Grain::ImageAccess access;

    if (!access.setup(image)) {
        std::fprintf(stderr, "Failed to set up ImageAccess.\n");
        return 1;
    }

    std::printf("Image: %u x %u\n", width, height);
    std::printf("Sampled colors (x, y, r, g, b, a):\n");

    for (uint32_t y = 0; y < height; y += 100) {
        for (uint32_t x = 0; x < width; x += 100) {
            const float gray =
                static_cast<float>(image.pixel<uint8_t>(x, y, 0))
                / 255.0f;

            const Grain::Color color {
                gray,
                gray,
                gray,
                1.0f
            };

            std::printf(
                "(%3u, %3u, %.3f, %.3f, %.3f, %.1f) ",
                x, y,
                color.red,
                color.green,
                color.blue,
                color.alpha
            );
        }

        std::printf("\n");
    }

    return 0;
}