
#include "../../include/Grain.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace Grain;


//------------------------------------------------------------------------------
//  Configuration
//------------------------------------------------------------------------------

// constexpr const char* input_filename = "demo/Image/615881207_10227239060757332_5280328925510808401_n.jpg";
constexpr const char* input_filename = "/Users/roaldchristesen/Desktop/heartbeats2.jpg";

constexpr const char* output_filename = "demo/Image/poisson-disc-image-density.png";

constexpr double min_radius = 1.0;
constexpr double max_radius = 10.0;

constexpr int32_t max_tries = 20;

constexpr float point_radius = 1.0f;

constexpr size_t max_points = 400000;


//------------------------------------------------------------------------------
//  Copy source image into an RGBA output image
//------------------------------------------------------------------------------

auto copyImage(
    Image& source,
    Image& destination
) -> bool {

    const int32_t width = source.width();
    const int32_t height = source.height();

    if (!destination.create(
        width,
        height,
        4,
        Image::PixelType::UInt8
    )) {
        return false;
    }

    ImageAccess source_access(source);
    ImageAccess destination_access(destination);

    if (!source_access.isUsable() ||
        !destination_access.isUsable()) {
        return false;
    }

    const int32_t components = source_access.componentCount();

    float rgba[4] = {};

    destination_access.setTransferPtr(rgba);

    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {

            if (!source_access.readAt(x, y)) {
                return false;
            }

            const float* pixel = source_access.transferPtr();

            switch (components) {

                case 1:
                    rgba[0] = pixel[0];
                    rgba[1] = pixel[0];
                    rgba[2] = pixel[0];
                    rgba[3] = 1.0f;
                    break;

                case 2:
                    rgba[0] = pixel[0];
                    rgba[1] = pixel[0];
                    rgba[2] = pixel[0];
                    rgba[3] = pixel[1];
                    break;

                case 3:
                    rgba[0] = pixel[0];
                    rgba[1] = pixel[1];
                    rgba[2] = pixel[2];
                    rgba[3] = 1.0f;
                    break;

                case 4:
                    rgba[0] = pixel[0];
                    rgba[1] = pixel[1];
                    rgba[2] = pixel[2];
                    rgba[3] = pixel[3];
                    break;

                default:
                    return false;
            }

            if (!destination_access.writeAt(x, y)) {
                return false;
            }
        }
    }

    return true;
}


//------------------------------------------------------------------------------
//  Draw a filled circle into the output image
//------------------------------------------------------------------------------

auto drawPoint(
    ImageAccess& image_access,
    const Vec2d& position,
    int32_t radius
) -> void {

    const int32_t center_x = static_cast<int32_t>(position.x);
    const int32_t center_y = static_cast<int32_t>(position.y);

    const float color[4] = {0, 0, 0, 1};

    image_access.setTransferPtr(
        const_cast<float*>(color)
    );

    const int32_t radius_squared = radius * radius;

    for (int32_t dy = -radius; dy <= radius; ++dy) {
        for (int32_t dx = -radius; dx <= radius; ++dx) {

            if (dx * dx + dy * dy > radius_squared) {
                continue;
            }

            (void)image_access.writeAt(
                center_x + dx,
                center_y + dy
            );
        }
    }
}


//------------------------------------------------------------------------------
//  Main
//------------------------------------------------------------------------------

int main() {

    // Load source image.
    Image source_image;

    (void)ImageIO::load(
        source_image,
        input_filename
    );

    if (!source_image.isUsable()) {
        std::cerr << "Could not load: " << input_filename << '\n';
        return 1;
    }

    const int32_t width = source_image.width();
    const int32_t height = source_image.height();

    std::cout << "Image: " << width << " x " << height << '\n';


    // Create density from image brightness.
    PoissonDiscImageDensity density(&source_image);


    // Sampling bounds correspond to the image dimensions.
    const Boundsd bounds(
        0.0,
        0.0,
        static_cast<double>(width),
        static_cast<double>(height)
    );


    // Create sampler.
    PoissonDiscSampler sampler(
        bounds,
        density,
        min_radius,
        max_radius,
        max_tries,
        max_points
    );


    // Generate points.
    const std::vector<Vec2d> points = sampler.all();

    std::cout << "Generated points: " << points.size() << '\n';


    // Create output image.
    Image output_image;

    if (!output_image.create(
        source_image.width(),
        source_image.height(),
        4,
        Grain::Image::PixelType::Float32
    )) {
        return 1;
    }


    // Draw generated points.
    ImageAccess output_access(output_image);

    if (!output_access.isUsable()) {
        std::cerr << "Could not access output image.\n";
        return 1;
    }

    for (const Vec2d& point : points) {
        drawPoint(
            output_access,
            point,
            point_radius
        );
    }


    // Save result.
    (void)ImageIO::save(
        output_image,
        output_filename,
        ImageIO::Format::PNG,
        100,
        true
    );

    std::cout << "Saved: " << output_filename << '\n';

    return 0;
}
