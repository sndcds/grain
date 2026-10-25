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

    explicit Random(Seed seed)
        : engine_(seed) {}


    //--------------------------------------------------------------------------
    // Seeding
    //--------------------------------------------------------------------------

    void seed(Seed value) {
        engine_.seed(value);
        gaussian_.reset();
    }


    //--------------------------------------------------------------------------
    // Integers
    //--------------------------------------------------------------------------

    [[nodiscard]] auto integer(int32_t min, int32_t max) -> int32_t {
        assert(min <= max);
        return std::uniform_int_distribution<int32_t>(min, max)(engine_);
    }

    [[nodiscard]] auto integer(int32_t max) -> int32_t {
        assert(max >= 0);
        return integer(0, max);
    }


    //--------------------------------------------------------------------------
    // Real numbers
    //--------------------------------------------------------------------------

    [[nodiscard]] auto real(float min, float max) -> float {
        assert(min <= max);
        return std::uniform_real_distribution<float>(min, max)(engine_);
    }

    [[nodiscard]] auto real(float max = 1.0f) -> float {
        assert(max >= 0.0f);
        return real(0.0f, max);
    }


    //--------------------------------------------------------------------------
    // Bipolar values
    //--------------------------------------------------------------------------

    [[nodiscard]] auto bipolar(float max = 1.0f) -> float {
        assert(max >= 0.0f);
        return real(-max, max);
    }

    [[nodiscard]] auto bipolarPi() -> float {
        return bipolar(std::numbers::pi_v<float>);
    }


    //--------------------------------------------------------------------------
    // Probability
    //--------------------------------------------------------------------------

    [[nodiscard]] auto chance(float probability = 0.5f) -> bool {
        assert(probability >= 0.0f && probability <= 1.0f);
        return real() < probability;
    }


    //--------------------------------------------------------------------------
    // Gaussian distribution
    //--------------------------------------------------------------------------

    [[nodiscard]] auto gaussian(float mean = 0.0f, float stddev = 1.0f) -> float {
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

    [[nodiscard]] auto byte() -> uint8_t {
        return static_cast<uint8_t>(integer(0, 255));
    }


private:
    std::mt19937 engine_;
    std::normal_distribution<float> gaussian_;
};

} // namespace Grain
