#pragma once

#include <grain/Graphics/Color.hpp>

#include <cstdint>
#include <utility>
#include <vector>

namespace Grain {

class ColorLUT;

enum class GradientBlendMode : int8_t {
    RGB = 0,
    Mixbox
};


class GradientStop {
    friend class Gradient;
    friend class GradientControl;

public:
    GradientStop() = default;

    GradientStop(float pos, const Color& color);

    GradientStop(float pos, const Color& color1, const Color& color2);

    GradientStop(const GradientStop* stop);

    ~GradientStop() = default;

    auto operator < (const GradientStop& other) const -> bool {
        return pos_ < other.pos_;
    }

    auto operator == (const GradientStop& other) const -> bool {
        return pos_ == other.pos_;
    }

    auto operator != (const GradientStop& other) const -> bool {
        return !(*this == other);
    }

    auto operator > (const GradientStop& other) const -> bool {
        return pos_ > other.pos_;
    }

    auto operator >= (const GradientStop& other) const -> bool {
        return pos_ >= other.pos_;
    }

    auto operator <= (const GradientStop& other) const -> bool {
        return pos_ <= other.pos_;
    }

    void init();

    [[nodiscard]] auto pos() const -> float { return pos_; }

    [[nodiscard]] auto color(int32_t part) const -> Color {
        return part >= 0 && part <= 1
            ? colors_[part]
            : Color(0, 0, 0, 0);
    }
    [[nodiscard]] auto leftColor() const -> Color { return colors_[0]; }
    [[nodiscard]] auto rightColor() const -> Color {
        return two_colored_ ? colors_[1] : colors_[0];
    }

    [[nodiscard]] auto isSelected() const -> bool { return selected_; }
    [[nodiscard]] auto isTwoColored() const -> bool { return two_colored_; }
    [[nodiscard]] auto isSecondActive() const -> bool { return second_active_; }

    [[nodiscard]] auto stepCount() const -> int32_t { return step_count_; }
    [[nodiscard]] auto blendMode() const -> GradientBlendMode { return blend_mode_; }

    [[nodiscard]] auto rememberedPos() const -> float { return remembered_pos_; }

    auto setPos(float pos) -> bool;

    auto setColor(int32_t part, const Color& color) -> bool;

    auto setStepCount(int32_t step_count) -> bool;

    void setBlendMode(GradientBlendMode blend_mode) {
        blend_mode_ = blend_mode;
    }

    void select() { selected_ = true; }
    void deselect() { selected_ = false; }
    void toggleSelection() { selected_ = !selected_; }

    void remember() {
        remembered_pos_ = pos_;
    }

protected:
    float pos_ = 0.0f;

    std::array<Color, 2> colors_ = {{
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};

    std::array<int32_t, 6> level_curve_values_ = {
        0, 5000, 0, 5000, 10000, 10000
    };


    int32_t step_count_ = 0;
    bool two_colored_ = false;
    GradientBlendMode blend_mode_ = GradientBlendMode::RGB;

    // Editor state; retained for compatibility with GradientControl.
    bool selected_ = false;
    bool second_active_ = false;
    float remembered_pos_ = 0.0f;
};


class Gradient {
    friend class GradientStop;
    friend class GradientControl;

public:
    enum class ColorSpace : int8_t {
        sRGB = 0,
        LinearRGB
    };

    enum class Preset : int8_t {
        Fast = 0,
        Blackbody,
        Viridis,
        Inferno,
        Magma,
        Plasma,

        First = 0,
        Last = Plasma
    };

    Gradient();
    Gradient(const Gradient* gradient);
    ~Gradient();

    Gradient(const Gradient&) = delete;
    auto operator=(const Gradient&) -> Gradient& = delete;

    static auto createByPreset(Preset preset, bool flip = false) -> Gradient*;

    void set(const Gradient* gradient);

    [[nodiscard]] auto colorSpace() const -> ColorSpace {
        return color_space_;
    }

    void setColorSpace(ColorSpace color_space);

    [[nodiscard]] auto blendMode() const -> GradientBlendMode {
        return blend_mode_;
    }

    void setBlendMode(GradientBlendMode blend_mode);

    void buildKelvinGradient(
        float k0,
        float k1,
        float s,
        float v,
        int32_t resolution = 16
    );

    void buildHueGradient(
        float s,
        float v,
        int32_t resolution = 16
    );

    [[nodiscard]] auto hasStops() const -> bool {
        return !stops_.empty();
    }

    [[nodiscard]] auto canAccessStop(int32_t index) const -> bool {
        return index >= 0 && std::cmp_less(index ,stops_.size());
    }

    [[nodiscard]] auto stopCount() const -> int32_t {
        return static_cast<int32_t>(stops_.size());
    }

    [[nodiscard]] auto lastStopIndex() const -> int32_t {
        return stopCount() - 1;
    }

    [[nodiscard]] auto selectedStopCount() const -> int32_t;

    [[nodiscard]] auto firstSelectedStopIndex() const -> int32_t;

    [[nodiscard]] auto lastSelectedStopIndex() const -> int32_t;

    auto selectedStopIndices(
        int32_t* out_first_index,
        int32_t* out_last_index
    ) const -> int32_t;

    void selectAllStops();
    void deselectAllStops();
    void revertStopSelection();
    void selectStopEach(int32_t step);

    auto stepSelectedStops(int32_t step_count) -> bool;

    void removeAllStops();
    void removeStop(int32_t index);
    auto removeSelectedStops() -> int32_t;

    void addStop(float pos, const Color& color);

    void addStop(float pos, const Color& color1, const Color& color2);

    void addStop(GradientStop& stop);

    [[nodiscard]] auto mutStopPtrAtIndex(int32_t index) -> GradientStop*;

    [[nodiscard]] auto stopPtrAtIndex(int32_t index) const -> const GradientStop*;

    void colorAtIndex(int32_t index, int32_t part, Color& out_color) const;

    void leftColorAtIndex(int32_t index, Color& out_color) const;

    void rightColorAtIndex(int32_t index, Color& out_color) const;

    auto lookupColor(float pos, Color& out_color) -> bool;

    auto lookupColorValues(float pos, float* out_values) -> bool;

    auto setColorAtIndex(int32_t index, int32_t part, const Color& color) -> bool;

    void rememberSelectedStops();

    auto moveSelectedStops(float delta) -> bool;

    auto setColorOfSelectedStops(const Color& color) -> bool;

    auto setStepCountOfSelectedStops(int32_t step_count) -> bool;

    auto setColorModeOfSetectedStops(bool two_colored) -> bool;

    // Invalidates sorted-stop and LUT caches.
    void needsUpdate(bool flag = true);

    auto reset() -> bool;
    auto flip() -> bool;
    auto distribute() -> bool;
    auto stretch() -> bool;

    void sortStops();

    [[nodiscard]] auto lutPtr() -> ColorLUT* {
        updateLUT();
        return lut_;
    }

    [[nodiscard]] auto lutResolution() const -> int32_t {
        return lut_resolution_;
    }

    void setLUTResolution(int32_t resolution);

    bool updateLUT();

    auto lookupFromLUT(float pos, Color& out_color) -> bool;

private:
    void _init();

    ColorSpace color_space_ = ColorSpace::sRGB;
    GradientBlendMode blend_mode_ = GradientBlendMode::RGB;

    std::vector<GradientStop> stops_;

    int32_t lut_resolution_ = 512;
    ColorLUT* lut_{};

    bool must_sort_ = false;
    bool lut_must_update_ = true;
};

} // namespace Grain
