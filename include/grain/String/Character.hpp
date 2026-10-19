#pragma once

#include <cstdint>

namespace Grain {

bool isAsciiDigit(char32_t c) noexcept;
bool isAsciiNumericLiteralCharacter(char32_t c) noexcept;

} // namespace Grain