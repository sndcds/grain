
#include <grain/Image/ImageIO.hpp>

#include "ImageIO_PNG.hpp"
#include "ImageIO_JPG.hpp"
#include "ImageIO_WebP.hpp"

#include <cstdio>
#include <cstdint>
#include <print>

namespace Grain {

auto ImageIO::detectFormat(
    const char* filename
) noexcept -> Format {
    if (filename == nullptr || filename[0] == '\0') {
        return Format::Unknown;
    }

    auto* file = std::fopen(filename, "rb");

    if (file == nullptr) {
        return Format::Unknown;
    }

    uint8_t signature[12]{};
    const size_t count = std::fread(
        signature, 1, sizeof(signature), file
    );

    std::fclose(file);

    // PNG signature.
    constexpr uint8_t pngSignature[] = {
        0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A
    };

    if (count >= sizeof(pngSignature)) {
        bool matches = true;

        for (size_t i = 0; i < sizeof(pngSignature); ++i) {
            if (signature[i] != pngSignature[i]) {
                matches = false;
                break;
            }
        }

        if (matches) {
            return Format::PNG;
        }
    }

    // JPEG SOI marker.
    if (count >= 3
        && signature[0] == 0xFF
        && signature[1] == 0xD8
        && signature[2] == 0xFF) {
        return Format::JPEG;
    }

    // WebP: RIFF container with WEBP signature.
    if (count >= 12
        && signature[0] == 'R'
        && signature[1] == 'I'
        && signature[2] == 'F'
        && signature[3] == 'F'
        && signature[8] == 'W'
        && signature[9] == 'E'
        && signature[10] == 'B'
        && signature[11] == 'P') {
        return Format::WebP;
    }

    return Format::Unknown;
}

auto ImageIO::load(
    Image& image,
    const char* filename
) noexcept -> Error {
    if (filename == nullptr || filename[0] == '\0') {
        return Error::InvalidArgument;
    }

    const Format format = detectFormat(filename);

    switch (format) {
        case Format::PNG:
            return Detail::loadPNG(image, filename);

        case Format::JPEG:
            return Detail::loadJPEG(image, filename);

        case Format::WebP:
            return Error::UnsupportedFormat;

        case Format::Unknown: {
            // Distinguish an unreadable/missing file from an
            // unrecognized file format.
            auto* file = std::fopen(filename, "rb");

            if (file == nullptr) {
                return Error::FileError;
            }

            std::fclose(file);
            return Error::UnsupportedFormat;
        }
    }

    return Error::UnsupportedFormat;
}

auto ImageIO::save(
    const Image& image,
    const char* filename,
    Format format,
    int quality
) noexcept -> Error {
    (void)quality; // Used by JPEG and WebP implementations later.

    if (filename == nullptr || filename[0] == '\0') {
        return Error::InvalidArgument;
    }

    switch (format) {
    case Format::PNG:
            return Detail::savePNG(image, filename);

        case Format::JPEG:
            return Detail::saveJPEG(image, filename, quality);

        case Format::WebP:
            return Detail::saveWebP(image, filename, quality);

        case Format::Unknown:
            return Error::UnsupportedFormat;
    }

    return Error::UnsupportedFormat;
}

} // namespace Grain
