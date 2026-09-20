#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <type_traits>

namespace Grain::Statistics {

template <typename T>
[[nodiscard]]
T median(T* values, std::size_t length)
{
    if (values == nullptr || length == 0) {
        return T{};
    }

    T* middle = values + length / 2;

    std::nth_element(values, middle, values + length);

    if (length % 2 != 0) {
        return *middle;
    }

    const T upper = *middle;
    const T lower = *std::max_element(values, middle);

    if constexpr (std::is_floating_point_v<T>) {
        return lower + (upper - lower) / T{2};
    } else {
        return (lower + upper) / T{2};
    }
}

template <typename T>
[[nodiscard]]
T sum(const T* values, std::size_t length)
{
    if (values == nullptr || length == 0) {
        return T{};
    }

    if constexpr (std::is_floating_point_v<T>) {
        T result = T{};
        T compensation = T{};

        // Neumaier / Kahan-Babuška compensated summation.
        for (std::size_t i = 0; i < length; ++i) {
            const T value = values[i];
            const T temporary = result + value;

            if (std::abs(result) >= std::abs(value)) {
                compensation +=
                    (result - temporary) + value;
            } else {
                compensation +=
                    (value - temporary) + result;
            }

            result = temporary;
        }

        return result + compensation;
    } else {
        T result = T{};

        for (std::size_t i = 0; i < length; ++i) {
            result += values[i];
        }

        return result;
    }
}

} // namespace Grain::Statistics