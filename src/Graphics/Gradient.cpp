#include <grain/Graphics/Gradient.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <new>
#include <utility>
#include <vector>

namespace Grain {
namespace {

constexpr float kPositionMin = 0.0f;
constexpr float kPositionMax = 1.0f;
constexpr float kEpsilon = 1.0e-6f;

[[nodiscard]]
inline float clampPosition(float pos) {
    return std::clamp(pos, kPositionMin, kPositionMax);
}

[[nodiscard]]
inline float clampUnit(float value) {
    return std::clamp(value, 0.0f, 1.0f);
}

// Approximate black-body color, based on the common temperature-to-RGB
// approximation. Temperature is expressed in kelvin.
[[nodiscard]]
Color kelvinToColor(float kelvin) {
    const float temperature = std::clamp(kelvin, 1000.0f, 40000.0f);
    const float t = temperature / 100.0f;

    float red;
    float green;
    float blue;

    if (t <= 66.0f) {
        red = 255.0f;
        green = 99.4708025861f * std::log(t)
              - 161.1195681661f;

        if (t <= 19.0f) {
            blue = 0.0f;
        }
        else {
            blue = 138.5177312231f * std::log(t - 10.0f)
                 - 305.0447927307f;
        }
    }
    else {
        red = 329.698727446f * std::pow(t - 60.0f, -0.1332047592f);
        green = 288.1221695283f * std::pow(t - 60.0f, -0.0755148492f);
        blue = 255.0f;
    }

    return Color(
        clampUnit(red / 255.0f),
        clampUnit(green / 255.0f),
        clampUnit(blue / 255.0f),
        1.0f
    );
}

// HSV hue is expressed in degrees.
[[nodiscard]]
Color hsvToColor(
    float hue,
    float saturation,
    float value
) noexcept {
    hue = std::fmod(hue, 360.0f);

    if (hue < 0.0f) {
        hue += 360.0f;
    }

    saturation = clampUnit(saturation);
    value = clampUnit(value);

    const float chroma = value * saturation;
    const float h = hue / 60.0f;
    const float x = chroma * (1.0f - std::fabs(std::fmod(h, 2.0f) - 1.0f));
    const float m = value - chroma;

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    if (h < 1.0f) {
        r = chroma; g = x;
    }
    else if (h < 2.0f) {
        r = x; g = chroma;
    }
    else if (h < 3.0f) {
        g = chroma; b = x;
    }
    else if (h < 4.0f) {
        g = x; b = chroma;
    }
    else if (h < 5.0f) {
        r = x; b = chroma;
    }
    else {
        r = chroma; b = x;
    }

    return Color(r + m, g + m, b + m, 1.0f);
}

} // namespace


// -----------------------------------------------------------------------------
// GradientStop
// -----------------------------------------------------------------------------

GradientStop::GradientStop(
    float pos,
    const Color& color
)
    : pos_(clampPosition(pos)),
      colors_{color, color},
      two_colored_(false) {
}

GradientStop::GradientStop(
    float pos,
    const Color& color1,
    const Color& color2
)
    : pos_(clampPosition(pos)),
      colors_{color1, color2},
      two_colored_(true) {
}

GradientStop::GradientStop(
    const GradientStop* stop
) {
    if (stop != nullptr) {
        *this = *stop;
    }
}

void GradientStop::init() {
    pos_ = 0.0f;
    colors_[0] = Color::black();
    colors_[1] = Color::black();

    level_curve_values_[0] = 0;
    level_curve_values_[1] = 5000;
    level_curve_values_[2] = 0;
    level_curve_values_[3] = 5000;
    level_curve_values_[4] = 10000;
    level_curve_values_[5] = 10000;

    step_count_ = 0;
    two_colored_ = false;
    blend_mode_ = GradientBlendMode::RGB;

    selected_ = false;
    second_active_ = false;
    remembered_pos_ = 0.0f;
}

bool GradientStop::setPos(float pos) {
    if (!std::isfinite(pos)) {
        return false;
    }

    pos_ = clampPosition(pos);
    return true;
}

bool GradientStop::setColor(int32_t part, const Color& color) {
    if (part < 0 || part > 1) {
        return false;
    }

    colors_[part] = color;

    if (part == 1) {
        two_colored_ = true;
    }

    return true;
}

bool GradientStop::setStepCount(int32_t step_count) {
    if (step_count < 0) {
        return false;
    }

    step_count_ = step_count;
    return true;
}


// -----------------------------------------------------------------------------
// Gradient lifecycle
// -----------------------------------------------------------------------------

Gradient::Gradient() {
    _init();
}

Gradient::Gradient(const Gradient* gradient) {
    _init();

    if (gradient != nullptr) {
        try {
            set(gradient);
        }
        catch (...) {
            stops_.clear();
            color_space_ = ColorSpace::sRGB;
        }
    }
}

Gradient::~Gradient() = default;

void Gradient::_init() {
    color_space_ = ColorSpace::sRGB;
    must_sort_ = false;
}

Gradient* Gradient::createByPreset(Preset preset, bool flip_gradient) {
    auto* gradient = new (std::nothrow) Gradient();

    if (gradient == nullptr) {
        return nullptr;
    }

    try {
        // Compact palettes intended as useful starting points.
        // The exact palette definitions can be expanded independently.
        switch (preset) {
            case Preset::Fast:
                gradient->addStop(0.0f, Color(0.02f, 0.02f, 0.08f));
                gradient->addStop(0.25f, Color(0.05f, 0.25f, 0.75f));
                gradient->addStop(0.50f, Color(0.05f, 0.80f, 0.65f));
                gradient->addStop(0.75f, Color(0.95f, 0.85f, 0.15f));
                gradient->addStop(1.0f, Color(0.95f, 0.15f, 0.05f));
                break;

            case Preset::Blackbody:
                gradient->addStop(0.0f, Color(0.0f, 0.0f, 0.0f));
                gradient->addStop(0.35f, Color(0.7f, 0.0f, 0.0f));
                gradient->addStop(0.65f, Color(1.0f, 0.45f, 0.0f));
                gradient->addStop(0.85f, Color(1.0f, 0.9f, 0.3f));
                gradient->addStop(1.0f, Color::white());
                break;

            case Preset::Viridis:
                gradient->addStop(0.0f, Color(0.267f, 0.005f, 0.329f));
                gradient->addStop(0.25f, Color(0.230f, 0.322f, 0.546f));
                gradient->addStop(0.50f, Color(0.128f, 0.567f, 0.551f));
                gradient->addStop(0.75f, Color(0.369f, 0.789f, 0.383f));
                gradient->addStop(1.0f, Color(0.993f, 0.906f, 0.144f));
                break;

            case Preset::Inferno:
                gradient->addStop(0.0f, Color(0.001f, 0.000f, 0.014f));
                gradient->addStop(0.25f, Color(0.342f, 0.062f, 0.429f));
                gradient->addStop(0.50f, Color(0.735f, 0.216f, 0.330f));
                gradient->addStop(0.75f, Color(0.978f, 0.557f, 0.035f));
                gradient->addStop(1.0f, Color(0.988f, 0.998f, 0.645f));
                break;

            case Preset::Magma:
                gradient->addStop(0.0f, Color(0.001f, 0.000f, 0.014f));
                gradient->addStop(0.25f, Color(0.317f, 0.071f, 0.485f));
                gradient->addStop(0.50f, Color(0.716f, 0.215f, 0.475f));
                gradient->addStop(0.75f, Color(0.987f, 0.536f, 0.382f));
                gradient->addStop(1.0f, Color(0.987f, 0.991f, 0.750f));
                break;

            case Preset::Plasma:
                gradient->addStop(0.0f, Color(0.050f, 0.030f, 0.528f));
                gradient->addStop(0.25f, Color(0.495f, 0.012f, 0.658f));
                gradient->addStop(0.50f, Color(0.798f, 0.280f, 0.470f));
                gradient->addStop(0.75f, Color(0.973f, 0.586f, 0.252f));
                gradient->addStop(1.0f, Color(0.940f, 0.975f, 0.131f));
                break;
        }

        if (flip_gradient) {
            gradient->flip();
        }

        gradient->needsUpdate();
        return gradient;
    }
    catch (...) {
        delete gradient;
        return nullptr;
    }
}

void Gradient::set(const Gradient* gradient) {
    if (gradient == nullptr || gradient == this) {
        return;
    }

    stops_ = gradient->stops_;
    color_space_ = gradient->color_space_;
    needsUpdate();
}

void Gradient::setColorSpace(ColorSpace color_space) {
    if (color_space_ != color_space) {
        color_space_ = color_space;
        needsUpdate();
    }
}

void Gradient::setBlendMode(GradientBlendMode blend_mode) {
    if (blend_mode_ != blend_mode) {
        blend_mode_ = blend_mode;
        needsUpdate();
    }
}


// -----------------------------------------------------------------------------
// Gradient builders
// -----------------------------------------------------------------------------

void Gradient::buildKelvinGradient(
    float k0,
    float k1,
    float saturation,
    float value,
    int32_t resolution
) {
    if (resolution < 2 || !std::isfinite(k0) || !std::isfinite(k1)) {
        return;
    }

    try {
        stops_.clear();
        stops_.reserve(static_cast<std::size_t>(resolution));

        for (int32_t i = 0; i < resolution; ++i) {
            const float t = static_cast<float>(i)
                          / static_cast<float>(resolution - 1);
            const float kelvin = k0 + (k1 - k0) * t;

            Color color = kelvinToColor(kelvin);

            // Apply HSV-like saturation/value adjustments by blending with
            // grayscale and scaling the resulting RGB channels.
            const float grey =
                0.2126f * color.red +
                0.7152f * color.green +
                0.0722f * color.blue;

            color.red = clampUnit((grey + (color.red - grey) * clampUnit(saturation))
                                  * clampUnit(value));
            color.green = clampUnit((grey + (color.green - grey) * clampUnit(saturation))
                                    * clampUnit(value));
            color.blue = clampUnit((grey + (color.blue - grey) * clampUnit(saturation))
                                   * clampUnit(value));

            stops_.emplace_back(t, color);
        }

        needsUpdate();
    }
    catch (...) {
        needsUpdate();
    }
}

void Gradient::buildHueGradient(
    float saturation,
    float value,
    int32_t resolution
) {
    if (resolution < 2) {
        return;
    }

    try {
        stops_.clear();
        stops_.reserve(static_cast<std::size_t>(resolution));

        for (int32_t i = 0; i < resolution; ++i) {
            const float t = static_cast<float>(i)
                          / static_cast<float>(resolution - 1);

            stops_.emplace_back(
                t,
                hsvToColor(t * 360.0f, saturation, value)
            );
        }

        needsUpdate();
    }
    catch (...) {
        needsUpdate();
    }
}


// -----------------------------------------------------------------------------
// Selection
// -----------------------------------------------------------------------------

int32_t Gradient::selectedStopCount() const {
    int32_t count = 0;

    for (const auto& stop : stops_) {
        if (stop.selected_) {
            ++count;
        }
    }

    return count;
}

int32_t Gradient::firstSelectedStopIndex() const {
    for (std::size_t i = 0; i < stops_.size(); ++i) {
        if (stops_[i].selected_) {
            return static_cast<int32_t>(i);
        }
    }

    return -1;
}

int32_t Gradient::lastSelectedStopIndex() const {
    for (std::size_t i = stops_.size(); i > 0; --i) {
        if (stops_[i - 1].selected_) {
            return static_cast<int32_t>(i - 1);
        }
    }

    return -1;
}

int32_t Gradient::selectedStopIndices(
    int32_t* out_first_index,
    int32_t* out_last_index
) const {
    int32_t first = -1;
    int32_t last = -1;
    int32_t count = 0;

    for (std::size_t i = 0; i < stops_.size(); ++i) {
        if (!stops_[i].selected_) {
            continue;
        }

        const int32_t index = static_cast<int32_t>(i);

        if (first < 0) {
            first = index;
        }

        last = index;
        ++count;
    }

    if (out_first_index != nullptr) {
        *out_first_index = first;
    }

    if (out_last_index != nullptr) {
        *out_last_index = last;
    }

    return count;
}

void Gradient::selectAllStops() {
    for (auto& stop : stops_) {
        stop.selected_ = true;
    }
}

void Gradient::deselectAllStops() {
    for (auto& stop : stops_) {
        stop.selected_ = false;
    }
}

void Gradient::revertStopSelection() {
    for (auto& stop : stops_) {
        stop.selected_ = !stop.selected_;
    }
}

void Gradient::selectStopEach(int32_t step) {
    deselectAllStops();

    if (step <= 0) {
        return;
    }

    for (std::size_t i = 0; i < stops_.size(); i += static_cast<std::size_t>(step)) {
        stops_[i].selected_ = true;
    }
}

bool Gradient::stepSelectedStops(int32_t step_count) {
    if (step_count == 0) {
        return false;
    }

    bool changed = false;

    for (auto& stop : stops_) {
        if (!stop.selected_) {
            continue;
        }

        const int64_t next =
            static_cast<int64_t>(stop.step_count_) + step_count;

        if (next < 0 || next > std::numeric_limits<int32_t>::max()) {
            continue;
        }

        stop.step_count_ = static_cast<int32_t>(next);
        changed = true;
    }

    if (changed) {
        needsUpdate();
    }

    return changed;
}


// -----------------------------------------------------------------------------
// Stop management
// -----------------------------------------------------------------------------

void Gradient::removeAllStops() {
    if (stops_.empty()) {
        return;
    }

    stops_.clear();
    needsUpdate();
}

void Gradient::removeStop(int32_t index) {
    if (!canAccessStop(index)) {
        return;
    }

    stops_.erase(stops_.begin() + index);
    needsUpdate();
}

int32_t Gradient::removeSelectedStops() {
    const auto old_size = stops_.size();

    stops_.erase(
        std::remove_if(
            stops_.begin(),
            stops_.end(),
            [](const GradientStop& stop) {
                return stop.selected_;
            }
        ),
        stops_.end()
    );

    const auto removed = old_size - stops_.size();

    if (removed > 0) {
        needsUpdate();
    }

    return static_cast<int32_t>(removed);
}

void Gradient::addStop(float pos, const Color& color) {
    if (!std::isfinite(pos)) {
        return;
    }

    try {
        stops_.emplace_back(clampPosition(pos), color);
        needsUpdate();
    }
    catch (...) {
        // Preserve noexcept API.
    }
}

void Gradient::addStop(float pos, const Color& color1, const Color& color2) {
    if (!std::isfinite(pos)) {
        return;
    }

    try {
        stops_.emplace_back(clampPosition(pos), color1, color2);
        needsUpdate();
    }
    catch (...) {
        // Preserve noexcept API.
    }
}

void Gradient::addStop(GradientStop& stop) {
    try {
        stops_.push_back(stop);
        needsUpdate();
    }
    catch (...) {
        // Preserve noexcept API.
    }
}

GradientStop* Gradient::mutStopPtrAtIndex(int32_t index) {
    if (!canAccessStop(index)) {
        return nullptr;
    }

    return &stops_[static_cast<std::size_t>(index)];
}

const GradientStop* Gradient::stopPtrAtIndex(int32_t index) const {
    if (!canAccessStop(index)) {
        return nullptr;
    }

    return &stops_[static_cast<std::size_t>(index)];
}

void Gradient::colorAtIndex(
    int32_t index,
    int32_t part,
    Color& out_color
) const {
    const auto* stop = stopPtrAtIndex(index);

    if (stop == nullptr || part < 0 || part > 1) {
        out_color = Color::transparent();
        return;
    }

    out_color = stop->color(part);
}

void Gradient::leftColorAtIndex(int32_t index, Color& out_color) const {
    const auto* stop = stopPtrAtIndex(index);

    out_color = stop != nullptr
        ? stop->leftColor()
        : Color::transparent();
}

void Gradient::rightColorAtIndex(int32_t index, Color& out_color) const {
    const auto* stop = stopPtrAtIndex(index);

    out_color = stop != nullptr
        ? stop->rightColor()
        : Color::transparent();
}


// -----------------------------------------------------------------------------
// Color lookup
// -----------------------------------------------------------------------------

bool Gradient::lookupColor(float pos, Color& out_color) {
    if (stops_.empty() || !std::isfinite(pos)) {
        out_color = Color::transparent();
        return false;
    }

    sortStops();
    pos = clampPosition(pos);

    if (stops_.size() == 1 || pos <= stops_.front().pos_) {
        out_color = stops_.front().leftColor();
        return true;
    }

    if (pos >= stops_.back().pos_) {
        out_color = stops_.back().rightColor();
        return true;
    }

    for (std::size_t i = 0; i + 1 < stops_.size(); ++i) {
        const auto& left = stops_[i];
        const auto& right = stops_[i + 1];

        if (pos < left.pos_ || pos > right.pos_) {
            continue;
        }

        const float distance = right.pos_ - left.pos_;

        if (distance <= kEpsilon) {
            out_color = right.leftColor();
            return true;
        }

        float t = (pos - left.pos_) / distance;

        if (left.step_count_ > 0) {
            const float steps = static_cast<float>(left.step_count_);
            t = std::floor(t * steps) / steps;
        }

        // A two-color stop represents a discontinuity at the stop:
        // its left color applies before the stop and its right color after it.
        const Color& start_color = left.rightColor();
        const Color& end_color = right.leftColor();

        switch (blend_mode_) {
        case GradientBlendMode::RGB:
            out_color = Color::lerp(start_color, end_color, clampUnit(t));
            break;
        case GradientBlendMode::Mixbox:
            out_color = Color::mixboxLerp(start_color, end_color, clampUnit(t));
            break;
        }

        return true;
    }

    out_color = stops_.back().rightColor();
    return true;
}

bool Gradient::lookupColorValues(float pos, float* out_values) {
    if (out_values == nullptr) {
        return false;
    }

    Color color;

    if (!lookupColor(pos, color)) {
        return false;
    }

    out_values[0] = color.red;
    out_values[1] = color.green;
    out_values[2] = color.blue;
    out_values[3] = color.alpha;

    return true;
}

bool Gradient::setColorAtIndex(
    int32_t index,
    int32_t part,
    const Color& color
) {
    if (!canAccessStop(index)) {
        return false;
    }

    if (!stops_[static_cast<std::size_t>(index)].setColor(part, color)) {
        return false;
    }

    needsUpdate();
    return true;
}

void Gradient::rememberSelectedStops() {
    for (auto& stop : stops_) {
        if (stop.selected_) {
            stop.remember();
        }
    }
}

bool Gradient::moveSelectedStops(float delta) {
    if (!std::isfinite(delta) || delta == 0.0f || stops_.empty()) {
        return false;
    }

    float min_pos = 1.0f;
    float max_pos = 0.0f;
    bool found = false;

    for (const auto& stop : stops_) {
        if (!stop.selected_) {
            continue;
        }

        found = true;
        min_pos = std::min(min_pos, stop.pos_);
        max_pos = std::max(max_pos, stop.pos_);
    }

    if (!found) {
        return false;
    }

    // Preserve relative spacing between selected stops.
    delta = std::clamp(delta, -min_pos, 1.0f - max_pos);

    if (delta == 0.0f) {
        return false;
    }

    for (auto& stop : stops_) {
        if (stop.selected_) {
            stop.pos_ = clampPosition(stop.pos_ + delta);
        }
    }

    needsUpdate();
    return true;
}

bool Gradient::setColorOfSelectedStops(const Color& color) {
    bool changed = false;

    for (auto& stop : stops_) {
        if (!stop.selected_) {
            continue;
        }

        stop.colors_[stop.second_active_ ? 1 : 0] = color;
        changed = true;
    }

    if (changed) {
        needsUpdate();
    }

    return changed;
}

bool Gradient::setStepCountOfSelectedStops(int32_t step_count) {
    if (step_count < 0) {
        return false;
    }

    bool changed = false;

    for (auto& stop : stops_) {
        if (!stop.selected_) {
            continue;
        }

        stop.step_count_ = step_count;
        changed = true;
    }

    if (changed) {
        needsUpdate();
    }

    return changed;
}

bool Gradient::setColorModeOfSetectedStops(bool two_colored) {
    bool changed = false;

    for (auto& stop : stops_) {
        if (!stop.selected_) {
            continue;
        }

        if (stop.two_colored_ == two_colored) {
            continue;
        }

        if (!two_colored) {
            stop.colors_[1] = stop.colors_[0];
            stop.second_active_ = false;
        }

        stop.two_colored_ = two_colored;
        changed = true;
    }

    if (changed) {
        needsUpdate();
    }

    return changed;
}


// -----------------------------------------------------------------------------
// Gradient editing
// -----------------------------------------------------------------------------

void Gradient::needsUpdate(bool flag) {
    if (!flag) {
        return;
    }

    must_sort_ = true;
}

bool Gradient::reset() {
    if (stops_.empty()) {
        return false;
    }

    for (auto& stop : stops_) {
        stop.selected_ = false;
        stop.second_active_ = false;
        stop.remembered_pos_ = stop.pos_;
    }

    return true;
}

bool Gradient::flip() {
    if (stops_.empty()) {
        return false;
    }

    sortStops();

    const std::size_t count = stops_.size();

    for (auto& stop : stops_) {
        stop.pos_ = 1.0f - stop.pos_;
        std::swap(stop.colors_[0], stop.colors_[1]);
        stop.second_active_ = false;
    }

    std::reverse(stops_.begin(), stops_.end());

    // Preserve single-color stop semantics after swapping color slots.
    for (auto& stop : stops_) {
        if (!stop.two_colored_) {
            stop.colors_[1] = stop.colors_[0];
        }
    }

    (void)count;
    needsUpdate();
    return true;
}

bool Gradient::distribute() {
    if (stops_.size() < 3) {
        return false;
    }

    sortStops();

    const float first = stops_.front().pos_;
    const float last = stops_.back().pos_;
    const float interval = (last - first)
                         / static_cast<float>(stops_.size() - 1);

    for (std::size_t i = 1; i + 1 < stops_.size(); ++i) {
        stops_[i].pos_ = first + interval * static_cast<float>(i);
    }

    needsUpdate();
    return true;
}

bool Gradient::stretch() {
    if (stops_.size() < 2) {
        return false;
    }

    sortStops();

    const float first = stops_.front().pos_;
    const float last = stops_.back().pos_;
    const float range = last - first;

    if (range <= kEpsilon) {
        return false;
    }

    for (auto& stop : stops_) {
        stop.pos_ = (stop.pos_ - first) / range;
    }

    needsUpdate();
    return true;
}

void Gradient::sortStops() {
    if (!must_sort_) {
        return;
    }

    std::stable_sort(
        stops_.begin(),
        stops_.end(),
        [](const GradientStop& a, const GradientStop& b) {
            return a.pos_ < b.pos_;
        }
    );

    must_sort_ = false;
}

} // namespace Grain
