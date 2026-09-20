#include <grain/Math/Quantization.hpp>

#include <limits>

namespace Grain::Quantization {

std::uint32_t quantize(
    const std::uint32_t value,
    const unsigned sourceBits,
    const unsigned targetBits
) noexcept
{
    if (targetBits >= sourceBits) {
        return value;
    }

    if (targetBits == 0) {
        return 0;
    }

    if (sourceBits == 0) {
        return 0;
    }

    if (sourceBits >= 32) {
        const std::uint64_t sourceMax =
            std::numeric_limits<std::uint32_t>::max();

        const std::uint64_t targetMax =
            (std::uint64_t{1} << targetBits) - 1;

        return static_cast<std::uint32_t>(
            (static_cast<std::uint64_t>(value) * targetMax + sourceMax / 2)
            / sourceMax
        );
    }

    const std::uint32_t sourceMax =
        (std::uint32_t{1} << sourceBits) - 1;

    const std::uint32_t targetMax =
        (std::uint32_t{1} << targetBits) - 1;

    return static_cast<std::uint32_t>(
        (static_cast<std::uint64_t>(value) * targetMax + sourceMax / 2)
        / sourceMax
    );
}

} // namespace Grain::Quantization