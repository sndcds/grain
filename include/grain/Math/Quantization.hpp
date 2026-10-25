#pragma once

#include <cstdint>

namespace Grain::Quantization {

[[nodiscard]] auto quantize(
    std::uint32_t value,
    unsigned source_bits,
    unsigned target_bits
) -> std::uint32_t;

} // namespace Grain::Quantization