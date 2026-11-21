
#include <grain/Image/ImageAccess.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace Grain {

ImageAccess::ImageAccess(Image& image) noexcept {
    setup(image);
}

auto ImageAccess::setup(Image& image) noexcept -> bool {
    undefine();

    if (!image.isUsable() ||
        image.width() <= 0 ||
        image.height() <= 0 ||
        image.componentsPerPixel() <= 0 ||
        image.componentsPerPixel() > MaxComponents ||
        image.mutPixelData() == nullptr) {
        return false;
    }

    image_ = &image;
    data_ = image.mutPixelData();

    width_ = image.width();
    height_ = image.height();
    component_count_ = image.componentsPerPixel();

    pixel_stride_ = image.bytesPerPixel();
    row_stride_ = image.bytesPerRow();
    pixel_type_ = image.pixelType();

    if (pixel_stride_ == 0 ||
        row_stride_ < static_cast<size_t>(width_) * pixel_stride_) {
        undefine();
        return false;
    }

    selectTransferFunctions();

    if (read_fn_ == nullptr || write_fn_ == nullptr) {
        undefine();
        return false;
    }

    resetRegion();
    usable_ = true;

    return beginRow(0);
}

void ImageAccess::undefine() noexcept {
    image_ = nullptr;
    data_ = nullptr;
    row_ptr_ = nullptr;
    current_ptr_ = nullptr;

    width_ = 0;
    height_ = 0;
    component_count_ = 0;

    x_ = 0;
    y_ = 0;

    region_x1_ = 0;
    region_y1_ = 0;
    region_x2_ = 0;
    region_y2_ = 0;

    pixel_stride_ = 0;
    row_stride_ = 0;

    read_fn_ = nullptr;
    write_fn_ = nullptr;

    usable_ = false;
}

void ImageAccess::resetRegion() noexcept {
    region_x1_ = 0;
    region_y1_ = 0;
    region_x2_ = width_;
    region_y2_ = height_;

    current_ptr_ = nullptr;
    row_ptr_ = nullptr;
}

auto ImageAccess::setRegion(
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2) noexcept -> bool {

    if (!usable_ ||
        x1 < 0 || y1 < 0 ||
        x2 > width_ || y2 > height_ ||
        x1 >= x2 || y1 >= y2) {
        return false;
    }

    region_x1_ = x1;
    region_y1_ = y1;
    region_x2_ = x2;
    region_y2_ = y2;

    current_ptr_ = nullptr;
    row_ptr_ = nullptr;

    return true;
}

auto ImageAccess::setPos(int32_t x, int32_t y) noexcept -> bool {
    if (!usable_ ||
        x < region_x1_ || x >= region_x2_ ||
        y < region_y1_ || y >= region_y2_) {
        return false;
    }

    x_ = x;
    y_ = y;
    updateCurrentPtr();

    return true;
}

auto ImageAccess::beginRow(int32_t y) noexcept -> bool {
    if (!usable_ || y < region_y1_ || y >= region_y2_) {
        row_ptr_ = nullptr;
        current_ptr_ = nullptr;
        return false;
    }

    y_ = y;
    x_ = region_x1_;

    row_ptr_ = data_
        + static_cast<size_t>(y_) * row_stride_
        + static_cast<size_t>(region_x1_) * pixel_stride_;

    current_ptr_ = row_ptr_;

    return true;
}

// Advances to the next pixel. Call beginRow() first and process the
// current pixel before calling nextX() for the first time.
auto ImageAccess::nextX() noexcept -> bool {
    if (!usable_ || current_ptr_ == nullptr) {
        return false;
    }

    ++x_;

    if (x_ >= region_x2_) {
        current_ptr_ = nullptr;
        return false;
    }

    current_ptr_ += pixel_stride_;
    return true;
}

void ImageAccess::updateCurrentPtr() noexcept {
    if (data_ == nullptr) {
        row_ptr_ = nullptr;
        current_ptr_ = nullptr;
        return;
    }

    row_ptr_ = data_
        + static_cast<size_t>(y_) * row_stride_
        + static_cast<size_t>(region_x1_) * pixel_stride_;

    current_ptr_ = data_
        + static_cast<size_t>(y_) * row_stride_
        + static_cast<size_t>(x_) * pixel_stride_;
}

auto ImageAccess::read() noexcept -> bool {
    if (!usable_ || current_ptr_ == nullptr || transfer_ptr_ == nullptr) {
        return false;
    }

    read_fn_(current_ptr_, transfer_ptr_, component_count_);
    return true;
}

auto ImageAccess::write() noexcept -> bool {
    if (!usable_ || current_ptr_ == nullptr || transfer_ptr_ == nullptr) {
        return false;
    }

    write_fn_(current_ptr_, transfer_ptr_, component_count_);
    return true;
}

auto ImageAccess::readAt(int32_t x, int32_t y) noexcept -> bool {
    return setPos(x, y) && read();
}

