
#pragma once

#include <cstdint>

namespace Grain {

class NumberSeries {
public:
    NumberSeries() = delete;

    [[nodiscard]] static auto countBits(
        uint32_t value
    ) -> uint32_t;

    [[nodiscard]] static auto perNoergaardInfiniteNumber(
        uint32_t index
    ) -> int32_t;

    [[nodiscard]] static auto collatzSequenceNumber(
        uint64_t value,
        uint32_t max_steps
    ) -> int32_t;
};

} // namespace Grain
