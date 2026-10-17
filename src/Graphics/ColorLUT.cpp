#include <grain/Graphics/ColorLUT.hpp>

#include <algorithm>
#include <cmath>
#include <new>

namespace Grain {

ColorLUT::ColorLUT(int32_t resolution) {
    setResolution(resolution);
}


ColorLUT::~ColorLUT() {
    delete[] samples_;
}


bool ColorLUT::setResolution(int32_t resolution) {

    resolution = clampedResolution(resolution);

    if (resolution <= max_resolution_) {
        resolution_ = resolution;
        max_index_ = resolution - 1;
        return true;
    }

    Color* new_samples = new (std::nothrow) Color[resolution];

    if (new_samples == nullptr || samples_ == nullptr) {
        return false;
    }

    for (int32_t i = 0; i < resolution_; ++i) {
        new_samples[i] = samples_[i];
    }

    delete[] samples_;

    samples_ = new_samples;
    resolution_ = resolution;
    max_resolution_ = resolution;
    max_index_ = resolution - 1;

    return true;
}


void ColorLUT::setColorAtIndex(int32_t index, const Color& color) {

    if (samples_ && index >= 0 && index < resolution_) {
        samples_[index] = color;
    }
}


bool ColorLUT::shrink() {

    if (resolution_ == 0 || !samples_) {
        return false;
    }

    if (resolution_ == max_resolution_) {
        return true;
    }

    Color* new_samples =
        new (std::nothrow) Color[resolution_];

    if (!new_samples) {
        return false;
    }

    for (int32_t i = 0; i < resolution_; ++i) {
        new_samples[i] = samples_[i];
    }

    delete[] samples_;

    samples_ = new_samples;
    max_resolution_ = resolution_;

    return true;
}


void ColorLUT::smooth() {

    if (!samples_ || resolution_ < 3) {
        return;
    }

    Color previous = samples_[0];

    for (int32_t i = 1; i < resolution_ - 1; ++i) {

        const Color next = samples_[i + 1];
        Color& current = samples_[i];

        current.red =
            (previous.red + current.red + next.red) / 3.0f;

        current.green =
            (previous.green + current.green + next.green) / 3.0f;

        current.blue =
            (previous.blue + current.blue + next.blue) / 3.0f;

        current.alpha =
            (previous.alpha + current.alpha + next.alpha) / 3.0f;

        previous = samples_[i];
    }
}


void ColorLUT::lookup(float pos, Color& out_color) const {

    if (!samples_ || resolution_ == 0) {
        return;
    }

    if (pos <= 0.0f) {
        out_color = samples_[0];
        return;
    }

    if (pos >= 1.0f) {
        out_color = samples_[max_index_];
        return;
    }

    const float sample_pos =
        pos * static_cast<float>(max_index_);

    const int32_t index =
        static_cast<int32_t>(sample_pos);

    const float t = sample_pos - static_cast<float>(index);

    const Color& c1 = samples_[index];
    const Color& c2 = samples_[index + 1];

    out_color.red = c1.red + (c2.red - c1.red) * t;
    out_color.green = c1.green + (c2.green - c1.green) * t;
    out_color.blue = c1.blue + (c2.blue - c1.blue) * t;
    out_color.alpha = c1.alpha + (c2.alpha - c1.alpha) * t;
}


void ColorLUT::lookup(float pos, float* out_color) const {

    if (!out_color) {
        return;
    }

    Color color;
    lookup(pos, color);

    out_color[0] = color.red;
    out_color[1] = color.green;
    out_color[2] = color.blue;
    out_color[3] = color.alpha;
}


const ColorLUT* ColorLUT::createKelvinLUT(float min_kelvin, float max_kelvin) {

    constexpr int32_t kResolution = 128;

    if (!std::isfinite(min_kelvin) ||
        !std::isfinite(max_kelvin) ||
        min_kelvin < 1000.0f ||
        max_kelvin > 40000.0f ||
        min_kelvin >= max_kelvin) {
        return nullptr;
        }

    auto* lut = new (std::nothrow) ColorLUT(kResolution);

    if (!lut || lut->resolution() != kResolution) {
        delete lut;
        return nullptr;
    }

    for (int32_t i = 0; i < kResolution; ++i) {
        const float t =
            static_cast<float>(i) /
            static_cast<float>(kResolution - 1);

        const float kelvin =
            min_kelvin + (max_kelvin - min_kelvin) * t;

        const float temperature = kelvin / 100.0f;

        // Keep your existing black-body RGB calculation here,
        // starting with:
        float red;
        float green;
        float blue;

        // Approximation of black-body color temperature.
        if (temperature <= 66.0f) {
            red = 255.0f;

            green = 99.4708025861f * std::log(temperature)
                    - 161.1195681661f;

            if (temperature <= 19.0f) {
                blue = 0.0f;
            }
            else {
                blue = 138.5177312231f *
                       std::log(temperature - 10.0f)
                       - 305.0447927307f;
            }
        }
        else {
            red = 329.698727446f *
                  std::pow(temperature - 60.0f, -0.1332047592f);

            green = 288.1221695283f *
                    std::pow(temperature - 60.0f, -0.0755148492f);

            blue = 255.0f;
        }

        lut->setColorAtIndex(i, Color(red, green, blue));
    }

    return lut;
}

} // namespace Grain