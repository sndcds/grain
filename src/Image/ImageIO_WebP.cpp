
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
    int quality
) noexcept -> ImageIO::Error {

    if (filename == nullptr || !image.isUsable()) {
        return ImageIO::Error::InvalidArgument;
    }

    if (image.pixelType() != Image::PixelType::UInt8) {
        return ImageIO::Error::UnsupportedPixelType;
    }

    const uint32_t width = image.width();
    const uint32_t height = image.height();
    const uint32_t components = image.componentsPerPixel();

    if (width == 0 || height == 0 ||
        width > 16383 || height > 16383) {
        return ImageIO::Error::UnsupportedFormat;
    }

    if (components != 3 && components != 4) {
        return ImageIO::Error::UnsupportedFormat;
    }

    const size_t rowBytes =
        static_cast<size_t>(width) * components;

    // Determine the actual row stride. Image rows are expected to
    // belong to the same contiguous pixel buffer.
    size_t rowStride = rowBytes;

    if (height > 1) {
        const auto* first = image.rowData(0);
        const auto* second = image.rowData(1);

        if (second <= first) {
            return ImageIO::Error::InvalidArgument;
        }

        rowStride = static_cast<size_t>(second - first);
    }

    if (rowStride < rowBytes ||
        rowStride >
            static_cast<size_t>(std::numeric_limits<int>::max())) {
        return ImageIO::Error::UnsupportedFormat;
    }

    uint8_t* encoded = nullptr;

    const auto qualityValue = static_cast<float>(
        quality < 0 ? 0 : (quality > 100 ? 100 : quality)
    );

    const size_t encodedSize = components == 4
        ? WebPEncodeRGBA(
            image.rowData(0),
            static_cast<int>(width),
            static_cast<int>(height),
            static_cast<int>(rowStride),
            qualityValue,
            &encoded
        )
        : WebPEncodeRGB(
            image.rowData(0),
            static_cast<int>(width),
            static_cast<int>(height),
            static_cast<int>(rowStride),
            qualityValue,
            &encoded
        );

    if (encodedSize == 0 || encoded == nullptr) {
        WebPFree(encoded);
        return ImageIO::Error::EncodeError;
    }

    std::FILE* file = std::fopen(filename, "wb");

    if (file == nullptr) {
        WebPFree(encoded);
        return ImageIO::Error::FileError;
    }

    const size_t bytesWritten = std::fwrite(
        encoded, 1, encodedSize, file
    );

    const bool writeError = bytesWritten != encodedSize;
    const bool closeError = std::fclose(file) != 0;

    WebPFree(encoded);

    if (writeError || closeError) {
        return ImageIO::Error::FileError;
    }

    return ImageIO::Error::None;
}

} // namespace Grain::Detail
