#pragma once

#include <grain/UI/Component.hpp>
#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Size.hpp>
#include <grain/Graphics/Color.hpp>

namespace Grain {

class Slider : public Component {
public:
    enum class Orientation :int8_t {
        Horizontal,
        Vertical
    };

    using ValueChanged = std::function<void(double)>;

    explicit Slider(
        View* view,
        double min = 0.0f,
        double max = 1.0f,
        double value = 0.0f
    );

    void draw(GraphicContext& gc) override;
    auto handleEvent(const Event& event) -> bool override;

    void setBounds(const Rectd& bounds) override;
    void setRange(double min, double max);
    void setValue(double value);
    void setStep(double step);
    void setOrientation(Orientation orientation);

    void setTrackColor(const Color& color);
    void setTrackSize(double track_size);
    [[nodiscard]] auto trackSize() const -> double { return track_size_; }

    void setHandleColor(const Color& color);
    void setHandleSize(const Sized& handle_size);
    [[nodiscard]] const Sized& handleSize() const { return handle_size_; }

    void setIndicatorColor(const Color& color);

    void setAction(ValueChanged action);

    [[nodiscard]] auto min() const -> double { return min_; }
    [[nodiscard]] auto max() const -> double { return max_; }
    [[nodiscard]] auto value() const -> double { return value_; }
    [[nodiscard]] auto step() const -> double { return step_; }
    [[nodiscard]] auto normalizedValue() const -> double;

    [[nodiscard]] auto orientation() const -> Orientation;

private:
    [[nodiscard]] auto clampValue(double value) const -> double;
    [[nodiscard]] auto valueFromPosition(const Vec2d& position) const -> double;
    [[nodiscard]] auto handleRect() const -> Rectd;
    [[nodiscard]] auto handleCenter() const -> Vec2d;

    void updateValueFromPosition(const Vec2d& position);

    double min_ = 0;
    double max_ = 1;
    double value_ = 0;
    double step_ = 0;

    double track_size_ = 4;
    Sized handle_size_ = {12, 12};
    bool dragging_ = false;
    Vec2d drag_offset_;

    Orientation orientation_ = Orientation::Horizontal;

    Color track_color_ = {0.5f, 0.5f, 0.5f, 0.5f};
    Color indicator_color_ = {0.2f, 0.5f, 1.0f, 1.0f};
    Color handle_color_ = {0.0f, 0.0f, 0.0f, 1.0f};

    ValueChanged action_;
};

}