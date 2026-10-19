
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
        : raw_(static_cast<RawType>(value) * kScale) {}

    template<typename T>
        requires std::is_floating_point_v<T>
    explicit FixValue(T value)
        : raw_(fromDouble(static_cast<double>(value))) {}

    explicit FixValue(std::string_view text) {
        set(text);
    }

    [[nodiscard]]
    static constexpr FixValue fromRaw(RawType raw) {
        FixValue result;
        result.raw_ = raw;
        return result;
    }

    [[nodiscard]]
    constexpr RawType raw() const {
        return raw_;
    }

    constexpr void setRaw(RawType raw) {
        raw_ = raw;
    }

    [[nodiscard]]
    constexpr int64_t asInt32() const {
        return static_cast<int32_t>(std::round(asDouble()));
    }

    [[nodiscard]]
    constexpr int64_t asInt64() const {
        return static_cast<int64_t>(std::round(asDouble()));
    }

    [[nodiscard]]
    constexpr double asDouble() const {
        return static_cast<double>(raw_) / kScale;
    }

    [[nodiscard]]
    constexpr float asFloat() const {
        return static_cast<float>(asDouble());
    }

    [[nodiscard]]
    constexpr bool isZero() const {
        return raw_ == 0;
    }

    [[nodiscard]]
    constexpr bool isPositive() const {
        return raw_ > 0;
    }

    [[nodiscard]]
    constexpr bool isNegative() const {
        return raw_ < 0;
    }

    [[nodiscard]]
    constexpr bool isInt() const {
        return raw_ % kScale == 0;
    }

    [[nodiscard]]
    constexpr bool isFloat() const {
        return raw_ % kScale != 0;
    }

    [[nodiscard]]
    constexpr explicit operator bool() const {
        return raw_ != 0;
    }

    [[nodiscard]]
    constexpr auto operator<=>(const FixValue&) const = default;

    [[nodiscard]]
    constexpr FixValue operator-() const {
        return fromRaw(-raw_);
    }

    [[nodiscard]]
    constexpr FixValue operator+(FixValue other) const {
        return fromRaw(raw_ + other.raw_);
    }

    [[nodiscard]]
    constexpr FixValue operator-(FixValue other) const {
        return fromRaw(raw_ - other.raw_);
    }

    [[nodiscard]]
    FixValue operator*(FixValue other) const;

    [[nodiscard]]
    FixValue operator/(FixValue other) const;

    constexpr FixValue& operator+=(FixValue other) {
        raw_ += other.raw_;
        return *this;
    }

    constexpr FixValue& operator-=(FixValue other) {
        raw_ -= other.raw_;
        return *this;
    }

    FixValue& operator*=(FixValue other) {
        return *this = *this * other;
    }

    FixValue& operator/=(FixValue other) {
        return *this = *this / other;
    }

    [[nodiscard]]
    constexpr FixValue abs() const {
        return fromRaw(raw_ < 0 ? -raw_ : raw_);
    }

    [[nodiscard]]
    constexpr FixValue floor() const {
        const auto integer = raw_ / kScale;
        const auto remainder = raw_ % kScale;

        return fromRaw((integer - (remainder < 0 ? 1 : 0)) * kScale);
    }

    [[nodiscard]]
    constexpr FixValue ceil() const {
        const auto integer = raw_ / kScale;
        const auto remainder = raw_ % kScale;

        return fromRaw((integer + (remainder > 0 ? 1 : 0)) * kScale);
    }

    [[nodiscard]] FixValue round() const;

    [[nodiscard]] FixValue sqrt() const;

    bool set(std::string_view text);

    bool setDouble(double value);

    bool setInt64(int64_t value);

    bool set(
        const FixValue& value,
        const FixValue& min,
        const FixValue& max,
        int32_t precision
    ) noexcept {
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
    void flip(const FixValue& min, const FixValue& max) noexcept;

    [[nodiscard]] std::string toString(int32_t precision = kDecimalPrecision) const;

    [[nodiscard]]
    static std::optional<FixValue> parse(std::string_view text);

    friend std::ostream& operator<<(std::ostream& stream, FixValue value);

private:
    [[nodiscard]] static RawType fromDouble(double value);

    RawType raw_ = 0;
};

} // namespace Grain
