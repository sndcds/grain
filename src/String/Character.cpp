#include <grain/String/Character.hpp>

namespace Grain {

bool isAsciiDigit(char32_t c) noexcept {
    return c >= U'0' && c <= U'9';
}

bool isAsciiNumericLiteralCharacter(char32_t c) noexcept {
    return isAsciiDigit(c) || c == U'-' || c == U'.';
}

} // namespace Grain