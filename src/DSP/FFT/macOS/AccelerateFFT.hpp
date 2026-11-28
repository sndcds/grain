
#pragma once

#include "../FFTBackend.hpp"

#include <memory>

namespace Grain {

auto createAccelerateFFT(
    std::size_t size,
    std::unique_ptr<FFTBackend>& output
) noexcept -> Result;

} // namespace Grain
