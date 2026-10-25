
#pragma once

#include <grain/Image/ImageIO.hpp>

namespace Grain::Detail {

[[nodiscard]] auto loadPNG(
    Image& image,
    const char* filename
) noexcept -> ImageIO::Error;

[[nodiscard]] auto savePNG(
    const Image& image,
    const char* filename
) noexcept -> ImageIO::Error;

} // namespace Grain::Detail
