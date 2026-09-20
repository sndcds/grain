#include <grain/Math/Interpolation.hpp>

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>

namespace Grain::Interpolation {

namespace {

double cubicImpl(
    double p0,
    double p1,
    double p2,
    double p3,
    double t
) noexcept
{
    // Catmull-Rom spline, tension = 0.5.
    //
    // This is equivalent to:
    //
    // 0.5 * (
    //     2*p1 +
    //     (-p0 + p2) * t +
    //     (2*p0 - 5*p1 + 4*p2 - p3) * t^2 +
    //     (-p0 + 3*p1 - 3*p2 + p3) * t^3
    // )

    const double a = 2.0 * p1;
    const double b = -p0 + p2;
    const double c = 2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3;
    const double d = -p0 + 3.0 * p1 - 3.0 * p2 + p3;

    return 0.5 * (((d * t + c) * t + b) * t + a);
}

} // namespace

double cubic(
    double p0,
    double p1,
    double p2,
    double p3,
    double t
) noexcept
{
    return cubicImpl(p0, p1, p2, p3, t);
}

double bicubic(
    std::span<const double, 16> samples,
    double tx,
    double ty
) noexcept
{
    std::array<double, 4> rows{};

    for (std::size_t y = 0; y < 4; ++y) {
        const std::size_t offset = y * 4;

        rows[y] = cubicImpl(
            samples[offset + 0],
            samples[offset + 1],
            samples[offset + 2],
            samples[offset + 3],
            tx
        );
    }

    return cubicImpl(
        rows[0],
        rows[1],
        rows[2],
        rows[3],
        ty
    );
}

double tricubic(
    std::span<const double, 64> samples,
    double tx,
    double ty,
    double tz
) noexcept
{
    std::array<double, 16> xy{};

    for (std::size_t z = 0; z < 4; ++z) {
        const std::size_t zOffset = z * 16;

        for (std::size_t y = 0; y < 4; ++y) {
            const std::size_t offset = zOffset + y * 4;

            xy[z * 4 + y] = cubicImpl(
                samples[offset + 0],
                samples[offset + 1],
                samples[offset + 2],
                samples[offset + 3],
                tx
            );
        }
    }

    std::array<double, 4> zValues{};

    for (std::size_t z = 0; z < 4; ++z) {
        const std::size_t offset = z * 4;

        zValues[z] = cubicImpl(
            xy[offset + 0],
            xy[offset + 1],
            xy[offset + 2],
            xy[offset + 3],
            ty
        );
    }

    return cubicImpl(
        zValues[0],
        zValues[1],
        zValues[2],
        zValues[3],
        tz
    );
}

double nCubic(
    std::span<const double> samples,
    std::span<const double> coordinates
) noexcept
{
    if (coordinates.empty()) {
        return samples.empty() ? 0.0 : samples.front();
    }

    /*
     * There are four samples per dimension.
     *
     * For N dimensions:
     *
     *     sampleCount = 4^N
     *
     * We recursively reduce one dimension at a time.
     *
     * The first dimension varies fastest in the flat sample
     * array.
     */
    std::size_t expectedSamples = 1;

    for (std::size_t i = 0; i < coordinates.size(); ++i) {
        if (expectedSamples > samples.size() / 4) {
            return 0.0;
        }

        expectedSamples *= 4;
    }

    if (samples.size() != expectedSamples) {
        return 0.0;
    }

    auto interpolateDimension =
        [&](auto&& self,
            std::span<const double> values,
            std::size_t dimension) noexcept -> double
    {
        if (dimension == coordinates.size()) {
            return values.front();
        }

        const std::size_t blockSize = values.size() / 4;

        std::array<double, 4> interpolated{};

        for (std::size_t i = 0; i < 4; ++i) {
            interpolated[i] = self(
                self,
                values.subspan(i * blockSize, blockSize),
                dimension + 1
            );
        }

        return cubicImpl(
            interpolated[0],
            interpolated[1],
            interpolated[2],
            interpolated[3],
            coordinates[dimension]
        );
    };

    return interpolateDimension(
        interpolateDimension,
        samples,
        0
    );
}

} // namespace Grain::Interpolation