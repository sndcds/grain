#include <cstdint>
#include <cstdio>

#include "../../include/Grain.hpp"

auto main() -> int {
    constexpr uint32_t width = 1200;
    constexpr uint32_t height = 800;

    Grain::Image image;


    //--------------------------------------------------------------------------
    //  Create an 8-bit RGB image
    //--------------------------------------------------------------------------

    if (!image.create(
        width,
        height,
        4,
        Grain::Image::PixelType::Float32
    )) {
        std::fprintf(stderr, "Failed to create image.\n");
        return 1;
    }


    //--------------------------------------------------------------------------
    //  Set up pixel access
    //--------------------------------------------------------------------------

    Grain::ImageAccess access;

    if (!access.setup(image)) {
        std::fprintf(stderr, "Failed to set up ImageAccess.\n");
        return 1;
    }

    //--------------------------------------------------------------------------
    //  Fill every RGB channel
    //--------------------------------------------------------------------------

    if (true) {
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

                values[0] = 0.001 * y;
                values[1] = 0.001 * access.x();
                values[2] = 0;
                values[3] = 0.25;

                if (!access.write()) {
                    std::fprintf(stderr, "Failed to write pixel.\n");
                    return 1;
                }
            } while (access.nextX());
        }
    }

    //--------------------------------------------------------------------------
    //  Draw
    //--------------------------------------------------------------------------

    auto gc = Grain::GraphicContext::createGraphicContext(image);
    if (gc != nullptr) {
        gc->setFillColor({1.0f, 1.0f, 0.0f, 1.0f});
        gc->fillCircle({200, 200}, 100);

        for (int i = 0; i < 10; ++i) {
            gc->setFillColor({0.0f, 0.0f, 1.0f, 0.1f});
            gc->fillRect({10, 10, 900, 600});
            gc->translate(20, 0);
            gc->rotateDegrees(10);
        }
    }

    //--------------------------------------------------------------------------
    //  Invert every RGB channel
    //--------------------------------------------------------------------------

    if (false) {
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
                values[1] = 1.0f - values[1];
                values[2] = 1.0f - values[2];

                if (!access.write()) {
                    std::fprintf(stderr, "Failed to write pixel.\n");
                    return 1;
                }
            } while (access.nextX());
        }
    }


    //--------------------------------------------------------------------------
    //  Save the image in different formats
    //--------------------------------------------------------------------------

    struct Output {
        const char* filename;
        Grain::ImageIO::Format format;
        const char* name;
    };

    constexpr Output outputs[] = {
        {
            "gradient-inverted.jpg",
            Grain::ImageIO::Format::JPEG,
            "JPEG"
        },
        {
            "gradient-inverted.png",
            Grain::ImageIO::Format::PNG,
            "PNG"
        },
        {
            "gradient-inverted.webp",
            Grain::ImageIO::Format::WebP,
            "WebP"
        },
    };

    for (const auto& output : outputs) {
        const auto error = Grain::ImageIO::save(
            image,
            output.filename,
            output.format,
            100,
            true
        );

        if (error != Grain::ImageIO::Error::None) {
            std::fprintf(
                stderr,
                "Failed to save %s (error code: %d).\n",
                output.name,
                static_cast<int>(error)
            );

            return 1;
        }

        std::printf(
            "Saved %s (%u x %u, RGB).\n",
            output.filename,
            width,
            height
        );
    }


    //--------------------------------------------------------------------------
    //  Print sampled colors
    //--------------------------------------------------------------------------

    std::printf("Sampled colors (x, y, r, g, b):\n");

    for (uint32_t y = 0; y < height; y += 100) {
        for (uint32_t x = 0; x < width; x += 100) {
            const float red = static_cast<float>(image.pixel<uint8_t>(x, y, 0)) / 255.0f;
            const float green = static_cast<float>(image.pixel<uint8_t>(x, y, 1)) / 255.0f;
            const float blue = static_cast<float>(image.pixel<uint8_t>(x, y, 2)) / 255.0f;
            const Grain::Color color {red, green, blue, 1.0f};

            std::printf("(%3u, %3u, %.3f, %.3f, %.3f) ", x, y, color.red, color.green, color.blue);
        }

        std::printf("\n");
    }

    return 0;
}