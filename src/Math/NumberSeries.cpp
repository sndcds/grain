#include <grain/Math/NumberSeries.hpp>

#include <bit>
#include <limits>

namespace Grain {

auto NumberSeries::countBits(
    std::uint32_t value
) -> std::uint32_t {
    return std::bit_width(value);
}

auto NumberSeries::perNoergaardInfiniteNumber(
    std::uint32_t index
) -> std::int32_t {
    std::int32_t result = 0;

    while (index != 0) {
        if ((index & 1U) != 0U) {
            ++result;
        }
        else {
            result = -result;
        }

        index >>= 1U;
    }

    return result;
}

auto NumberSeries::collatzSequenceNumber(
    std::uint64_t value,
    std::uint32_t max_steps
) -> std::int32_t {
    if (value == 0) {
        return -1;
    }

    for (std::uint32_t step = 0; step <= max_steps; ++step) {
        if (value == 1) {
            if (step > static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())) {
                return -1;
            }

            return static_cast<std::int32_t>(step);
        }

        if (step == max_steps) {
            break;
        }

        if ((value & 1U) == 0U) {
            value >>= 1U;
        }
        else {
            constexpr auto max_value =
                std::numeric_limits<std::uint64_t>::max();

            if (value > (max_value - 1U) / 3U) {
                return -1;
            }

            value = value * 3U + 1U;
        }
    }

    return -1;
}

} // namespace Grain
