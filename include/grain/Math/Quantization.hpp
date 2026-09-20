#pragma once

#include <cstdint>

namespace Grain::Quantization {

[[nodiscard]]
std::uint32_t quantize(
    std::uint32_t value,
    unsigned sourceBits,
    unsigned targetBits
) noexcept;

} // namespace Grain::Quantization