
#pragma once

#include <cmath>
#include <compare>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace Grain {

class FixValue {
public:
    using RawType = std::int64_t;

    static constexpr RawType kScale = 1'000'000'000;
    static constexpr RawType kMinRaw = -999'999'999'000'000'000LL;
    static constexpr RawType kMaxRaw =  999'999'999'000'000'000LL;
    static constexpr int32_t kDecimalPrecision = 9;
    static constexpr int32_t kMaxStrLength = 21; // Maximum length of str representation incl. EOS


    constexpr FixValue() = default;

    template<typename T>
        requires std::is_integral_v<T>
    constexpr FixValue(T value)
        : raw_(static_cast<RawType>(value) * kScale) {
    }

    template<typename T>
        requires std::is_floating_point_v<T>
    explicit FixValue(T value)
        : raw_(fromDouble(static_cast<double>(value))) {
    }

    explicit FixValue(std::string_view text) {
        set(text);
    }

    [[nodiscard]] static constexpr auto fromRaw(RawType raw) -> FixValue {
        FixValue result;
        result.raw_ = raw;
        return result;
    }

    [[nodiscard]] constexpr auto raw() const -> RawType {
        return raw_;
    }

    constexpr void setRaw(RawType raw) {
        raw_ = raw;
    }

    [[nodiscard]] constexpr auto asInt32() const -> int32_t {
        return static_cast<int32_t>(std::round(asDouble()));
    }

    [[nodiscard]] constexpr auto asInt64() const -> int64_t {
        return static_cast<int64_t>(std::round(asDouble()));
    }

    [[nodiscard]] constexpr auto asDouble() const -> double {
        return static_cast<double>(raw_) / kScale;
    }

    [[nodiscard]] constexpr auto asFloat() const -> float {
        return static_cast<float>(asDouble());
    }

    [[nodiscard]] constexpr auto isZero() const -> bool {
        return raw_ == 0;
    }

    [[nodiscard]] constexpr auto isPositive() const -> bool {
        return raw_ > 0;
    }

    [[nodiscard]] constexpr auto isNegative() const -> bool {
        return raw_ < 0;
    }

    [[nodiscard]] constexpr auto isInt() const -> bool {
        return raw_ % kScale == 0;
    }

    [[nodiscard]] constexpr auto isFloat() const -> bool {
        return raw_ % kScale != 0;
    }

    [[nodiscard]] constexpr explicit operator bool() const {
        return raw_ != 0;
    }

    [[nodiscard]] constexpr auto operator <=> (const FixValue&) const = default;

    [[nodiscard]] constexpr auto operator - () const -> FixValue {
        return fromRaw(-raw_);
    }

    [[nodiscard]] constexpr auto operator + (FixValue other) const -> FixValue {
        return fromRaw(raw_ + other.raw_);
    }

    [[nodiscard]] constexpr auto operator - (FixValue other) const -> FixValue {
        return fromRaw(raw_ - other.raw_);
    }

    [[nodiscard]] auto operator * (FixValue other) const -> FixValue;

    [[nodiscard]] auto operator / (FixValue other) const -> FixValue;

    constexpr auto operator += (FixValue other) -> FixValue& {
        raw_ += other.raw_;
        return *this;
    }

    constexpr auto operator -= (FixValue other) -> FixValue& {
        raw_ -= other.raw_;
        return *this;
    }

    auto operator *= (FixValue other) -> FixValue& {
        return *this = *this * other;
    }

    auto operator /= (FixValue other) -> FixValue& {
        return *this = *this / other;
    }

    [[nodiscard]] constexpr auto abs() const -> FixValue {
        return fromRaw(raw_ < 0 ? -raw_ : raw_);
    }

    [[nodiscard]] constexpr auto floor() const -> FixValue {
        const auto integer = raw_ / kScale;
        const auto remainder = raw_ % kScale;

        return fromRaw((integer - (remainder < 0 ? 1 : 0)) * kScale);
    }

    [[nodiscard]] constexpr auto ceil() const -> FixValue {
        const auto integer = raw_ / kScale;
        const auto remainder = raw_ % kScale;

        return fromRaw((integer + (remainder > 0 ? 1 : 0)) * kScale);
    }

    [[nodiscard]] auto round() const -> FixValue;

    [[nodiscard]] auto sqrt() const -> FixValue;

    auto set(std::string_view text) -> bool;

    auto setDouble(double value) -> bool;

    auto setInt64(int64_t value) -> bool;

    auto set(
        const FixValue& value,
        const FixValue& min,
        const FixValue& max,
        int32_t precision
    ) -> bool {
        FixValue new_value = value;
        new_value.clamp(min, max);
        new_value.setPrecision(precision);
        if (new_value.raw_ != raw_) {
            raw_ = new_value.raw_;
            return true;
        }
        return false;
    }

    void setPrecision(int precision);

    void clamp(FixValue minimum, FixValue maximum);
    void flip(const FixValue& min, const FixValue& max);

    [[nodiscard]] auto toString(int32_t precision = kDecimalPrecision) const -> std::string;

    [[nodiscard]]
    static auto parse(std::string_view text) -> std::optional<FixValue>;

    friend auto operator << (std::ostream& stream, FixValue value) -> std::ostream&;

private:
    [[nodiscard]] static auto fromDouble(double value) -> RawType;

    RawType raw_ = 0;
};

} // namespace Grain
