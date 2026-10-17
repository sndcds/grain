#pragma once

#include <grain/Graphics/Color.hpp>

#include <cstdint>
#include <vector>

namespace Grain {

class ColorLUT;

enum class GradientBlendMode : int32_t {
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

    bool operator<(const GradientStop& other) const {
        return pos_ < other.pos_;
    }

    bool operator==(const GradientStop& other) const {
        return pos_ == other.pos_;
    }

    bool operator!=(const GradientStop& other) const {
        return !(*this == other);
    }

    bool operator>(const GradientStop& other) const {
        return pos_ > other.pos_;
    }

    bool operator>=(const GradientStop& other) const {
        return pos_ >= other.pos_;
    }

    bool operator<=(const GradientStop& other) const {
        return pos_ <= other.pos_;
    }

    void init();

    [[nodiscard]]
    float pos() const {
        return pos_;
    }

    [[nodiscard]]
    Color color(int32_t part) const {
        return part >= 0 && part <= 1
            ? colors_[part]
            : Color(0, 0, 0, 0);
    }

    [[nodiscard]]
    Color leftColor() const {
        return colors_[0];
    }

    [[nodiscard]]
    Color rightColor() const {
        return two_colored_ ? colors_[1] : colors_[0];
    }

    [[nodiscard]]
    bool isSelected() const {
        return selected_;
    }

    [[nodiscard]]
    bool isTwoColored() const {
        return two_colored_;
    }

    [[nodiscard]]
    bool isSecondActive() const {
        return second_active_;
    }

    [[nodiscard]]
    int32_t stepCount() const {
        return step_count_;
    }

    [[nodiscard]]
    GradientBlendMode blendMode() const {
        return blend_mode_;
    }

    [[nodiscard]]
    float rememberedPos() const {
        return remembered_pos_;
    }

    bool setPos(float pos);

    bool setColor(int32_t part, const Color& color);

    bool setStepCount(int32_t step_count);

    void setBlendMode(GradientBlendMode blend_mode) {
        blend_mode_ = blend_mode;
    }

    void select() {
        selected_ = true;
    }

    void deselect() {
        selected_ = false;
    }

    void toggleSelection() {
        selected_ = !selected_;
    }

    void remember() {
        remembered_pos_ = pos_;
    }

protected:
    float pos_ = 0.0f;

    Color colors_[2] = {
        {0, 0, 0, 1},
        {0, 0, 0, 1}
    };

    int32_t level_curve_values_[6] = {
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

        First = Fast,
        Last = Plasma
    };

    Gradient();
    Gradient(const Gradient* gradient);
    ~Gradient();

    Gradient(const Gradient&) = delete;
    Gradient& operator=(const Gradient&) = delete;

    static Gradient* createByPreset(Preset preset, bool flip = false);

    void set(const Gradient* gradient);

    [[nodiscard]]
    ColorSpace colorSpace() const {
        return color_space_;
    }

    void setColorSpace(ColorSpace color_space);

    [[nodiscard]]
        GradientBlendMode blendMode() const {
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

    [[nodiscard]]
    bool hasStops() const {
        return !stops_.empty();
    }

    [[nodiscard]]
    bool canAccessStop(int32_t index) const {
        return index >= 0 &&
               index < static_cast<int32_t>(stops_.size());
    }

    [[nodiscard]]
    int32_t stopCount() const {
        return static_cast<int32_t>(stops_.size());
    }

    [[nodiscard]]
    int32_t lastStopIndex() const {
        return stopCount() - 1;
    }

    [[nodiscard]]
    int32_t selectedStopCount() const;

    [[nodiscard]]
    int32_t firstSelectedStopIndex() const;

    [[nodiscard]]
    int32_t lastSelectedStopIndex() const;

    int32_t selectedStopIndices(
        int32_t* out_first_index,
        int32_t* out_last_index
    ) const;

    void selectAllStops();
    void deselectAllStops();
    void revertStopSelection();
    void selectStopEach(int32_t step);

    bool stepSelectedStops(int32_t step_count);

    void removeAllStops();
    void removeStop(int32_t index);
    int32_t removeSelectedStops();

    void addStop(float pos, const Color& color);

    void addStop(float pos, const Color& color1, const Color& color2);

    void addStop(GradientStop& stop);

    [[nodiscard]]
    GradientStop* mutStopPtrAtIndex(int32_t index);

    [[nodiscard]]
    const GradientStop* stopPtrAtIndex(int32_t index) const;

    void colorAtIndex(int32_t index, int32_t part, Color& out_color) const;

    void leftColorAtIndex(int32_t index, Color& out_color) const;

    void rightColorAtIndex(int32_t index, Color& out_color) const;

    bool lookupColor(float pos, Color& out_color);

    bool lookupColorValues(float pos, float* out_values);

    bool setColorAtIndex(int32_t index, int32_t part, const Color& color);

    void rememberSelectedStops();

    bool moveSelectedStops(float delta);

    bool setColorOfSelectedStops(const Color& color);

    bool setStepCountOfSelectedStops(int32_t step_count);

    bool setColorModeOfSetectedStops(bool two_colored);

    // Invalidates sorted-stop and LUT caches.
    void needsUpdate(bool flag = true);

    bool reset();
    bool flip();
    bool distribute();
    bool stretch();

    void sortStops();

    [[nodiscard]]
    ColorLUT* lutPtr() {
        updateLUT();
        return lut_;
    }

    [[nodiscard]]
    int32_t lutResolution() const {
        return lut_resolution_;
    }

    void setLUTResolution(int32_t resolution);

    bool updateLUT();

    bool lookupFromLUT(float pos, Color& out_color);

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
