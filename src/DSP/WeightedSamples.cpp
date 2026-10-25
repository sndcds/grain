
#include <grain/DSP/WeightedSamples.hpp>
#include <grain/Math/Vec2.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace Grain {

namespace {

constexpr float kMissingSample = std::numeric_limits<float>::max();

auto isValidResolution(int32_t resolution) -> bool {
    if (resolution < WeightedSamples::kMinResolution) {
        return false;
    }

    const auto max_size = std::numeric_limits<std::size_t>::max();

    return static_cast<std::size_t>(resolution)
        <= max_size / (2 * sizeof(float));
}

} // namespace


WeightedSamples::WeightedSamples(int32_t resolution) {
    if (!isValidResolution(resolution)) {
        last_err_ = kErrResolutionNotAllowed;
        return;
    }

    last_err_ = setResolution(resolution);
}


WeightedSamples::WeightedSamples(
    int32_t resolution,
    float* mem
) {
    if (mem == nullptr || !isValidResolution(resolution)) {
        last_err_ = kErrResolutionNotAllowed;
        return;
    }

    resolution_ = resolution;
    max_resolution_ = resolution;

    samples_ = mem;
    weights_ = mem + resolution;

    use_extern_mem_ = true;
}


WeightedSamples::~WeightedSamples() {
    if (!use_extern_mem_) {
        std::free(samples_);
    }
}


auto WeightedSamples::setResolution(int32_t resolution) -> Error {
    if (!isValidResolution(resolution)) {
        last_err_ = kErrResolutionNotAllowed;
        return last_err_;
    }

    if (resolution == resolution_) {
        last_err_ = kErrNone;
        return last_err_;
    }

    if (use_extern_mem_ && resolution > max_resolution_) {
        last_err_ = kErrResolutionNotAllowed;
        return last_err_;
    }

    if (resolution <= max_resolution_) {
        resolution_ = resolution;
        last_err_ = kErrNone;
        return last_err_;
    }

    const auto old_capacity = max_resolution_;
    const auto new_capacity = resolution;

    const auto new_size =
        static_cast<std::size_t>(new_capacity) * 2 * sizeof(float);

    float* new_samples = nullptr;

    if (samples_ != nullptr) {
        new_samples = static_cast<float*>(
            std::realloc(samples_, new_size)
        );
    }
    else {
        new_samples = static_cast<float*>(
            std::malloc(new_size)
        );
    }

    if (new_samples == nullptr) {
        last_err_ = kErrNoMemory;
        return last_err_;
    }

    if (old_capacity > 0) {
        // Move the old weight array to its new offset.
        std::memmove(
            new_samples + new_capacity,
            new_samples + old_capacity,
            static_cast<std::size_t>(old_capacity) * sizeof(float)
        );
    }
    else {
        std::fill_n(new_samples, new_capacity * 2, 0.0f);
    }

    // Initialize newly added samples and weights.
    if (new_capacity > old_capacity) {
        const auto added = static_cast<std::size_t>(
            new_capacity - old_capacity
        );

        std::fill_n(new_samples + old_capacity, added, 0.0f);
        std::fill_n(new_samples + new_capacity + old_capacity,
                    added, 0.0f);
    }

    samples_ = new_samples;
    weights_ = new_samples + new_capacity;
    resolution_ = resolution;
    max_resolution_ = new_capacity;

    last_err_ = kErrNone;
    return last_err_;
}


void WeightedSamples::clear() {
    if (samples_ == nullptr || resolution_ <= 0) {
        return;
    }

    std::fill_n(samples_, resolution_, 0.0f);
    std::fill_n(weights_, resolution_, 0.0f);
}


void WeightedSamples::setSample(int32_t index, float value) {
    if (samples_ != nullptr && index >= 0 && index < resolution_) {
        samples_[index] = value;
    }
}


auto WeightedSamples::sampleAtIndex(int32_t index) const -> float {
    if (samples_ == nullptr || index < 0 || index >= resolution_) {
        return 0;
    }

    return samples_[index];
}


void WeightedSamples::setWeight(int32_t index,float weight) {
    if (weights_ != nullptr && index >= 0 && index < resolution_) {
        weights_[index] = weight;
    }
}


auto WeightedSamples::weightAtIndex(int32_t index) const -> float {
    if (weights_ == nullptr || index < 0 || index >= resolution_) {
        return 0;
    }

    return weights_[index];
}


void WeightedSamples::addSample(float value, int32_t index, int32_t resolution) {
    if (samples_ == nullptr ||
        weights_ == nullptr ||
        resolution <= 0 ||
        index < 0 ||
        index >= resolution) {
        return;
    }

    const float t = static_cast<float>(index) / static_cast<float>(resolution);

    const float real_index = t * static_cast<float>(resolution_);
    const auto index1 = static_cast<int32_t>(std::floor(real_index));
    const auto index2 = index1 + 1;

    const float f2 = real_index - static_cast<float>(index1);
    const float f1 = 1.0f - f2;

    if (index1 >= 0 && index1 < resolution_) {
        samples_[index1] += f1 * value;
        weights_[index1] += f1;
    }

    if (index2 >= 0 && index2 < resolution_) {
        samples_[index2] += f2 * value;
        weights_[index2] += f2;
    }
}


