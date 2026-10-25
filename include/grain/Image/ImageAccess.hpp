
#pragma once

#include <cstddef>
#include <cstdint>

#include <grain/Image/Image.hpp>

namespace Grain {

class ImageAccess {
public:
    static constexpr int32_t MaxComponents = 4;

    ImageAccess() = default;
    explicit ImageAccess(Image& image) noexcept;

    ImageAccess(const ImageAccess&) = delete;
    auto operator=(const ImageAccess&) -> ImageAccess& = delete;

    ImageAccess(ImageAccess&&) noexcept = default;
    auto operator=(ImageAccess&&) noexcept -> ImageAccess& = default;

    [[nodiscard]] auto setup(Image& image) noexcept -> bool;
    void undefine() noexcept;

    [[nodiscard]] auto isUsable() const noexcept -> bool {
        return usable_;
    }

    [[nodiscard]] auto width() const noexcept -> int32_t {
        return width_;
    }

    [[nodiscard]] auto height() const noexcept -> int32_t {
        return height_;
    }

    [[nodiscard]] auto componentCount() const noexcept -> int32_t {
        return component_count_;
    }

    [[nodiscard]] auto x() const noexcept -> int32_t {
        return x_;
    }

    [[nodiscard]] auto y() const noexcept -> int32_t {
        return y_;
    }

    [[nodiscard]] auto pixelStride() const noexcept -> size_t {
        return pixel_stride_;
    }

    [[nodiscard]] auto rowStride() const noexcept -> size_t {
        return row_stride_;
    }

    // Set the current pixel. Coordinates outside the image are rejected.
    [[nodiscard]] auto setPos(int32_t x, int32_t y) noexcept -> bool;

    // Set the horizontal iteration range [x1, x2).
    // Set the vertical iteration range [y1, y2).
    [[nodiscard]] auto setRegion(
        int32_t x1,
        int32_t y1,
        int32_t x2,
        int32_t y2) noexcept -> bool;

    void resetRegion() noexcept;

    // Iteration:
    //   beginRow(y);
    //   while (nextX()) { ... }
    // Or use beginRow(y) and increment pixelPtr() manually.
    [[nodiscard]] auto beginRow(int32_t y) noexcept -> bool;
    [[nodiscard]] auto nextX() noexcept -> bool;

    // Raw pointer to the current pixel.
    [[nodiscard]] auto pixelPtr() noexcept -> uint8_t* {
        return current_ptr_;
    }

    [[nodiscard]] auto pixelPtr() const noexcept -> const uint8_t* {
        return current_ptr_;
    }

    // Raw pointer to the first pixel in the current row region.
    [[nodiscard]] auto rowPtr() noexcept -> uint8_t* {
        return row_ptr_;
    }

    [[nodiscard]] auto rowPtr() const noexcept -> const uint8_t* {
        return row_ptr_;
    }

    // Transfer buffer for normalized values.
    // Unused components are initialized to zero; component 3 defaults to 1.
    void setTransferPtr(float* values) noexcept {
        transfer_ptr_ = values;
    }

    [[nodiscard]] auto transferPtr() noexcept -> float* {
        return transfer_ptr_;
    }

    [[nodiscard]] auto read() noexcept -> bool;
    [[nodiscard]] auto write() noexcept -> bool;

    // Read/write the current pixel without changing the cursor.
    [[nodiscard]] auto readAt(int32_t x, int32_t y) noexcept -> bool;
    [[nodiscard]] auto writeAt(int32_t x, int32_t y) noexcept -> bool;

private:
    using ReadFn = void (*)(const uint8_t*, float*, int32_t) noexcept;
    using WriteFn = void (*)(uint8_t*, const float*, int32_t) noexcept;

    Image* image_ = nullptr;
    uint8_t* data_ = nullptr;
    uint8_t* row_ptr_ = nullptr;
    uint8_t* current_ptr_ = nullptr;

    int32_t width_ = 0;
    int32_t height_ = 0;
    int32_t component_count_ = 0;

    int32_t x_ = 0;
    int32_t y_ = 0;

    int32_t region_x1_ = 0;
    int32_t region_y1_ = 0;
    int32_t region_x2_ = 0;
    int32_t region_y2_ = 0;

    size_t pixel_stride_ = 0;
    size_t row_stride_ = 0;

    Image::PixelType pixel_type_ = Image::PixelType::UInt8;

    float internal_values_[MaxComponents] = {0.0f, 0.0f, 0.0f, 1.0f};
    float* transfer_ptr_ = internal_values_;

    ReadFn read_fn_ = nullptr;
    WriteFn write_fn_ = nullptr;

    bool usable_ = false;

    void updateCurrentPtr() noexcept;
    void selectTransferFunctions() noexcept;

    template<typename T>
    static void readInteger(
        const uint8_t* source,
        float* destination,
        int32_t components) noexcept;

    template<typename T>
    static void writeInteger(
        uint8_t* destination,
        const float* source,
        int32_t components) noexcept;

    static void readFloat(
        const uint8_t* source,
        float* destination,
        int32_t components) noexcept;

    static void writeFloat(
        uint8_t* destination,
        const float* source,
        int32_t components) noexcept;
};

} // namespace Grain
