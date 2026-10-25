
#include <grain/Image/ImageTransfer.hpp>

namespace Grain {

ImageTransfer::ImageTransfer(Image& image) noexcept {
    setup(image);
}

auto ImageTransfer::setup(Image& image) noexcept -> bool {
    if (!access_.setup(image)) {
        return false;
    }

    access_.setTransferPtr(rgba_);
    return true;
}

void ImageTransfer::colorToValues(const Color& color) noexcept {
    rgba_[0] = color.red;
    rgba_[1] = color.green;
    rgba_[2] = color.blue;
    rgba_[3] = color.alpha;
}

void ImageTransfer::valuesToColor(Color& color) const noexcept {
    color.red = rgba_[0];
    color.green = rgba_[1];
    color.blue = rgba_[2];
    color.alpha = rgba_[3];
}

auto ImageTransfer::read(int32_t x, int32_t y, Color& color) noexcept -> bool {
    if (!access_.setPos(x, y) || !access_.read()) {
        return false;
    }

    valuesToColor(color);
    return true;
}

auto ImageTransfer::write(
    int32_t x,
    int32_t y,
    const Color& color) noexcept -> bool {

    if (!access_.setPos(x, y)) {
        return false;
    }

    colorToValues(color);
    return access_.write();
}

auto ImageTransfer::read(Color& color) noexcept -> bool {
    if (!access_.read()) {
        return false;
    }

    valuesToColor(color);
    return true;
}

auto ImageTransfer::write(const Color& color) noexcept -> bool {
    colorToValues(color);
    return access_.write();
}

} // namespace Grain
