
#include <cstdio>
#include <cstdint>
#include <csetjmp>
#include <iostream>

#include <jpeglib.h>
#include <ostream>

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

    if (image.pixelType() != Image::PixelType::UInt8) {
        return ImageIO::Error::UnsupportedPixelType;
    }

    const uint32_t components = image.componentsPerPixel();

    if (components != 1 && components != 3) {
        return ImageIO::Error::UnsupportedFormat;
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

    encoder.image_width = image.width();
    encoder.image_height = image.height();
    encoder.input_components = static_cast<int>(components);
    encoder.in_color_space =
        components == 1 ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_set_defaults(&encoder);
    jpeg_set_quality(&encoder, quality < 0 ? 0 :
        (quality > 100 ? 100 : quality), TRUE);

    jpeg_start_compress(&encoder, TRUE);

    while (encoder.next_scanline < encoder.image_height) {
        const uint32_t y = encoder.next_scanline;

        auto* row = const_cast<JSAMPROW>(
            reinterpret_cast<const JSAMPLE*>(image.rowData(y))
        );

        if (jpeg_write_scanlines(&encoder, &row, 1) != 1) {
            jpeg_destroy_compress(&encoder);
            std::fclose(file);
            return ImageIO::Error::EncodeError;
        }
    }

    jpeg_finish_compress(&encoder);
    jpeg_destroy_compress(&encoder);

    const bool writeError = std::ferror(file) != 0;
    const bool closeError = std::fclose(file) != 0;

    if (writeError || closeError) {
        return ImageIO::Error::FileError;
    }

    return ImageIO::Error::None;
}

} // namespace Grain::Detail
