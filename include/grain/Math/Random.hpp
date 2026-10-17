#pragma once

#include <cassert>
#include <cstdint>
#include <numbers>
#include <random>

namespace Grain {

class Random {
public:
    using Seed = std::mt19937::result_type;

    //--------------------------------------------------------------------------
    // Construction
    //--------------------------------------------------------------------------

    Random()
        : engine_(std::random_device{}()) {}

    explicit Random(Seed seed) noexcept
        : engine_(seed) {}


    //--------------------------------------------------------------------------
    // Seeding
    //--------------------------------------------------------------------------

    void seed(Seed value) noexcept {
        engine_.seed(value);
        gaussian_.reset();
    }


    //--------------------------------------------------------------------------
    // Integers
    //--------------------------------------------------------------------------

    [[nodiscard]]
    int32_t integer(int32_t min, int32_t max) {
        assert(min <= max);

        return std::uniform_int_distribution<int32_t>(
            min, max
        )(engine_);
    }

    [[nodiscard]]
    int32_t integer(int32_t max) {
        assert(max >= 0);

        return integer(0, max);
    }


    //--------------------------------------------------------------------------
    // Real numbers
    //--------------------------------------------------------------------------

    [[nodiscard]]
    float real(float min, float max) {
        assert(min <= max);

        return std::uniform_real_distribution<float>(
            min, max
        )(engine_);
    }

    [[nodiscard]]
    float real(float max = 1.0f) {
        assert(max >= 0.0f);

        return real(0.0f, max);
    }


    //--------------------------------------------------------------------------
    // Bipolar values
    //--------------------------------------------------------------------------

    [[nodiscard]]
    float bipolar(float max = 1.0f) {
        assert(max >= 0.0f);

        return real(-max, max);
    }

    [[nodiscard]]
    float bipolarPi() {
        return bipolar(std::numbers::pi_v<float>);
    }


    //--------------------------------------------------------------------------
    // Probability
    //--------------------------------------------------------------------------

    [[nodiscard]]
    bool chance(float probability = 0.5f) {
        assert(probability >= 0.0f && probability <= 1.0f);

        return real() < probability;
    }


    //--------------------------------------------------------------------------
    // Gaussian distribution
    //--------------------------------------------------------------------------

    [[nodiscard]]
    float gaussian(float mean = 0.0f,
                   float stddev = 1.0f) {
        assert(stddev > 0.0f);

        return gaussian_(
            engine_,
            std::normal_distribution<float>::param_type(
                mean, stddev
            )
        );
    }


    //--------------------------------------------------------------------------
    // Bytes
    //--------------------------------------------------------------------------

    [[nodiscard]]
    uint8_t byte() {
        return static_cast<uint8_t>(integer(0, 255));
    }


private:
    std::mt19937 engine_;
    std::normal_distribution<float> gaussian_;
};

} // namespace Grain
