#pragma once

#include <grain/Image/ImageIO.hpp>

namespace Grain::Detail {

auto loadWebP(
    Image& image,
    const char* filename
) noexcept -> ImageIO::Error;

auto saveWebP(
    const Image& image,
    const char* filename,
    int quality
) noexcept -> ImageIO::Error;

} // namespace Grain::Detail
