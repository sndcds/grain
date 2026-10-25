
#pragma once

#include <cstdint>

namespace Grain {

class NumberSeries {
public:
    NumberSeries() = delete;

    [[nodiscard]] static auto countBits(
        std::uint32_t value
    ) -> std::uint32_t;

    [[nodiscard]] static auto perNoergaardInfiniteNumber(
        std::uint32_t index
    ) -> std::int32_t;

    [[nodiscard]] static auto collatzSequenceNumber(
        std::uint64_t value,
        std::uint32_t max_steps
    ) -> std::int32_t;
};

} // namespace Grain
