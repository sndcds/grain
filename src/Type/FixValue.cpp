#include <grain/Type/FixValue.hpp>

#include <algorithm>
#include <ostream>
#include <string>

namespace Grain {

namespace {

using RawType = FixValue::RawType;

// Return the magnitude without overflowing for the minimum signed value.
constexpr auto magnitude(RawType value) -> std::uint64_t {
    if (value >= 0) {
        return static_cast<std::uint64_t>(value);
    }

    return static_cast<std::uint64_t>(-(value + 1)) + 1;
}

constexpr auto clampRaw(__int128 value) -> RawType {
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
constexpr auto roundToMultiple(RawType value, RawType factor) -> RawType {
    if (factor <= 1) {
        return value;
    }

    const RawType quotient = value / factor;
    const RawType remainder = value % factor;

    // The remainder's magnitude is strictly smaller than factor.
    if (magnitude(remainder) < static_cast<std::uint64_t>((factor + 1) / 2)) {
        return quotient * factor;
    }

    const RawType rounded_quotient = quotient + (value < 0 ? -1 : 1);

    return clampRaw(static_cast<__int128>(rounded_quotient) * factor);
}

constexpr auto precisionFactor(int precision) -> RawType {
    RawType factor = 1;

    for (int i = precision; i < FixValue::kDecimalPrecision; ++i) {
        factor *= 10;
    }

    return factor;
}

auto isWhitespace(char c) -> bool {
    return
        c == ' ' ||
        c == '\t' ||
        c == '\n' ||
        c == '\r' ||
        c == '\f' ||
        c == '\v';
}

} // namespace


//------------------------------------------------------------------------------
//  Conversion.
//------------------------------------------------------------------------------

auto FixValue::fromDouble(double value) -> FixValue::RawType {
    if (std::isnan(value)) {
        return 0;
    }

    if (value >= static_cast<double>(kMaxRaw) / static_cast<double>(kScale)) {
        return kMaxRaw;
    }

    if (value <= static_cast<double>(kMinRaw) / static_cast<double>(kScale)) {
        return kMinRaw;
    }

    if (!std::isfinite(value)) {
        return value < 0.0 ? kMinRaw : kMaxRaw;
    }

    const double scaled = value * static_cast<double>(kScale);

    return static_cast<RawType>(std::round(scaled));
}

auto FixValue::setDouble(double value) -> bool {
    if (!std::isfinite(value)) {
        return false;
    }

    raw_ = fromDouble(value);
    return true;
}

auto FixValue::setInt64(int64_t value) -> bool {
    raw_ = clampRaw(static_cast<__int128>(value) * kScale);
    return true;
}

//------------------------------------------------------------------------------
//  Arithmetic.
//------------------------------------------------------------------------------

auto FixValue::operator * (FixValue other) const -> FixValue {
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

auto FixValue::operator / (FixValue other) const -> FixValue {
    if (other.raw_ == 0) {
        return {};
    }

    const __int128 numerator =
        static_cast<__int128>(raw_) * kScale;

    const auto denominator =
        static_cast<__int128>(other.raw_);

    const __int128 half_denominator =
        denominator < 0 ? -denominator / 2 : denominator / 2;

    const __int128 adjusted =
        numerator >= 0
            ? numerator + (denominator > 0 ? half_denominator : -half_denominator)
            : numerator + (denominator > 0 ? -half_denominator : half_denominator);

    return fromRaw(clampRaw(adjusted / denominator));
}


//------------------------------------------------------------------------------
//  Rounding and mathematics.
//------------------------------------------------------------------------------

auto FixValue::round() const -> FixValue {
    return fromRaw(roundToMultiple(raw_, kScale));
}

auto FixValue::sqrt() const -> FixValue {
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


void FixValue::flip(const FixValue& min, const FixValue& max) {
    if (raw_ > max.raw_) {
        raw_ = min.raw_ + (raw_ - max.raw_);
    }
    else if (raw_ < min.raw_) {
        raw_ = max.raw_ - (min.raw_ - raw_);
    }
}


//------------------------------------------------------------------------------
//  Parsing.
//------------------------------------------------------------------------------

auto FixValue::set(std::string_view text) -> bool {
    std::size_t pos = 0;

    while (pos < text.size() && isWhitespace(text[pos])) {
        ++pos;
    }

    bool negative = false;

    if (pos < text.size() && (text[pos] == '+' || text[pos] == '-')) {
        negative = text[pos] == '-';
        ++pos;
    }

    bool has_integer_digits = false;
    bool has_fraction_digits = false;
    bool has_decimal_point = false;

    std::uint64_t integer_part = 0;
    std::uint64_t fractional_part = 0;
    int fractional_digits = 0;
    int next_digit = -1;

    constexpr auto max_integer_part = static_cast<std::uint64_t>(kMaxRaw / kScale);

    for (; pos < text.size(); ++pos) {
        const char c = text[pos];

        if (c >= '0' && c <= '9') {
            const auto digit = static_cast<unsigned>(c - '0');

            if (!has_decimal_point) {
                has_integer_digits = true;

                if (integer_part > (max_integer_part - digit) / 10) {
                    return false;
                }

                integer_part = (integer_part * 10) + digit;
            }
            else {
                has_fraction_digits = true;

                if (fractional_digits < kDecimalPrecision) {
                    fractional_part = (fractional_part * 10) + digit;
                    ++fractional_digits;
                } else if (next_digit < 0) {
                    next_digit = static_cast<int>(digit);
                }
            }

            continue;
        }

        if (c == '.' && !has_decimal_point) {
            has_decimal_point = true;
            continue;
        }

        break;
    }

    if (!has_integer_digits && !has_fraction_digits) {
        return false;
    }

    while (pos < text.size() && isWhitespace(text[pos])) {
        ++pos;
    }

    if (pos != text.size()) {
        return false;
    }

    while (fractional_digits < kDecimalPrecision) {
        fractional_part *= 10;
        ++fractional_digits;
    }

    __int128 result =
        (static_cast<__int128>(integer_part) * kScale) +
        static_cast<__int128>(fractional_part);

    // Round excess fractional digits to nine decimal places.
    if (next_digit >= 5) {
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

auto FixValue::parse(std::string_view text) -> std::optional<FixValue> {
    FixValue result;

    if (!result.set(text)) {
        return std::nullopt;
    }

    return result;
}

//------------------------------------------------------------------------------
//  Formatting.
//------------------------------------------------------------------------------

auto FixValue::toString(int32_t precision) const -> std::string {
    precision = std::clamp(precision, 0, kDecimalPrecision);

    const RawType factor = precisionFactor(precision);
    const RawType rounded_raw = roundToMultiple(raw_, factor);
    const std::uint64_t absolute = magnitude(rounded_raw);

    const std::uint64_t integer_part = absolute / static_cast<std::uint64_t>(kScale);
    const std::uint64_t fractional_part = absolute % static_cast<std::uint64_t>(kScale);

    std::string result;

    if (rounded_raw < 0) {
        result.push_back('-');
    }

    result += std::to_string(integer_part);

    if (precision == 0) {
        return result;
    }

    std::string fraction = std::to_string(fractional_part);
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

auto operator << (std::ostream& stream, FixValue value) -> std::ostream& {
    return stream << value.toString();
}

} // namespace Grain
