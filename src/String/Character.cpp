#include <grain/String/Character.hpp>

namespace Grain {

auto isAsciiDigit(char32_t c) -> bool {
    return c >= U'0' && c <= U'9';
}

auto isAsciiNumericLiteralCharacter(char32_t c) -> bool {
    return isAsciiDigit(c) || c == U'-' || c == U'.';
}

} // namespace Grain