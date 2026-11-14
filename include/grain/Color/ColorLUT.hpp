#pragma once

#include "Color.hpp"

#include <cstdint>
#include <algorithm>

namespace Grain {

class ColorLUT {
public:
    enum Resolution : int16_t {
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

    [[nodiscard]] auto resolution() const -> int32_t { return resolution_; }
    [[nodiscard]] auto maxResolution() const -> int32_t { return max_resolution_; }
    [[nodiscard]] auto maxIndex() const -> int32_t { return max_index_; }
    [[nodiscard]] auto samplesPtr() const -> const Color* { return samples_; }
    [[nodiscard]] auto mutSamplesPtr() const -> Color* { return samples_; }

    auto setResolution(int32_t resolution) -> bool;
    void setColorAtIndex(int32_t index, const Color& color);

    auto shrink() -> bool;
    void smooth();
    void lookup(float pos, Color& out_color) const;
    void lookup(float pos, float* out_color) const;

    static auto createKelvinLUT(float min_kelvin, float max_kelvin) -> const ColorLUT *;

    static auto clampedResolution(int32_t resolution) -> int32_t {
        return std::clamp(
            resolution,
            static_cast<int32_t>(Resolution::Min),
            static_cast<int32_t>(Resolution::Max));
    }
};

} // End of namespace Grain
