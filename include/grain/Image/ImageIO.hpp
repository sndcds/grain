#pragma once

#include <grain/Image/Image.hpp>

namespace Grain {

class ImageIO {
public:
    enum class Format : uint8_t {
        Unknown,
        PNG,
        JPEG,
        WebP
    };

    enum class Error : uint8_t {
        None,
        InvalidArgument,
        UnsupportedFormat,
        FileError,
        DecodeError,
        EncodeError,
        OutOfMemory,
        UnsupportedPixelType
    };

    [[nodiscard]] static auto load(
        Image& image,
        const char* filename
    ) noexcept -> Error;

    [[nodiscard]] static auto save(
        const Image& image,
        const char* filename,
        Format format,
        int quality = 90
    ) noexcept -> Error;

    [[nodiscard]] static auto detectFormat(
        const char* filename
    ) noexcept -> Format;
};

}