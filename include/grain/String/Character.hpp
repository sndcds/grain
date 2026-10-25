#pragma once

#include <cstdint>

namespace Grain {

auto isAsciiDigit(char32_t c) -> bool;
auto isAsciiNumericLiteralCharacter(char32_t c) -> bool;

} // namespace Grain