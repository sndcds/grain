#include <grain/Type/FixValue.hpp>

#include <algorithm>
#include <ostream>
#include <string>

namespace Grain {

namespace {

using RawType = FixValue::RawType;

// Return the magnitude without overflowing for the minimum signed value.
constexpr std::uint64_t magnitude(RawType value) {
    if (value >= 0) {
        return static_cast<std::uint64_t>(value);
    }

    return static_cast<std::uint64_t>(-(value + 1)) + 1;
}

constexpr RawType clampRaw(__int128 value) {
    if (value < static_cast<__int128>(FixValue::kMinRaw)) {
        return FixValue::kMinRaw;
    }

    if (value > static_cast<__int128>(FixValue::kMaxRaw)) {
        return FixValue::kMaxRaw;
    }

    return static_cast<RawType>(value);
}

// Round to the nearest multiple of factor, with half values rounded away
// from zero. factor must be positive.
constexpr RawType roundToMultiple(RawType value, RawType factor) {
    if (factor <= 1) {
        return value;
    }

    const RawType quotient = value / factor;
    const RawType remainder = value % factor;

    // The remainder's magnitude is strictly smaller than factor.
    if (magnitude(remainder) < static_cast<std::uint64_t>((factor + 1) / 2)) {
        return quotient * factor;
    }

    const RawType roundedQuotient =
        quotient + (value < 0 ? -1 : 1);

    return clampRaw(
        static_cast<__int128>(roundedQuotient) * factor
    );
}

constexpr RawType precisionFactor(int precision) {
    RawType factor = 1;

    for (int i = precision; i < FixValue::kDecimalPrecision; ++i) {
        factor *= 10;
    }

    return factor;
}

bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '\f' || c == '\v';
}

} // namespace

// MARK: - Conversion

FixValue::RawType FixValue::fromDouble(double value) {
    if (std::isnan(value)) {
        return 0;
    }

    if (value >= static_cast<double>(kMaxRaw) /
                     static_cast<double>(kScale)) {
        return kMaxRaw;
    }

    if (value <= static_cast<double>(kMinRaw) /
                     static_cast<double>(kScale)) {
        return kMinRaw;
    }

    if (!std::isfinite(value)) {
        return value < 0.0 ? kMinRaw : kMaxRaw;
    }

    const double scaled = value * static_cast<double>(kScale);

    return static_cast<RawType>(std::round(scaled));
}

bool FixValue::setDouble(double value) {
    if (!std::isfinite(value)) {
        return false;
    }

    raw_ = fromDouble(value);
    return true;
}

bool FixValue::setInt64(int64_t value) {
    raw_ = clampRaw(static_cast<__int128>(value) * kScale);
    return true;
}

// MARK: - Arithmetic

FixValue FixValue::operator*(FixValue other) const {
    const __int128 product =
        static_cast<__int128>(raw_) *
        static_cast<__int128>(other.raw_);

    // Round to the nearest raw unit, with ties away from zero.
    const __int128 halfScale = kScale / 2;
    const __int128 rounded =
        product >= 0
            ? (product + halfScale) / kScale
            : (product - halfScale) / kScale;

    return fromRaw(clampRaw(rounded));
}

FixValue FixValue::operator/(FixValue other) const {
    if (other.raw_ == 0) {
        return {};
    }

    const __int128 numerator =
        static_cast<__int128>(raw_) * kScale;

    const __int128 denominator =
        static_cast<__int128>(other.raw_);

    const __int128 halfDenominator =
        denominator < 0 ? -denominator / 2 : denominator / 2;

    const __int128 adjusted =
        numerator >= 0
            ? numerator + (denominator > 0 ? halfDenominator : -halfDenominator)
            : numerator + (denominator > 0 ? -halfDenominator : halfDenominator);

    return fromRaw(clampRaw(adjusted / denominator));
}

// MARK: - Rounding and mathematics

FixValue FixValue::round() const {
    return fromRaw(roundToMultiple(raw_, kScale));
}

FixValue FixValue::sqrt() const {
    if (raw_ < 0) {
        return {};
    }

    return fromRaw(fromDouble(std::sqrt(asDouble())));
}

void FixValue::setPrecision(int precision) {
    precision = std::clamp(precision, 0, kDecimalPrecision);
    raw_ = roundToMultiple(raw_, precisionFactor(precision));
}

