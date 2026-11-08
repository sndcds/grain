
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <csetjmp>
#include <iostream>

#include <jpeglib.h>
#include <ostream>
#include <vector>

#include <grain/Image/Image.hpp>
#include <grain/Image/ImageIO.hpp>

namespace Grain::Detail {

namespace {

struct JPEGErrorManager {
    jpeg_error_mgr manager;
    std::jmp_buf jumpBuffer;
};

auto jpegErrorExit(j_common_ptr info) -> void {
    auto* error = reinterpret_cast<JPEGErrorManager*>(info->err);
    std::longjmp(error->jumpBuffer, 1);
}

auto floatToByte(const float value) noexcept -> JSAMPLE {
    const float normalized = std::isfinite(value)
        ? std::clamp(value, 0.0f, 1.0f)
        : 0.0f;

    return static_cast<JSAMPLE>(
        std::lround(normalized * 255.0f)
    );
}

auto convertScanlineToRGB(
    const Image& image,
    const std::size_t y,
    JSAMPLE* destination
) noexcept -> void {

    const auto width = static_cast<std::size_t>(image.width());
    const auto components = image.componentsPerPixel();
    const auto is_float = image.pixelType() == Image::PixelType::Float32;

    const auto* source = image.rowData(y);

    // Grayscale and RGB.
    if (components != 4) {
        if (is_float) {
            const auto* pixels = reinterpret_cast<const float*>(source);

            for (std::size_t x = 0; x < width; ++x) {
                for (uint32_t c = 0; c < components; ++c) {

                    destination[x * components + c] =
                        floatToByte(pixels[x * components + c]);
                }
            }
        } else {
            const auto* pixels = reinterpret_cast<const JSAMPLE*>(source);

            std::memcpy(
                destination,
                pixels,
                width * components * sizeof(JSAMPLE)
            );
        }

        return;
    }

    // RGBA: composite premultiplied pixels against white.
    for (std::size_t x = 0; x < width; ++x) {

        float rgba[4];

        if (is_float) {
            const auto* pixel = reinterpret_cast<const float*>(source) + x * 4;

            for (int c = 0; c < 4; ++c) {
                rgba[c] = std::isfinite(pixel[c])
                    ? std::clamp(pixel[c], 0.0f, 1.0f)
                    : 0.0f;
            }
        } else {
            const auto* pixel = reinterpret_cast<const JSAMPLE*>(source) + x * 4;

            for (int c = 0; c < 4; ++c) {
                rgba[c] = static_cast<float>(pixel[c]) / 255.0f;
            }
        }

        const float alpha = rgba[3];

        for (int c = 0; c < 3; ++c) {
            const float composited = rgba[c] + (1.0f - alpha);
            destination[x * 3 + c] = floatToByte(composited);
        }
    }
}

} // namespace

auto loadJPEG(Image& image, const char* filename) noexcept
    -> ImageIO::Error {

    if (filename == nullptr) {
        return ImageIO::Error::InvalidArgument;
    }

    FILE* file = std::fopen(filename, "rb");
    if (file == nullptr) {
        return ImageIO::Error::FileError;
    }

    jpeg_decompress_struct decoder {};
    JPEGErrorManager error {};

    decoder.err = jpeg_std_error(&error.manager);
    error.manager.error_exit = jpegErrorExit;

    if (setjmp(error.jumpBuffer)) {
        jpeg_destroy_decompress(&decoder);
        std::fclose(file);
        return ImageIO::Error::DecodeError;
    }

    jpeg_create_decompress(&decoder);
    jpeg_stdio_src(&decoder, file);

    if (jpeg_read_header(&decoder, TRUE) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&decoder);
        std::fclose(file);
        return ImageIO::Error::DecodeError;
    }

    decoder.out_color_space =
        decoder.jpeg_color_space == JCS_GRAYSCALE
            ? JCS_GRAYSCALE
            : JCS_RGB;

    jpeg_start_decompress(&decoder);

    const uint32_t width = decoder.output_width;
    const uint32_t height = decoder.output_height;
    const int components = decoder.output_components;

    if (width == 0 || height == 0 ||
        (components != 1 && components != 3)) {
        jpeg_destroy_decompress(&decoder);
        std::fclose(file);
        return ImageIO::Error::DecodeError;
    }

