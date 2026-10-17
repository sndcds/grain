#pragma once

#include "Color.hpp"

#include <cstdint>
#include <algorithm>

namespace Grain {

class ColorLUT {
public:
    enum Resolution : int32_t {
        Default = 512,
        Min = 2,
        Max = 8192,
    };

protected:
    int32_t resolution_{};
    int32_t max_resolution_{};
    int32_t max_index_{};
    Color* samples_{};

public:
    ColorLUT(int32_t resolution);
    ~ColorLUT();

    int32_t resolution() const { return resolution_; }
    int32_t maxResolution() const { return max_resolution_; }
    int32_t maxIndex() const { return max_index_; }
    const Color* samplesPtr() const { return samples_; }
    Color* mutSamplesPtr() const { return samples_; }

    bool setResolution(int32_t resolution);
    void setColorAtIndex(int32_t index, const Color& color);

    bool shrink();
    void smooth();
    void lookup(float pos, Color& out_color) const;
    void lookup(float pos, float* out_color) const;

    static const ColorLUT *createKelvinLUT(float min_kelvin, float max_kelvin);

    inline static int32_t clampedResolution(int32_t resolution) {
        return std::clamp(
            resolution,
            (int32_t)Resolution::Min,
            (int32_t)Resolution::Max);
    }
};

} // End of namespace Grain