auto ImageAccess::readInterpolated(
    const Vec2d& position
) noexcept -> bool {

    if (!usable_ ||
        !data_ ||
        !current_ptr_ ||
        !transfer_ptr_ ||
        component_count_ <= 0 ||
        component_count_ > MaxComponents ||
        width_ <= 0 ||
        height_ <= 0 ||
        !std::isfinite(position.x) ||
        !std::isfinite(position.y) ||
        position.x < 0.0 ||
        position.y < 0.0 ||
        position.x >= static_cast<double>(width_) ||
        position.y >= static_cast<double>(height_)) {
        return false;
    }

    // Preserve the current pixel cursor.
    auto* const saved_ptr = current_ptr_;

    // Calculate the four surrounding pixel coordinates.
    const int32_t x0 = static_cast<int32_t>(std::floor(position.x));
    const int32_t y0 = static_cast<int32_t>(std::floor(position.y));

    const int32_t x1 = std::min(x0 + 1, width_ - 1);
    const int32_t y1 = std::min(y0 + 1, height_ - 1);

    const float xf1 = static_cast<float>(position.x - x0);
    const float xf0 = 1.0f - xf1;

    const float yf1 = static_cast<float>(position.y - y0);
    const float yf0 = 1.0f - yf1;

    const float coefficients[2][2] = {
        { xf0 * yf0, xf1 * yf0 },
        { xf0 * yf1, xf1 * yf1 }
    };

    const int32_t xs[2] = { x0, x1 };
    const int32_t ys[2] = { y0, y1 };

    float sum[MaxComponents] = {};

    // Read and accumulate the four surrounding pixels.
    for (int32_t yi = 0; yi < 2; ++yi) {
        for (int32_t xi = 0; xi < 2; ++xi) {

            const auto* const pixel_ptr =
                data_ +
                static_cast<size_t>(ys[yi]) * row_stride_ +
                static_cast<size_t>(xs[xi]) * pixel_stride_;

            current_ptr_ = const_cast<uint8_t*>(pixel_ptr);

            if (!read()) {
                current_ptr_ = saved_ptr;
                return false;
            }

            const float coefficient = coefficients[yi][xi];

            for (int32_t i = 0; i < component_count_; ++i) {
                sum[i] += transfer_ptr_[i] * coefficient;
            }
        }
    }

    // Store the interpolated result in the transfer buffer.
    for (int32_t i = 0; i < component_count_; ++i) {
        transfer_ptr_[i] = sum[i];
    }

    // Restore the original pixel cursor.
    current_ptr_ = saved_ptr;

    return true;
}

auto ImageAccess::writeAt(int32_t x, int32_t y) noexcept -> bool {
    return setPos(x, y) && write();
}

template<typename T>
void ImageAccess::readInteger(
    const uint8_t* source,
    float* destination,
    int32_t components) noexcept {

    constexpr double scale =
        1.0 / static_cast<double>(std::numeric_limits<T>::max());

    for (int32_t i = 0; i < components; ++i) {
        T value{};
        std::memcpy(&value, source + static_cast<size_t>(i) * sizeof(T),
                    sizeof(T));
        destination[i] = static_cast<float>(
            static_cast<double>(value) * scale);
    }
}

template<typename T>
void ImageAccess::writeInteger(
    uint8_t* destination,
    const float* source,
    int32_t components) noexcept {

    constexpr double max_value =
        static_cast<double>(std::numeric_limits<T>::max());

    for (int32_t i = 0; i < components; ++i) {
        const double normalized =
            std::clamp(static_cast<double>(source[i]), 0.0, 1.0);

        const auto value = static_cast<T>(
            std::round(normalized * max_value));

        std::memcpy(destination + static_cast<size_t>(i) * sizeof(T),
                    &value, sizeof(T));
    }
}

void ImageAccess::readFloat(
    const uint8_t* source,
    float* destination,
    int32_t components) noexcept {

    std::memcpy(destination, source,
                static_cast<size_t>(components) * sizeof(float));
}

void ImageAccess::writeFloat(
    uint8_t* destination,
    const float* source,
    int32_t components) noexcept {

    std::memcpy(destination, source,
                static_cast<size_t>(components) * sizeof(float));
}

void ImageAccess::selectTransferFunctions() noexcept {
    read_fn_ = nullptr;
    write_fn_ = nullptr;

    switch (pixel_type_) {
        case Image::PixelType::UInt8:
            read_fn_ = &readInteger<uint8_t>;
            write_fn_ = &writeInteger<uint8_t>;
            break;

        case Image::PixelType::UInt16:
            read_fn_ = &readInteger<uint16_t>;
            write_fn_ = &writeInteger<uint16_t>;
            break;

        case Image::PixelType::UInt32:
            read_fn_ = &readInteger<uint32_t>;
            write_fn_ = &writeInteger<uint32_t>;
            break;

        case Image::PixelType::Float32:
            read_fn_ = &readFloat;
            write_fn_ = &writeFloat;
            break;
    }
}

} // namespace Grain
