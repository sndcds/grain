#pragma once

#include <concepts>

namespace Grain {

template<typename T>
concept ScalarType =
    std::is_arithmetic_v<T>;

} // namespace Grain