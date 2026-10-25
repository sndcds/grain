
#include "ImageIO_PNG.hpp"

#include <cstdint>
#include <limits>

#include <png.h>

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
    const char* filename
) noexcept -> ImageIO::Error {
    if (filename == nullptr || filename[0] == '\0'
        || !image.isUsable()) {
        return ImageIO::Error::InvalidArgument;
    }

    if (image.pixelType() != Image::PixelType::UInt8) {
        return ImageIO::Error::UnsupportedPixelType;
    }

    png_uint_32 format = 0;

    switch (image.componentsPerPixel()) {
        case 1:
            format = PNG_FORMAT_GRAY;
            break;

        case 2:
            format = PNG_FORMAT_GA;
            break;

        case 3:
            format = PNG_FORMAT_RGB;
            break;

        case 4:
            format = PNG_FORMAT_RGBA;
            break;

        default:
            return ImageIO::Error::UnsupportedPixelType;
    }

    const size_t stride = image.bytesPerRow();

    if (!validPNGStride(stride)) {
        return ImageIO::Error::UnsupportedPixelType;
    }

    png_image png{};
    png.version = PNG_IMAGE_VERSION;
    png.opaque = nullptr;
    png.width = image.width();
    png.height = image.height();
    png.format = format;

    const int success = png_image_write_to_file(
        &png,
        filename,
        0, // Do not convert from 16-bit.
        image.pixelData(),
        static_cast<png_int_32>(stride),
        nullptr
    );

    png_image_free(&png);

    if (success == 0) {
        return ImageIO::Error::EncodeError;
    }

    return ImageIO::Error::None;
}

} // namespace Grain::Detail
