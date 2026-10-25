#pragma once

#include <grain/Animation/EaseMode.hpp>


namespace Grain {

class WeightedSamples;
class Bezier;
class LevelCurve;

class ValueLUT {

public:
    enum {
        kMinResolution = 64,
        kMaxResolution = std::numeric_limits<int32_t>::max(),
        kDefaultResolution = 512,
        kMaxbezierResolution = 512,
        kDefaultBezierResolution = 64
    };

    enum Error : int32_t {
    };

private:
    int32_t resolution_ = 0;
    int32_t last_index_ = 0;
    int32_t max_resolution_ = 0;
    float* samples_ = nullptr;
    bool use_extern_mem_ = false;

public:
    ValueLUT(int32_t resolution);
    ValueLUT(int32_t resolution, int32_t max_resolution);
    ValueLUT(int32_t resolution, float* mem);
    ~ValueLUT();

    void _init(int32_t resolution, int32_t max_resolution, float* mem);

    auto setResolution(int32_t resolution) -> Error;

    [[nodiscard]] auto resolution() const -> int32_t { return resolution_; }
    [[nodiscard]] auto samplePtr() const -> const float* { return samples_; }
    [[nodiscard]] auto mutSamplePtr() const -> float* { return samples_; }
    [[nodiscard]] auto samplePtrAtIndex(int32_t index) const -> const float*;
    [[nodiscard]] auto mutSamplePtrAtIndex(int32_t index) const -> float*;

    void clear();
    void clear(float value);
    void scale(float scale);
    void gamma(float e);

    void add(const ValueLUT* lut);
    void mul(const ValueLUT* lut);

    void hanningWindow();
    void window(DSPWindow::WindowType window_type, float alpha, float beta, bool unity_gain);

    void normalize();
    void alignZero();
    void limit(float min, float max);
    void flip();

    void setValueAtIndex(int32_t index, float value);
    void setValuesInIndexRange(int32_t start_index, int32_t end_index, float value);
    void setValuesInRange(float value, float t_left, float t_right);
    void setEaseValuesInRange(EaseMode ease_mode, float t_left, float t_right, float value_left, float value_right);
    void setByBezier(const Bezier& bezier, int32_t bezier_resolution);
    void setByLevelCurve(const LevelCurve& level_curve);
    auto setByWeightedSamples(const WeightedSamples* weighted_samples) -> bool;
    auto setByGaussKernel(double sigma_sqr) -> bool;

    auto interpolateGaps(float* min = nullptr, float* max = nullptr) -> Result;

    void vibrato(int32_t sample_rate, int32_t n, int32_t offset, float amount, float freq, float delay, float attack, float duration, float release);

    [[nodiscard]] auto countAbove(float value) const -> int32_t;

    [[nodiscard]] auto lookup(float t) const -> float;
    [[nodiscard]] auto firstValue() const -> float;
    [[nodiscard]] auto lastValue() const -> float;
    [[nodiscard]] auto valueAtIndex(int32_t index) const -> float;
    [[nodiscard]] auto valueAtRealIndex(float real_index) const -> float;

    [[nodiscard]] auto tToIndex(float t) const -> int32_t {
        return static_cast<int32_t>(round(t * last_index_));
    };
};



class ValueLUTStepper {

private:
    ValueLUT* lut_ = nullptr;
    float pos_ = 0.0f;
    float step_ = 1.0f;
    float duration_ = 10.0f;
    float min_ = 0.0f;
    float max_ = 1.0f;
    bool running_ = true;

public:
    ValueLUTStepper(ValueLUT* lut, float duration, float step = 1);

    void start();
    auto next(float* out_value) -> bool;
    auto next(float step, float* out_value) -> bool;

    void setLUT(ValueLUT* lut);
    void setDuration(float duration);
    void setDurationAndStep(float duration, float step);
    void setRange(float min, float max);
};


} // End of namespace Grain