void FixValue::clamp(FixValue minimum, FixValue maximum) {
    if (minimum.raw_ > maximum.raw_) {
        return;
    }

    raw_ = std::clamp(raw_, minimum.raw_, maximum.raw_);
}


void FixValue::flip(const FixValue& min, const FixValue& max) noexcept {
    if (raw_ > max.raw_) {
        raw_ = min.raw_ + (raw_ - max.raw_);
    }
    else if (raw_ < min.raw_) {
        raw_ = max.raw_ - (min.raw_ - raw_);
    }
}


//------------------------------------------------------------------------------
//  Parsing
//------------------------------------------------------------------------------

bool FixValue::set(std::string_view text) {
    std::size_t pos = 0;

    while (pos < text.size() && isWhitespace(text[pos])) {
        ++pos;
    }

    bool negative = false;

    if (pos < text.size() && (text[pos] == '+' || text[pos] == '-')) {
        negative = text[pos] == '-';
        ++pos;
    }

    bool hasIntegerDigits = false;
    bool hasFractionDigits = false;
    bool hasDecimalPoint = false;

    std::uint64_t integerPart = 0;
    std::uint64_t fractionalPart = 0;
    int fractionalDigits = 0;
    int nextDigit = -1;

    constexpr std::uint64_t maxIntegerPart =
        static_cast<std::uint64_t>(kMaxRaw / kScale);

    for (; pos < text.size(); ++pos) {
        const char c = text[pos];

        if (c >= '0' && c <= '9') {
            const unsigned digit = static_cast<unsigned>(c - '0');

            if (!hasDecimalPoint) {
                hasIntegerDigits = true;

                if (integerPart > (maxIntegerPart - digit) / 10) {
                    return false;
                }

                integerPart = integerPart * 10 + digit;
            } else {
                hasFractionDigits = true;

                if (fractionalDigits < kDecimalPrecision) {
                    fractionalPart = fractionalPart * 10 + digit;
                    ++fractionalDigits;
                } else if (nextDigit < 0) {
                    nextDigit = static_cast<int>(digit);
                }
            }

            continue;
        }

        if (c == '.' && !hasDecimalPoint) {
            hasDecimalPoint = true;
            continue;
        }

        break;
    }

    if (!hasIntegerDigits && !hasFractionDigits) {
        return false;
    }

    while (pos < text.size() && isWhitespace(text[pos])) {
        ++pos;
    }

    if (pos != text.size()) {
        return false;
    }

    while (fractionalDigits < kDecimalPrecision) {
        fractionalPart *= 10;
        ++fractionalDigits;
    }

    __int128 result =
        static_cast<__int128>(integerPart) * kScale +
        static_cast<__int128>(fractionalPart);

    // Round excess fractional digits to nine decimal places.
    if (nextDigit >= 5) {
        ++result;
    }

    if (negative) {
        result = -result;
    }

    if (result < kMinRaw || result > kMaxRaw) {
        return false;
    }

    raw_ = static_cast<RawType>(result);
    return true;
}

std::optional<FixValue> FixValue::parse(std::string_view text) {
    FixValue result;

    if (!result.set(text)) {
        return std::nullopt;
    }

    return result;
}

// MARK: - Formatting

std::string FixValue::toString(int32_t precision) const {
    precision = std::clamp(precision, 0, kDecimalPrecision);

    const RawType factor = precisionFactor(precision);
    const RawType roundedRaw = roundToMultiple(raw_, factor);
    const std::uint64_t absolute = magnitude(roundedRaw);

    const std::uint64_t integerPart =
        absolute / static_cast<std::uint64_t>(kScale);

    const std::uint64_t fractionalPart =
        absolute % static_cast<std::uint64_t>(kScale);

    std::string result;

    if (roundedRaw < 0) {
        result.push_back('-');
    }

    result += std::to_string(integerPart);

    if (precision == 0) {
        return result;
    }

    std::string fraction = std::to_string(fractionalPart);
    fraction.insert(0, kDecimalPrecision - fraction.size(), '0');
    fraction.resize(static_cast<std::size_t>(precision));

    while (!fraction.empty() && fraction.back() == '0') {
        fraction.pop_back();
    }

    if (!fraction.empty()) {
        result.push_back('.');
        result += fraction;
    }

    return result;
}

std::ostream& operator<<(std::ostream& stream, FixValue value) {
    return stream << value.toString();
}

} // namespace Grain
