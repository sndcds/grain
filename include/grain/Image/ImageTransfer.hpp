
#pragma once

#include <cstdint>

#include <grain/Graphics/Color.hpp>
#include <grain/Image/ImageAccess.hpp>

namespace Grain {

class ImageTransfer {
public:
    ImageTransfer() = default;
    explicit ImageTransfer(Image& image) noexcept;

    ImageTransfer(const ImageTransfer&) = delete;
    auto operator=(const ImageTransfer&) -> ImageTransfer& = delete;

    [[nodiscard]] auto setup(Image& image) noexcept -> bool;

    [[nodiscard]] auto isUsable() const noexcept -> bool {
        return access_.isUsable();
    }

    [[nodiscard]] auto access() noexcept -> ImageAccess& {
        return access_;
    }

    [[nodiscard]] auto read(int32_t x, int32_t y, Color& color) noexcept -> bool;
    [[nodiscard]] auto write(int32_t x, int32_t y, const Color& color) noexcept -> bool;

    [[nodiscard]] auto read(Color& color) noexcept -> bool;
    [[nodiscard]] auto write(const Color& color) noexcept -> bool;

private:
    ImageAccess access_;
    float rgba_[4] = {0.0f, 0.0f, 0.0f, 1.0f};

    void colorToValues(const Color& color) noexcept;
    void valuesToColor(Color& color) const noexcept;
};

} // namespace Grain