void WeightedSamples::addWeightedSample(int32_t index, float value, float weight) {
    if (samples_ == nullptr ||
        weights_ == nullptr ||
        index < 0 ||
        index >= resolution_) {
        return;
    }

    samples_[index] += value;
    weights_[index] += weight;
}


void WeightedSamples::minMaxY(float* out_min_y, float* out_max_y) const {
    if (out_min_y == nullptr && out_max_y == nullptr) {
        return;
    }

    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    bool found = false;

    if (samples_ != nullptr) {
        for (int32_t i = 0; i < resolution_; ++i) {
            const float value = samples_[i];

            if (!std::isfinite(value)) {
                continue;
            }

            min_y = std::min(min_y, value);
            max_y = std::max(max_y, value);
            found = true;
        }
    }

    if (!found) {
        min_y = 0.0f;
        max_y = 0.0f;
    }

    if (out_min_y != nullptr) {
        *out_min_y = min_y;
    }

    if (out_max_y != nullptr) {
        *out_max_y = max_y;
    }
}


auto WeightedSamples::lookup(float t) const -> float {
    if (samples_ == nullptr || resolution_ <= 0 || !std::isfinite(t)) {
        return 0;
    }

    if (t <= 0) {
        return samples_[0];
    }

    if (t >= 1) {
        return samples_[resolution_ - 1];
    }

    const float position = t * static_cast<float>(resolution_);

    const auto index0 = static_cast<int32_t>(std::floor(position));
    const auto index1 = index0 + 1;

    if (index1 >= resolution_) {
        return samples_[resolution_ - 1];
    }

    const float fraction = position - static_cast<float>(index0);

    return (samples_[index0] * (1.0f - fraction)) + (samples_[index1] * fraction);
}


void WeightedSamples::addBezier(const Bezier& bezier, int32_t bezier_resolution) {
    if (samples_ == nullptr ||
        weights_ == nullptr ||
        bezier_resolution < 1) {
        return;
    }

    for (int32_t i = 0; i <= bezier_resolution; ++i) {
        const Vec2d sample = bezier.posOnCurve(
            static_cast<double>(i) / static_cast<double>(bezier_resolution)
        );

        if (!std::isfinite(sample.x) || !std::isfinite(sample.y)) {
            continue;
        }

        const float position =
            static_cast<float>(sample.x) *
            static_cast<float>(resolution_);

        // Reject out-of-range values before converting to an integer.
        if (position < 0.0f ||
            position >= static_cast<float>(resolution_)) {
            continue;
        }

        const auto index1 =
            static_cast<int32_t>(std::floor(position));

        const auto index2 = index1 + 1;

        const float f2 = position - static_cast<float>(index1);
        const float f1 = 1.0f - f2;

        if (index1 >= 0 && index1 < resolution_) {
            samples_[index1] += f1 * static_cast<float>(sample.y);
            weights_[index1] += f1;
        }

        if (index2 >= 0 && index2 < resolution_) {
            samples_[index2] += f2 * static_cast<float>(sample.y);
            weights_[index2] += f2;
        }
    }
}


void WeightedSamples::finish() {
    if (samples_ == nullptr ||
        weights_ == nullptr ||
        resolution_ <= 0) {
        return;
    }

    // Normalize samples that have accumulated positive weights.
    for (int32_t i = 0; i < resolution_; ++i) {
        if (weights_[i] > 0 &&
            std::isfinite(weights_[i]) &&
            std::isfinite(samples_[i])) {
            samples_[i] /= weights_[i];
        }
        else {
            samples_[i] = kMissingSample;
        }
    }

    // Find the first valid sample.
    int32_t first_valid = 0;

    while (first_valid < resolution_ &&
           samples_[first_valid] == kMissingSample) {
        ++first_valid;
    }

    // No valid samples: initialize the entire output to zero.
    if (first_valid == resolution_) {
        std::fill_n(samples_, resolution_, 0.0f);
        return;
    }

    // Extend the first valid value to the beginning.
    std::fill_n(samples_, first_valid, samples_[first_valid]);

    int32_t previous = first_valid;

    for (int32_t i = first_valid + 1; i < resolution_; ++i) {
        if (samples_[i] == kMissingSample) {
            continue;
        }

        const int32_t gap = i - previous;

        if (gap > 1) {
            const float start = samples_[previous];
            const float step =
                (samples_[i] - start) / static_cast<float>(gap);

            for (int32_t j = 1; j < gap; ++j) {
                samples_[previous + j] =
                    start + (step * static_cast<float>(j));
            }
        }

        previous = i;
    }

    // Extend the last valid value to the end.
    std::fill(
        samples_ + previous + 1,
        samples_ + resolution_,
        samples_[previous]
    );
}

} // namespace Grain
