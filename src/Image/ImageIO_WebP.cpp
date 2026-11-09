
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <webp/decode.h>
#include <webp/encode.h>

#include <grain/Image/Image.hpp>
#include <grain/Image/ImageIO.hpp>
#include <utility>

namespace Grain::Detail {

auto loadWebP(
    Image& image,
    const char* filename
) noexcept -> ImageIO::Error {

    if (filename == nullptr) {
        return ImageIO::Error::InvalidArgument;
    }

    std::FILE* file = std::fopen(filename, "rb");

    if (file == nullptr) {
        return ImageIO::Error::FileError;
    }

    if (std::fseek(file, 0, SEEK_END) != 0) {
        std::fclose(file);
        return ImageIO::Error::FileError;
    }

    const long fileSize = std::ftell(file);

    if (fileSize <= 0 ||
        std::fseek(file, 0, SEEK_SET) != 0) {
        std::fclose(file);
        return ImageIO::Error::FileError;
    }

    auto* data = static_cast<uint8_t*>(
        std::malloc(static_cast<size_t>(fileSize))
    );

    if (data == nullptr) {
        std::fclose(file);
        return ImageIO::Error::OutOfMemory;
    }

    const size_t bytesRead = std::fread(
        data, 1, static_cast<size_t>(fileSize), file
    );

    const bool readError = std::cmp_not_equal(bytesRead ,fileSize);

    std::fclose(file);

    if (readError) {
        std::free(data);
        return ImageIO::Error::FileError;
    }

    WebPBitstreamFeatures features {};

    const VP8StatusCode featureStatus = WebPGetFeatures(
        data, bytesRead, &features
    );

    if (featureStatus != VP8_STATUS_OK ||
        features.width <= 0 ||
        features.height <= 0) {
        std::free(data);
        return ImageIO::Error::DecodeError;
    }

    const bool hasAlpha = features.has_alpha != 0;
    const int components = hasAlpha ? 4 : 3;

    int width = 0;
    int height = 0;

    uint8_t* decoded = hasAlpha
        ? WebPDecodeRGBA(data, bytesRead, &width, &height)
        : WebPDecodeRGB(data, bytesRead, &width, &height);

    std::free(data);

    if (decoded == nullptr) {
        return ImageIO::Error::DecodeError;
    }

    if (width <= 0 || height <= 0 ||
        width != features.width ||
        height != features.height) {
        WebPFree(decoded);
        return ImageIO::Error::DecodeError;
    }

    if (!image.create(
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        static_cast<uint8_t>(components),
        Image::PixelType::UInt8
    )) {
        WebPFree(decoded);
        return ImageIO::Error::OutOfMemory;
    }

    const size_t rowBytes =
        static_cast<size_t>(width) *
        static_cast<size_t>(components);

    for (int y = 0; y < height; ++y) {
        std::memcpy(
            image.rowData(static_cast<uint32_t>(y)),
            decoded + (static_cast<size_t>(y) * rowBytes),
            rowBytes
        );
    }

    WebPFree(decoded);

    return ImageIO::Error::None;
}

auto saveWebP(
    const Image& image,
    const char* filename,
    int quality,
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

    if (components != 3 && components != 4) {
        return ImageIO::Error::UnsupportedFormat;
    }

    const auto width = image.width();
    const auto height = image.height();

    if (width <= 0 || height <= 0 ||
        width > 16383 || height > 16383) {
        return ImageIO::Error::UnsupportedFormat;
    }

    const auto width_size =
        static_cast<std::size_t>(width);

    const auto height_size =
        static_cast<std::size_t>(height);

    const std::size_t source_bytes_per_component =
        is_float ? sizeof(float) : sizeof(std::uint8_t);

    if (width_size >
        std::numeric_limits<std::size_t>::max() /
            components / source_bytes_per_component) {
        return ImageIO::Error::InvalidArgument;
    }

    const auto source_row_bytes =
        width_size * components * source_bytes_per_component;

    const auto source_stride = image.bytesPerRow();

    if (source_stride < source_row_bytes) {
        return ImageIO::Error::InvalidArgument;
    }

    if (source_stride >
        static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        return ImageIO::Error::UnsupportedFormat;
    }

    const auto output_row_bytes =
        width_size * components;

    const bool needs_conversion =
        is_float ||
        (unpremultiply_alpha && components == 4);

    std::uint8_t* converted_pixels = nullptr;

    const std::uint8_t* pixels = nullptr;
    int output_stride = 0;

    if (needs_conversion) {

        if (height_size >
            std::numeric_limits<std::size_t>::max() /
                output_row_bytes) {
            return ImageIO::Error::InvalidArgument;
        }

        const auto output_size =
            output_row_bytes * height_size;

        converted_pixels = static_cast<std::uint8_t*>(
            std::malloc(output_size)
        );

        if (converted_pixels == nullptr) {
            return ImageIO::Error::MemoryError;
        }

        output_stride = static_cast<int>(output_row_bytes);

        for (std::size_t y = 0; y < height_size; ++y) {
            const auto* source = image.rowData(y);
            auto* destination = converted_pixels + y * output_row_bytes;

            if (!is_float) {
                for (std::size_t x = 0; x < width_size; ++x) {
                    const auto offset = x * components;

                    if (unpremultiply_alpha && components == 4) {
                        const auto alpha = source[offset + 3];

                        if (alpha == 0) {
                            destination[offset] = 0;
                            destination[offset + 1] = 0;
                            destination[offset + 2] = 0;
                        }
                        else {
                            for (std::size_t c = 0; c < 3; ++c) {
                                const auto value = std::lround(
                                    static_cast<double>(source[offset + c]) *
                                    255.0 / alpha
                                );

                                destination[offset + c] =
                                    static_cast<std::uint8_t>(
                                        std::clamp(value, 0L, 255L)
                                    );
                            }
                        }

                        destination[offset + 3] = alpha;
                    }
                    else {
                        std::memcpy(
                            destination + offset,
                            source + offset,
                            components
                        );
                    }
                }
            }
            else {
                for (std::size_t x = 0; x < width_size; ++x) {
                    const auto offset = x * components;
                    float values[4] = {};

                    for (std::size_t c = 0; c < components; ++c) {
                        std::memcpy(
                            &values[c],
                            source + (offset + c) * sizeof(float),
                            sizeof(float)
                        );

                        if (!std::isfinite(values[c])) {
                            values[c] = 0.0f;
                        }
                    }

                    if (unpremultiply_alpha && components == 4) {
                        const float alpha = std::clamp(values[3], 0.0f, 1.0f);
                        values[3] = alpha;

                        if (alpha > 0.0f) {
                            for (std::size_t c = 0; c < 3; ++c) {
                                values[c] /= alpha;
                            }
                        }
                        else {
                            values[0] = 0.0f;
                            values[1] = 0.0f;
                            values[2] = 0.0f;
                        }
                    }

                    for (std::size_t c = 0; c < components; ++c) {
                        const float value = std::clamp(values[c], 0.0f, 1.0f);

                        destination[offset + c] =
                            static_cast<std::uint8_t>(
                                std::lround(value * 255.0f)
                            );
                    }
                }
            }
        }

        pixels = converted_pixels;
    }
    else {

        pixels = image.rowData(0);
        output_stride = static_cast<int>(source_stride);
    }

    std::uint8_t* encoded = nullptr;

    const auto quality_value = static_cast<float>(
        std::clamp(quality, 0, 100)
    );

    const auto encoded_size = quality >= 100
        ? (components == 4
            ? WebPEncodeLosslessRGBA(
                pixels,
                width,
                height,
                output_stride,
                &encoded
            )
            : WebPEncodeLosslessRGB(
                pixels,
                width,
                height,
                output_stride,
                &encoded
            ))
        : (components == 4
            ? WebPEncodeRGBA(
                pixels,
                width,
                height,
                output_stride,
                quality_value,
                &encoded
            )
            : WebPEncodeRGB(
                pixels,
                width,
                height,
                output_stride,
                quality_value,
                &encoded
            ));

    std::free(converted_pixels);

    if (encoded_size == 0 || encoded == nullptr) {
        WebPFree(encoded);
        return ImageIO::Error::EncodeError;
    }

    FILE* file = std::fopen(filename, "wb");

    if (file == nullptr) {
        WebPFree(encoded);
        return ImageIO::Error::FileError;
    }

    const auto bytes_written = std::fwrite(
        encoded,
        1,
        encoded_size,
        file
    );

    const bool write_error =
        bytes_written != encoded_size;

    const bool close_error =
        std::fclose(file) != 0;

    WebPFree(encoded);

    if (write_error || close_error) {
        return ImageIO::Error::FileError;
    }

    return ImageIO::Error::None;
}

} // namespace Grain::Detail