    if (!image.create(
        width,
        height,
        static_cast<uint8_t>(components),
        Image::PixelType::UInt8
    )) {
        jpeg_destroy_decompress(&decoder);
        std::fclose(file);
        return ImageIO::Error::OutOfMemory;
    }

    while (decoder.output_scanline < decoder.output_height) {
        const uint32_t y = decoder.output_scanline;
        auto* row = reinterpret_cast<JSAMPROW>(image.rowData(y));

        if (jpeg_read_scanlines(&decoder, &row, 1) != 1) {
            jpeg_destroy_decompress(&decoder);
            std::fclose(file);
            return ImageIO::Error::DecodeError;
        }
    }

    jpeg_finish_decompress(&decoder);
    jpeg_destroy_decompress(&decoder);
    std::fclose(file);

    return ImageIO::Error::None;
}

auto saveJPEG(
    const Image& image,
    const char* filename,
    int quality
) noexcept -> ImageIO::Error {

    if (filename == nullptr || !image.isUsable()) {
        return ImageIO::Error::InvalidArgument;
    }

    const auto components = image.componentsPerPixel();

    if (components != 1 && components != 3 && components != 4) {
        return ImageIO::Error::UnsupportedFormat;
    }

    const auto pixel_type = image.pixelType();

    if (pixel_type != Image::PixelType::UInt8 &&
        pixel_type != Image::PixelType::Float32) {
        return ImageIO::Error::UnsupportedPixelType;
    }

    const auto width = image.width();
    const auto height = image.height();

    if (width <= 0 || height <= 0) {
        return ImageIO::Error::InvalidArgument;
    }

    const auto width_size = static_cast<std::size_t>(width);

    const uint32_t output_components = components == 1 ? 1 : 3;

    if (width_size >
        std::numeric_limits<std::size_t>::max() / output_components) {
        return ImageIO::Error::InvalidArgument;
    }

    std::vector<JSAMPLE> scanline;

    try {
        scanline.resize(width_size * output_components);
    } catch (...) {
        return ImageIO::Error::MemoryError;
    }

    FILE* file = std::fopen(filename, "wb");

    if (file == nullptr) {
        return ImageIO::Error::FileError;
    }

    jpeg_compress_struct encoder {};
    JPEGErrorManager error {};

    encoder.err = jpeg_std_error(&error.manager);
    error.manager.error_exit = jpegErrorExit;

    if (setjmp(error.jumpBuffer)) {
        jpeg_destroy_compress(&encoder);
        std::fclose(file);
        return ImageIO::Error::EncodeError;
    }

    jpeg_create_compress(&encoder);
    jpeg_stdio_dest(&encoder, file);

    encoder.image_width = static_cast<JDIMENSION>(width);
    encoder.image_height = static_cast<JDIMENSION>(height);
    encoder.input_components = static_cast<int>(output_components);

    encoder.in_color_space =
        output_components == 1 ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_set_defaults(&encoder);
    jpeg_set_quality(&encoder, std::clamp(quality, 0, 100), TRUE);

    jpeg_start_compress(&encoder, TRUE);

    while (encoder.next_scanline < encoder.image_height) {

        const auto y = static_cast<std::size_t>(encoder.next_scanline);

        JSAMPROW row = nullptr;

        const bool requires_conversion =
            pixel_type == Image::PixelType::Float32 ||
            components == 4;

        if (requires_conversion) {

            convertScanlineToRGB(
                image,
                y,
                scanline.data()
            );

            row = scanline.data();

        } else {

            row = const_cast<JSAMPROW>(
                reinterpret_cast<const JSAMPLE*>(
                    image.rowData(y)
                )
            );
        }

        if (jpeg_write_scanlines(&encoder, &row, 1) != 1) {
            jpeg_destroy_compress(&encoder);
            std::fclose(file);
            return ImageIO::Error::EncodeError;
        }
    }

    jpeg_finish_compress(&encoder);
    jpeg_destroy_compress(&encoder);

    const bool write_error = std::ferror(file) != 0;
    const bool close_error = std::fclose(file) != 0;

    if (write_error || close_error) {
        return ImageIO::Error::FileError;
    }

    return ImageIO::Error::None;
}

} // namespace Grain::Detail
