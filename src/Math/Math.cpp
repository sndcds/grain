//
// Math.cpp
//

#include <grain/Math/Math.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace Grain::Math {


//------------------------------------------------------------------------------
// Integer / discrete math
//------------------------------------------------------------------------------

int64_t greatestCommonDivisor(int64_t a, int64_t b) noexcept {
    // Work with unsigned values so the Euclidean algorithm itself
    // does not depend on signed overflow.
    using U = std::uint64_t;

    const U ua = a < 0
        ? U{0} - static_cast<U>(a)
        : static_cast<U>(a);

    const U ub = b < 0
        ? U{0} - static_cast<U>(b)
        : static_cast<U>(b);

    U x = ua;
    U y = ub;

    while (y != 0) {
        const U remainder = x % y;
        x = y;
        y = remainder;
    }

    // For normal int64_t inputs the result is representable.
    // INT64_MIN / INT64_MIN is the only problematic mathematical case.
    if (x > static_cast<U>(std::numeric_limits<int64_t>::max())) {
        return std::numeric_limits<int64_t>::max();
    }

    return static_cast<int64_t>(x);
}


int64_t factorial(int32_t n) noexcept {
    if (n < 0)
        return 0;

    int64_t result = 1;

    for (int32_t i = 2; i <= n; ++i) {
        // 20! is the largest factorial representable by int64_t.
        if (result > std::numeric_limits<int64_t>::max() / i)
            return std::numeric_limits<int64_t>::max();

        result *= i;
    }

    return result;
}


int64_t sumN(int32_t n) noexcept {
    if (n <= 0)
        return 0;

    // Calculate n * (n + 1) / 2 without overflowing
    // unnecessarily before the division.
    const int64_t a = n;
    const int64_t b = a + 1;

    if ((a & 1) == 0)
        return (a / 2) * b;

    return a * (b / 2);
}


int64_t roundToNearestPowerOfTwo(double value) noexcept {
    if (!std::isfinite(value) || value <= 0.0)
        return 0;

    const double exponent = std::log2(value);
    const double roundedExponent = std::round(exponent);

    // int64_t can safely represent powers of two up to 2^62.
    if (roundedExponent < 0.0)
        return 1;

    if (roundedExponent >= 63.0)
        return std::numeric_limits<int64_t>::max();

    return static_cast<int64_t>(
        std::ldexp(1.0, static_cast<int>(roundedExponent))
    );
}


int64_t stepsToReachAtOrAfterInt(
    int64_t value,
    int64_t step,
    int64_t target
) noexcept
{
    if (value >= target)
        return 0;

    if (step <= 0)
        return 0;

    const int64_t distance = target - value;

    return distance / step +
        ((distance % step) != 0 ? 1 : 0);
}


int64_t stepsToReachAtOrAfter(
    double value,
    double step,
    double target
) noexcept
{
    if (value >= target)
        return 0;

    if (!std::isfinite(value) ||
        !std::isfinite(step) ||
        !std::isfinite(target) ||
        step <= 0.0)
    {
        return 0;
    }

    const double steps = std::ceil((target - value) / step);

    if (steps <= 0.0)
        return 0;

    if (steps >= static_cast<double>(
            std::numeric_limits<int64_t>::max()))
    {
        return std::numeric_limits<int64_t>::max();
    }

    return static_cast<int64_t>(steps);
}


//------------------------------------------------------------------------------
// Powers of two
//------------------------------------------------------------------------------

int32_t nextLog2(int64_t value) noexcept {
    if (value <= 1)
        return 0;

    int32_t result = 0;

    // We need ceil(log2(value)).
    uint64_t v = static_cast<uint64_t>(value - 1);

    while (v > 0) {
        ++result;
        v >>= 1;
    }

    return result;
}


bool isPowerOfTwo(int64_t value) noexcept {
    return value > 0 &&
           (static_cast<uint64_t>(value) &
            (static_cast<uint64_t>(value) - 1)) == 0;
}


int32_t log2IfPowerOfTwo(int64_t value) noexcept {
    if (!isPowerOfTwo(value))
        return -1;

    int32_t result = 0;
    uint64_t v = static_cast<uint64_t>(value);

    while (v > 1) {
        v >>= 1;
        ++result;
    }

    return result;
}


int64_t nextPowerOfTwo(int64_t value) noexcept {
    if (value <= 1)
        return 1;

    const int32_t exponent = nextLog2(value);

    // 2^63 cannot be represented as a positive int64_t.
    if (exponent >= 63)
        return std::numeric_limits<int64_t>::max();

    return static_cast<int64_t>(
        uint64_t{1} << exponent
    );
}

} // namespace Grain::Math