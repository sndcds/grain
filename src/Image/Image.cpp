
#include <grain/Image/Image.hpp>

#include <limits>
#include <new>
#include <utility>

namespace Grain {

namespace {

[[nodiscard]]
auto bytesPerComponent(Image::PixelType pixel_type) noexcept -> size_t {
    switch (pixel_type) {
        case Image::PixelType::UInt8:
            return sizeof(uint8_t);

        case Image::PixelType::UInt16:
            return sizeof(uint16_t);

        case Image::PixelType::UInt32:
            return sizeof(uint32_t);

        case Image::PixelType::Float:
            return sizeof(float);
    }

    return 0;
}

[[nodiscard]]
auto checkedMultiply(size_t a, size_t b, size_t& result) noexcept -> bool {
    if (a != 0 && b > std::numeric_limits<size_t>::max() / a) {
        return false;
    }

    result = a * b;
    return true;
}

[[nodiscard]]
auto calculateRowBytes(
    uint32_t width,
    uint8_t components_per_pixel,
    Image::PixelType pixel_type,
    size_t& result
) noexcept -> bool {
    const size_t component_size = bytesPerComponent(pixel_type);

    if (width == 0 ||
        components_per_pixel == 0 ||
        components_per_pixel > 4 ||
        component_size == 0) {
        return false;
    }

    size_t bytes_per_pixel = 0;

    if (!checkedMultiply(
            component_size,
            static_cast<size_t>(components_per_pixel),
            bytes_per_pixel)) {
        return false;
    }

    return checkedMultiply(
        static_cast<size_t>(width),
        bytes_per_pixel,
        result);
}

[[nodiscard]]
auto calculateDataSpan(
    uint32_t height,
    size_t row_stride,
    size_t row_bytes,
    size_t& result
) noexcept -> bool {
    if (height == 0 || row_stride < row_bytes) {
        return false;
    }

    size_t preceding_rows = 0;

    if (!checkedMultiply(
            static_cast<size_t>(height - 1),
            row_stride,
            preceding_rows)) {
        return false;
    }

    if (row_bytes >
        std::numeric_limits<size_t>::max() - preceding_rows) {
        return false;
    }

    result = preceding_rows + row_bytes;
    return true;
}

} // namespace


//------------------------------------------------------------------------------
//  Construction.
//------------------------------------------------------------------------------

Image::Image(Image&& other) noexcept
    : owned_data_(std::move(other.owned_data_)),
      data_(other.data_),
      width_(other.width_),
      height_(other.height_),
      components_per_pixel_(other.components_per_pixel_),
      pixel_type_(other.pixel_type_),
      storage_mode_(other.storage_mode_),
      row_stride_(other.row_stride_),
      data_size_(other.data_size_) {
    other.data_ = nullptr;
    other.width_ = 0;
    other.height_ = 0;
    other.components_per_pixel_ = 0;
    other.pixel_type_ = PixelType::UInt8;
    other.storage_mode_ = StorageMode::Empty;
    other.row_stride_ = 0;
    other.data_size_ = 0;
}

auto Image::operator = (Image&& other) noexcept -> Image& {
    if (this == &other) {
        return *this;
    }

    owned_data_ = std::move(other.owned_data_);

    data_ = other.data_;
    width_ = other.width_;
    height_ = other.height_;
    components_per_pixel_ = other.components_per_pixel_;
    pixel_type_ = other.pixel_type_;
    storage_mode_ = other.storage_mode_;
    row_stride_ = other.row_stride_;
    data_size_ = other.data_size_;

    other.data_ = nullptr;
    other.width_ = 0;
    other.height_ = 0;
    other.components_per_pixel_ = 0;
    other.pixel_type_ = PixelType::UInt8;
    other.storage_mode_ = StorageMode::Empty;
    other.row_stride_ = 0;
    other.data_size_ = 0;

    return *this;
}


//------------------------------------------------------------------------------
//  Storage.
//------------------------------------------------------------------------------

auto Image::create(
    uint32_t width,
    uint32_t height,
    uint8_t components_per_pixel,
    PixelType pixel_type
) noexcept -> bool {
    size_t row_bytes = 0;

    if (!calculateRowBytes(
            width,
            components_per_pixel,
            pixel_type,
            row_bytes)) {
        return false;
    }

    size_t data_size = 0;

    if (!checkedMultiply(
            row_bytes,
            static_cast<size_t>(height),
            data_size)) {
        return false;
    }

    auto new_data = std::unique_ptr<uint8_t[]>(
        new (std::nothrow) uint8_t[data_size]);

    if (!new_data) {
        return false;
    }

    // Commit only after validation and allocation succeed.
    owned_data_ = std::move(new_data);
    data_ = owned_data_.get();

    width_ = width;
    height_ = height;
    components_per_pixel_ = components_per_pixel;
    pixel_type_ = pixel_type;
    storage_mode_ = StorageMode::Owned;
    row_stride_ = row_bytes;
    data_size_ = data_size;

    return true;
}

auto Image::wrap(
    uint32_t width,
    uint32_t height,
    uint8_t components_per_pixel,
    PixelType pixel_type,
    uint8_t* data,
    size_t buffer_size,
    size_t row_stride
) noexcept -> bool {
    if (data == nullptr) {
        return false;
    }

    size_t row_bytes = 0;

    if (!calculateRowBytes(
            width,
            components_per_pixel,
            pixel_type,
            row_bytes)) {
        return false;
    }

    if (row_stride == 0) {
        row_stride = row_bytes;
    }

    if (row_stride < row_bytes) {
        return false;
    }

    size_t data_span = 0;

    if (!calculateDataSpan(
            height,
            row_stride,
            row_bytes,
            data_span)) {
        return false;
    }

    if (buffer_size < data_span) {
        return false;
    }

    // Do not invalidate this image if asked to wrap its own buffer.
    if (owned_data_ && data == owned_data_.get()) {
        return false;
    }

    // The external buffer is borrowed, never freed by Image.
    owned_data_.reset();
    data_ = data;

    width_ = width;
    height_ = height;
    components_per_pixel_ = components_per_pixel;
    pixel_type_ = pixel_type;
    storage_mode_ = StorageMode::Borrowed;
    row_stride_ = row_stride;
    data_size_ = data_span;

    return true;
}

void Image::reset() noexcept {
    owned_data_.reset();

    data_ = nullptr;
    width_ = 0;
    height_ = 0;
    components_per_pixel_ = 0;
    pixel_type_ = PixelType::UInt8;
    storage_mode_ = StorageMode::Empty;
    row_stride_ = 0;
    data_size_ = 0;
}


//------------------------------------------------------------------------------
//  State.
//------------------------------------------------------------------------------

auto Image::isUsable() const noexcept -> bool {
    if (data_ == nullptr ||
        storage_mode_ == StorageMode::Empty) {
        return false;
    }

    size_t row_bytes = 0;

    if (!calculateRowBytes(
            width_,
            components_per_pixel_,
            pixel_type_,
            row_bytes)) {
        return false;
    }

    size_t required_span = 0;

    if (!calculateDataSpan(
            height_,
            row_stride_,
            row_bytes,
            required_span)) {
        return false;
    }

    return data_size_ >= required_span;
}

auto Image::ownsPixelData() const noexcept -> bool {
    return storage_mode_ == StorageMode::Owned &&
           owned_data_ != nullptr &&
           data_ == owned_data_.get();
}


//------------------------------------------------------------------------------
//  Properties.
//------------------------------------------------------------------------------


auto Image::bytesPerPixel() const noexcept -> size_t {
    size_t result = 0;

    if (!checkedMultiply(
            bytesPerComponent(pixel_type_),
            static_cast<size_t>(components_per_pixel_),
            result)) {
        return 0;
    }

    return result;
}


//------------------------------------------------------------------------------
//  Pixel Data.
//------------------------------------------------------------------------------

auto Image::rowData(uint32_t y) noexcept -> uint8_t* {
    if (!isUsable() || y >= height_) {
        return nullptr;
    }

    return data_ + (static_cast<size_t>(y) * row_stride_);
}

auto Image::rowData(uint32_t y) const noexcept -> const uint8_t* {
    if (!isUsable() || y >= height_) {
        return nullptr;
    }

    return data_ + (static_cast<size_t>(y) * row_stride_);
}



auto Image::createGraphicContext() noexcept -> std::unique_ptr<GraphicContext> {

}

} // namespace Grain
