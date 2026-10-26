#pragma once

#include <grain/Image/ImageIO.hpp>

namespace Grain::Detail {

auto loadJPEG(Image& image, const char* filename) noexcept
    -> ImageIO::Error;

auto saveJPEG(
    const Image& image,
    const char* filename,
    int quality
) noexcept -> ImageIO::Error;

} // namespace Grain::Detail