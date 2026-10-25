#include <grain/Math/Quantization.hpp>

#include <limits>

namespace Grain::Quantization {

auto quantize(
    const std::uint32_t value,
    const unsigned source_bits,
    const unsigned target_bits
) -> std::uint32_t {
    if (target_bits >= source_bits) {
        return value;
    }

    if (target_bits == 0) {
        return 0;
    }

    if (source_bits == 0) {
        return 0;
    }

    if (source_bits >= 32) {
        const std::uint64_t source_max = std::numeric_limits<std::uint32_t>::max();
        const std::uint64_t target_max = (std::uint64_t{1} << target_bits) - 1;

        return static_cast<std::uint32_t>(
            (static_cast<std::uint64_t>(value) * target_max + source_max / 2)
            / source_max
        );
    }

    const std::uint32_t source_max = (std::uint32_t{1} << source_bits) - 1;
    const std::uint32_t target_max = (std::uint32_t{1} << target_bits) - 1;

    return static_cast<std::uint32_t>(
        (static_cast<std::uint64_t>(value) * target_max + source_max / 2)
        / source_max
    );
}

} // namespace Grain::Quantization