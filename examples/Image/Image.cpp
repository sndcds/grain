#include <cstdint>
#include <cstdio>

#include <grain/Graphics/Color.hpp>
#include <grain/Image/Image.hpp>
#include <grain/Image/ImageAccess.hpp>
#include <grain/Image/ImageIO.hpp>

auto main() -> int {
    constexpr uint32_t width = 800;
    constexpr uint32_t height = 600;

    Grain::Image image;


    //--------------------------------------------------------------------------
    //  Create an 8-bit grayscale image.
    //--------------------------------------------------------------------------

    if (!image.create(
        width,
        height,
        1,
        Grain::Image::PixelType::UInt8
    )) {
        std::fprintf(stderr, "Failed to create image.\n");
        return 1;
    }


    //--------------------------------------------------------------------------
    //  Fill with a horizontal gradient: 0 to 255.
    //--------------------------------------------------------------------------

    for (uint32_t y = 0; y < height; ++y) {
        auto* row = image.rowData(y);

        for (uint32_t x = 0; x < width; ++x) {
            row[x] = static_cast<uint8_t>(
                x * 255 / (width - 1)
            );
        }
    }


    //--------------------------------------------------------------------------
    //  Set up pixel access.
    //--------------------------------------------------------------------------

    Grain::ImageAccess access;

    if (!access.setup(image)) {
        std::fprintf(stderr, "Failed to set up ImageAccess.\n");
        return 1;
    }


    //--------------------------------------------------------------------------
    //  Invert every pixel.
    //--------------------------------------------------------------------------

    for (int32_t y = 0; y < static_cast<int32_t>(height); ++y) {
        if (!access.beginRow(y)) {
            std::fprintf(stderr, "Failed to begin row.\n");
            return 1;
        }

        do {
            if (!access.read()) {
                std::fprintf(stderr, "Failed to read pixel.\n");
                return 1;
            }

            float* values = access.transferPtr();
            values[0] = 1.0f - values[0];

            if (!access.write()) {
                std::fprintf(stderr, "Failed to write pixel.\n");
                return 1;
            }
        } while (access.nextX());
    }


    //--------------------------------------------------------------------------
    //  Save as PNG.
    //--------------------------------------------------------------------------

    const auto error = Grain::ImageIO::save(
        image,
        "gradient-inverted.png",
        Grain::ImageIO::Format::PNG
    );

    if (error != Grain::ImageIO::Error::None) {
        std::fprintf(
            stderr,
            "Failed to save PNG (error code: %d).\n",
            static_cast<int>(error)
        );
        return 1;
    }

    std::printf(
        "Saved gradient-inverted.png (%u x %u).\n",
        width,
        height
    );

    //--------------------------------------------------------------------------
    //  Print sampled colors.
    //--------------------------------------------------------------------------

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