#pragma once

#include <cstddef>
#include <memory>
#include <cstring>
#include <type_traits>


namespace Grain {

class Image {
public:
    enum class PixelType : uint8_t {
        UInt8,
        UInt16,
        UInt32,
        Float
    };

    enum class StorageMode : uint8_t {
        Empty,
        Owned,
        Borrowed
    };

    Image() noexcept = default;
    ~Image() = default;

    Image(const Image&) = delete;
    auto operator = (const Image&) -> Image& = delete;

    Image(Image&& other) noexcept;
    auto operator = (Image&& other) noexcept -> Image&;

    [[nodiscard]]
    auto create(
        uint32_t width,
        uint32_t height,
        uint8_t components_per_pixel,
        PixelType pixel_type
    ) noexcept -> bool;

    [[nodiscard]]
    auto wrap(
        uint32_t width,
        uint32_t height,
        uint8_t components_per_pixel,
        PixelType pixel_type,
        uint8_t* data,
        size_t buffer_size,
        size_t row_stride = 0
    ) noexcept -> bool;

    void reset() noexcept;

    [[nodiscard]] auto isUsable() const noexcept -> bool;
    [[nodiscard]] auto ownsPixelData() const noexcept -> bool;

    [[nodiscard]] auto width() const noexcept -> uint32_t;
    [[nodiscard]] auto height() const noexcept -> uint32_t;
    [[nodiscard]] auto pixelType() const noexcept -> PixelType;
    [[nodiscard]] auto componentsPerPixel() const noexcept -> uint8_t;

    [[nodiscard]] auto bytesPerPixel() const noexcept -> size_t;
    [[nodiscard]] auto bytesPerRow() const noexcept -> size_t;
    [[nodiscard]] auto pixelDataSize() const noexcept -> size_t;

    [[nodiscard]]
    auto pixelData() const noexcept -> const uint8_t*;

    [[nodiscard]]
    auto mutPixelData() noexcept -> uint8_t*;

    [[nodiscard]]
    auto rowData(uint32_t y) noexcept -> uint8_t*;

    [[nodiscard]]
    auto rowData(uint32_t y) const noexcept -> const uint8_t*;


    template<typename T>
    [[nodiscard]]
    auto pixel(uint32_t x, uint32_t y, uint8_t component) const noexcept -> T {
        static_assert(std::is_trivially_copyable_v<T>);

        if (!isValidPixelAccess<T>(x, y, component)) {
            return T{};
        }

        T value{};

        const auto* source =
            rowData(y) +
            (static_cast<size_t>(x) * bytesPerPixel()) +
            (static_cast<size_t>(component) * sizeof(T));

        std::memcpy(&value, source, sizeof(T));

        return value;
    }

    template<typename T>
    auto setPixel(
        uint32_t x,
        uint32_t y,
        uint8_t component,
        T value
    ) noexcept -> bool {
        static_assert(std::is_trivially_copyable_v<T>);

        if (!isValidPixelAccess<T>(x, y, component)) {
            return false;
        }

        auto* destination =
            rowData(y) +
            (static_cast<size_t>(x) * bytesPerPixel()) +
            (static_cast<size_t>(component) * sizeof(T));

        std::memcpy(destination, &value, sizeof(T));

        return true;
    }


private:
    std::unique_ptr<uint8_t[]> owned_data_;
    uint8_t* data_ = nullptr;

    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint8_t components_per_pixel_ = 0;

    PixelType pixel_type_ = PixelType::UInt8;
    StorageMode storage_mode_ = StorageMode::Empty;

    size_t row_stride_ = 0;
    size_t data_size_ = 0;


    template<typename T>
    [[nodiscard]]
    auto isValidPixelAccess(
        uint32_t x,
        uint32_t y,
        uint8_t component
    ) const noexcept -> bool {
        if (!isUsable() || x >= width_ || y >= height_) {
            return false;
        }

        if (component >= components_per_pixel_) {
            return false;
        }

        if (sizeof(T) != bytesPerComponentForPixelType()) {
            return false;
        }

        return true;
    }

    [[nodiscard]]
    auto bytesPerComponentForPixelType() const noexcept -> size_t {
        switch (pixel_type_) {
        case PixelType::UInt8:  return sizeof(uint8_t);
        case PixelType::UInt16: return sizeof(uint16_t);
        case PixelType::UInt32: return sizeof(uint32_t);
        case PixelType::Float:  return sizeof(float);
        }

        return 0;
    }
};

}