
#pragma once

#include "Grain.hpp"

#include <cstdint>
#include <limits>

namespace Grain {

class Bezier;

class WeightedSamples {
public:
    static constexpr int32_t kMinResolution = 2;
    static constexpr int32_t kMaxResolution =
        std::numeric_limits<int32_t>::max();

    enum Error : int8_t {
        kErrNone = 0,
        kErrResolutionNotAllowed,
        kErrNoMemory
    };

private:
    int32_t resolution_ = 0;
    int32_t max_resolution_ = 0;

    float* samples_ = nullptr;
    float* weights_ = nullptr;

    bool use_extern_mem_ = false;
    Error last_err_ = kErrNone;

public:
    explicit WeightedSamples(int32_t resolution);
    WeightedSamples(int32_t resolution, float* mem);

    ~WeightedSamples();

    WeightedSamples(const WeightedSamples&) = delete;
    auto operator=(const WeightedSamples&) -> WeightedSamples& = delete;
    WeightedSamples(WeightedSamples&&) = delete;
    auto operator=(WeightedSamples&&) -> WeightedSamples& = delete;

    [[nodiscard]] auto resolution() const -> int32_t { return resolution_; }
    [[nodiscard]] auto maxResolution() const -> int32_t { return max_resolution_; }

    [[nodiscard]] auto lastError() const -> Error { return last_err_; }

    [[nodiscard]] auto mutSamplesPtr() -> float* { return samples_; }
    [[nodiscard]] auto samplesPtr() const -> const float* { return samples_; }
    [[nodiscard]] auto weightsPtr() const -> const float* { return weights_; }

    [[nodiscard]] auto sampleAtIndex(int32_t index) const -> float;
    [[nodiscard]] auto weightAtIndex(int32_t index) const -> float;

    void minMaxY(float* out_min_y, float* out_max_y) const;

    void clear();

    [[nodiscard]] auto setResolution(int32_t resolution) -> Error;

    void setSample(int32_t index, float value);
    void setWeight(int32_t index, float weight);

    void addSample(float value, int32_t index, int32_t resolution);
    void addWeightedSample(int32_t index, float value, float weight);
    void addBezier(const Bezier& bezier, int32_t bezier_resolution);

    void finish();

    [[nodiscard]] auto lookup(float t) const -> float;
};

} // namespace Grain
