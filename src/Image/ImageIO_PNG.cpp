
#include "ImageIO_PNG.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <limits>

#include <png.h>
#include <vector>

namespace Grain::Detail {

namespace {

[[nodiscard]] auto validPNGStride(size_t stride) noexcept -> bool {
    return stride <= static_cast<size_t>(
        std::numeric_limits<png_int_32>::max()
    );
}

} // namespace

auto loadPNG(
    Image& image,
    const char* filename
) noexcept -> ImageIO::Error {
    if (filename == nullptr || filename[0] == '\0') {
        return ImageIO::Error::InvalidArgument;
    }

    png_image png{};
    png.version = PNG_IMAGE_VERSION;
    png.opaque = nullptr;

    if (png_image_begin_read_from_file(&png, filename) == 0) {
        png_image_free(&png);
        return ImageIO::Error::DecodeError;
    }

    // Normalize all PNG inputs to 8-bit RGBA.
    png.format = PNG_FORMAT_RGBA;

    Image decoded;

    if (!decoded.create(
        png.width,
        png.height,
        4,
        Image::PixelType::UInt8
    )) {
        png_image_free(&png);
        return ImageIO::Error::OutOfMemory;
    }

    const size_t stride = decoded.bytesPerRow();

    if (!validPNGStride(stride)) {
        png_image_free(&png);
        return ImageIO::Error::UnsupportedPixelType;
    }

    const int success = png_image_finish_read(
        &png,
        nullptr,
        decoded.mutPixelData(),
        static_cast<png_int_32>(stride),
        nullptr
    );

    // Safe after finish_read as well: releases any remaining libpng state.
    png_image_free(&png);

    if (success == 0) {
        return ImageIO::Error::DecodeError;
    }

    image = static_cast<Image&&>(decoded);
    return ImageIO::Error::None;
}

auto savePNG(
    const Image& image,
    const char* filename,
    bool unpremultiply_alpha
) noexcept -> ImageIO::Error {

    if (filename == nullptr ||
        filename[0] == '\0' ||
        !image.isUsable()) {
        return ImageIO::Error::InvalidArgument;
    }

    const auto pixel_type = image.pixelType();
    const auto components = image.componentsPerPixel();

    const bool is_float =
        pixel_type == Image::PixelType::Float32;

    if (!is_float &&
        pixel_type != Image::PixelType::UInt8) {
        return ImageIO::Error::UnsupportedPixelType;
    }

    int color_type = 0;

    switch (components) {
        case 1: color_type = PNG_COLOR_TYPE_GRAY; break;
        case 2: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
        case 3: color_type = PNG_COLOR_TYPE_RGB; break;
        case 4: color_type = PNG_COLOR_TYPE_RGBA; break;
        default: return ImageIO::Error::UnsupportedPixelType;
    }

    const auto width = image.width();
    const auto height = image.height();

    if (width <= 0 || height <= 0 ||
        static_cast<std::uint64_t>(width) >
            std::numeric_limits<png_uint_32>::max() ||
        static_cast<std::uint64_t>(height) >
            std::numeric_limits<png_uint_32>::max()) {
        return ImageIO::Error::InvalidArgument;
    }

    const auto width_size =
        static_cast<std::size_t>(width);

    const auto height_size =
        static_cast<std::size_t>(height);

    const std::size_t bytes_per_component =
        is_float ? sizeof(float) : sizeof(std::uint8_t);

    if (width_size >
        std::numeric_limits<std::size_t>::max() /
            components / bytes_per_component) {
        return ImageIO::Error::InvalidArgument;
    }

    const auto source_row_bytes =
        width_size * components * bytes_per_component;

    const auto stride = image.bytesPerRow();

    if (stride < source_row_bytes) {
        return ImageIO::Error::InvalidArgument;
    }

    const std::size_t output_bytes_per_component =
        is_float ? 2 : 1;

    const auto output_row_bytes =
        width_size * components * output_bytes_per_component;

    auto* scanline = static_cast<png_bytep>(
        std::malloc(output_row_bytes)
    );

    if (scanline == nullptr) {
        return ImageIO::Error::MemoryError;
    }

    FILE* file = std::fopen(filename, "wb");

    if (file == nullptr) {
        std::free(scanline);
        return ImageIO::Error::FileError;
    }

    png_structp png_ptr = png_create_write_struct(
        PNG_LIBPNG_VER_STRING,
        nullptr,
        nullptr,
        nullptr
    );

    if (png_ptr == nullptr) {
        std::fclose(file);
        std::free(scanline);
        return ImageIO::Error::MemoryError;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr == nullptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        std::fclose(file);
        std::free(scanline);
        return ImageIO::Error::MemoryError;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        std::fclose(file);
        std::free(scanline);
        return ImageIO::Error::EncodeError;
    }

    png_init_io(png_ptr, file);

    png_set_IHDR(
        png_ptr,
        info_ptr,
        static_cast<png_uint_32>(width),
        static_cast<png_uint_32>(height),
        is_float ? 16 : 8,
        color_type,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_set_sRGB(
        png_ptr,
        info_ptr,
        PNG_sRGB_INTENT_PERCEPTUAL
    );

    png_write_info(png_ptr, info_ptr);

    for (std::size_t y = 0; y < height_size; ++y) {
        const auto* source = image.rowData(y);

        if (!is_float) {
            std::memcpy(scanline, source, output_row_bytes);
        }
        else {
            const auto pixel_count = width_size;

            for (std::size_t x = 0; x < pixel_count; ++x) {
                const auto source_offset = x * components * sizeof(float);
                const auto output_offset = x * components * 2;

                float values[4] = {};

                for (std::size_t c = 0; c < components; ++c) {

                    std::memcpy(
                        &values[c],
                        source + source_offset + c * sizeof(float),
                        sizeof(float)
                    );

                    if (!std::isfinite(values[c])) {
                        values[c] = 0.0f;
                    }
                }

                // Optional conversion from premultiplied
                // to straight alpha.
                if (unpremultiply_alpha &&
                    (components == 2 || components == 4)) {

                    const auto alpha_index = components - 1;
                    const float alpha = values[alpha_index];

                    if (alpha > 0.0f) {
                        for (std::size_t c = 0; c < alpha_index; ++c) {
                            values[c] /= alpha;
                        }
                    }
                    else {
                        for (std::size_t c = 0; c < alpha_index; ++c) {
                            values[c] = 0.0f;
                        }
                    }
                }

                for (std::size_t c = 0; c < components; ++c) {
                    const float value = std::clamp(values[c], 0.0f, 1.0f);

                    const auto sample =
                        static_cast<std::uint16_t>(
                            std::lround(
                                static_cast<double>(value) * 65535.0
                            )
                        );

                    // PNG stores 16-bit samples big-endian.
                    scanline[output_offset + c * 2] =
                        static_cast<png_byte>(sample >> 8);

                    scanline[output_offset + c * 2 + 1] =
                        static_cast<png_byte>(sample & 0xFF);
                }
            }
        }

        png_write_row(png_ptr, scanline);
    }

    png_write_end(png_ptr, info_ptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    const bool write_error = std::ferror(file) != 0;
    const bool close_error = std::fclose(file) != 0;

    std::free(scanline);

    if (write_error || close_error) {
        return ImageIO::Error::FileError;
    }

    return ImageIO::Error::None;
}

} // namespace Grain::Detail
