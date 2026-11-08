
#include "grain/UI/Slider.hpp"

#include "grain/Graphics/GraphicContext.hpp"
#include "grain/Platform/View.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace Grain {

Slider::Slider(View* view, double min, double max, double value)
    : Component(view)
{
    setRange(min, max);
    setValue(value);
}

void Slider::draw(GraphicContext& gc) {
    const Rectd track = trackRect();
    const bool horizontal = orientation_ == Orientation::Horizontal;
    const auto handle_pos = handlePosition();

    // Draw the visual track.
    gc.addRoundBarPath(track);
    gc.save();
    gc.clipPath();

    if (horizontal) {
        Rectd rect = bounds_;
        rect.width = handle_pos.x - bounds_.x + 0.5;
        gc.setFillColor(indicator_color_);
        gc.fillRect(rect);

        rect.x = handle_pos.x - 0.5;
        rect.width = bounds_.right() - handle_pos.x + 0.5;
        gc.setFillColor(track_color_);
        gc.fillRect(rect);
    }
    else {
        Rectd rect = bounds_;
        rect.y = handle_pos.y - 0.5;
        rect.height = handle_pos.y - bounds_.y + 0.5;

        gc.setFillColor(track_color_);
        gc.fillRect({
            bounds_.x,
            bounds_.y,
            bounds_.width,
            rect.y - bounds_.y
        });

        gc.setFillColor(indicator_color_);
        gc.fillRect({
            bounds_.x,
            rect.y,
            bounds_.width,
            bounds_.bottom() - rect.y
        });
    }

    gc.restore();

    // Draw the handle independently of the track representation.
    const Rectd handle = handleRect();

    gc.setFillColor(handle_color_);
    gc.addCirclePath(handle.center(), handle.height * 0.5);
    gc.fillPath();

    /*
    gc.setFillColor({1, 0, 0});
    gc.fillCircle(trackStart(), 3);
    gc.setFillColor({1, 1, 0});
    gc.fillCircle(trackEnd(), 3);
    gc.fillCircle(zeroPosition(), 3);
    */
}

auto Slider::handleEvent(const Event& event) -> bool {
    if (!enabled_) {
        return false;
    }

    switch (event.type_) {
    case EventType::MouseButtonDown: {
        if (!bounds_.contains(event.mouse_pos_)) {
            return false;
        }

        const Rectd handle_rect = handleRect();

        dragging_ = true;
        drag_offset_ = event.mouse_pos_ - handle_rect.center();

        // Clicking outside the handle moves it to the clicked
        // position and begins dragging.
        if (!handle_rect.contains(event.mouse_pos_)) {
            drag_offset_ = {0.0, 0.0};
            updateValueFromPosition(event.mouse_pos_);
        }

        return true;
    }

    case EventType::MouseMove: {
        if (!dragging_) {
            return false;
        }

        const Vec2d desired_center = {
            event.mouse_pos_.x - drag_offset_.x,
            event.mouse_pos_.y - drag_offset_.y
        };

        updateValueFromPosition(desired_center);
        return true;
    }

    case EventType::MouseButtonUp: {
        if (!dragging_) {
            return false;
        }

        dragging_ = false;
        return true;
    }

    default:
        return false;
    }
}

void Slider::setBounds(const Rectd& bounds) {
    bounds_ = bounds;
    invalidate();
}

void Slider::setRange(double min, double max) {
    if (!std::isfinite(min) ||
        !std::isfinite(max) ||
        min > max) {
        return;
    }

    min_ = min;
    max_ = max;

    value_ = clampValue(value_);

    invalidate();
}

void Slider::setValue(double value) {
    const double new_value = clampValue(value);

    if (value_ == new_value) {
        return;
    }

    value_ = new_value;
    invalidate();
}

void Slider::setStep(double step) {
    step_ = std::isfinite(step)
        ? std::max(0.0, step)
        : 0.0;

    setValue(value_);
}

void Slider::setOrientation(Orientation orientation) {
    if (orientation_ == orientation) {
        return;
    }

    orientation_ = orientation;
    invalidate();
}

void Slider::setPadding(double padding) {
    if (!std::isfinite(padding) || padding < 0.0) {
        return;
    }

    if (padding_ == padding) {
        return;
    }

    padding_ = padding;
    invalidate();
}

void Slider::setTrackColor(const Color& color) {
    track_color_ = color;
    invalidate();
}

void Slider::setTrackSize(double track_size) {
    if (!std::isfinite(track_size) || track_size < 0.0) {
        return;
    }

    if (track_size_ == track_size) {
        return;
    }

    track_size_ = track_size;
    invalidate();
}

void Slider::setHandleColor(const Color& color) {
    handle_color_ = color;
    invalidate();
}

void Slider::setHandleSize(const Sized& handle_size) {
    if (!std::isfinite(handle_size.width) ||
        !std::isfinite(handle_size.height) ||
        handle_size.width < 0.0 ||
        handle_size.height < 0.0) {
        return;
    }

    if (handle_size_.width == handle_size.width &&
        handle_size_.height == handle_size.height) {
        return;
    }

    handle_size_ = handle_size;
    invalidate();
}

void Slider::setIndicatorColor(const Color& color) {
    indicator_color_ = color;
    invalidate();
}

void Slider::setAction(ValueChanged action) {
    action_ = std::move(action);
}

auto Slider::normalizedValue() const -> double {
    const double range = max_ - min_;

    if (range <= 0.0) {
        return 0.0;
    }

    return std::clamp(
        (value_ - min_) / range,
        0.0,
        1.0
    );
}

auto Slider::orientation() const -> Slider::Orientation {
    return orientation_;
}

auto Slider::clampValue(double value) const -> double {
    if (!std::isfinite(value)) {
        return min_;
    }

    value = std::clamp(value, min_, max_);

    if (step_ > 0.0) {
        value = min_ + (std::round((value - min_) / step_) * step_);
        value = std::clamp(value, min_, max_);
    }

    return value;
}

auto Slider::trackStart() const -> Vec2d {
    const bool horizontal = orientation_ == Orientation::Horizontal;

    const double handle_length = horizontal
        ? handle_size_.width
        : handle_size_.height;

    const double length = horizontal
        ? bounds_.width
        : bounds_.height;

    const double inset = padding_ + (handle_length * 0.5);
    const double travel = std::max(0.0, length - 2.0 * inset);

    if (horizontal) {
        return {
            bounds_.x + inset,
            bounds_.y + (bounds_.height * 0.5)
        };
    }

    return {
        bounds_.x + (bounds_.width * 0.5),
        bounds_.y + bounds_.height - inset
    };
}

auto Slider::trackEnd() const -> Vec2d {
    const bool horizontal = orientation_ == Orientation::Horizontal;

    const double handle_length = horizontal
        ? handle_size_.width
        : handle_size_.height;

    const double length = horizontal
        ? bounds_.width
        : bounds_.height;

    const double inset = padding_ + (handle_length * 0.5);
    const double travel = std::max(0.0, length - 2.0 * inset);

    const Vec2d start = trackStart();

    if (horizontal) {
        return {
            start.x + travel,
            start.y
        };
    }

    return {
        start.x,
        start.y - travel
    };
}

auto Slider::zeroPosition() const -> Vec2d {
    const double range = max_ - min_;

    if (range <= 0.0 || min_ > 0.0 || max_ < 0.0) {
        // Zero is outside the range; return the minimum endpoint
        // as a deterministic fallback.
        return trackStart();
    }

    const double normalized = (0.0 - min_) / range;
    const Vec2d start = trackStart();
    const Vec2d end = trackEnd();

    return {
        start.x + ((end.x - start.x) * normalized),
        start.y + ((end.y - start.y) * normalized)
    };
}

auto Slider::handlePosition() const -> Vec2d {
    const double range = max_ - min_;

    if (range <= 0.0) {
        return trackStart();
    }

    const double normalized = (value_ - min_) / range;

    const Vec2d start = trackStart();
    const Vec2d end = trackEnd();

    return {
        start.x + ((end.x - start.x) * normalized),
        start.y + ((end.y - start.y) * normalized)
    };
}

auto Slider::trackRect() const -> Rectd {
    const bool horizontal = orientation_ == Orientation::Horizontal;

    // The visual track spans the full bounds in the travel direction.
    // Padding constrains handle travel, not track drawing.
    const double track_size = std::max(0.0, track_size_);

    const auto track_start = trackStart();
    const auto track_end = trackEnd();

    if (horizontal) {
        return {
            track_start.x - (track_size * 0.5),
            track_start.y - (track_size * 0.5),
            (track_end.x - track_start.x) + track_size,
            track_size
        };
    }

    return {
        track_start.x - (track_size * 0.5),
        track_end.y - (track_size * 0.5),
        track_size,
        (track_start.y - track_end.y) + track_size
    };
}

auto Slider::handleRect() const -> Rectd {
    const bool horizontal = orientation_ == Orientation::Horizontal;

    const double length = horizontal
        ? bounds_.width
        : bounds_.height;

    const double handle_length = horizontal
        ? handle_size_.width
        : handle_size_.height;

    // Padding is additional to half the handle's dimension.
    // Therefore, the track can extend beyond the handle at both ends.
    const double inset = padding_ + (handle_length * 0.5);
    const double travel = std::max(0.0, length - 2.0 * inset);

    const double position = normalizedValue() * travel;

    if (horizontal) {
        const double center_x = bounds_.x + inset + position;
        const double center_y = bounds_.y + (bounds_.height * 0.5);

        return {
            center_x - (handle_size_.width * 0.5),
            center_y - (handle_size_.height * 0.5),
            handle_size_.width,
            handle_size_.height
        };
    }

    const double center_x = bounds_.x + (bounds_.width * 0.5);
    const double center_y = bounds_.y + bounds_.height - inset - position;

    return {
        center_x - (handle_size_.width * 0.5),
        center_y - (handle_size_.height * 0.5),
        handle_size_.width,
        handle_size_.height
    };
}

auto Slider::handleCenter() const -> Vec2d {
    return handleRect().center();
}

auto Slider::valueFromPosition(const Vec2d& position) const -> double {
    const bool horizontal = orientation_ == Orientation::Horizontal;

    const double length = horizontal
        ? bounds_.width
        : bounds_.height;

    const double handle_length = horizontal
        ? handle_size_.width
        : handle_size_.height;

    const double inset = padding_ + (handle_length * 0.5);
    const double travel = std::max(0.0, length - 2.0 * inset);

    if (travel <= 0.0 || max_ <= min_) {
        return min_;
    }

    // The input position represents the desired handle center.
    // Horizontal values increase left to right.
    // Vertical values increase bottom to top.
    const double offset = horizontal
        ? position.x - (bounds_.x + inset)
        : (bounds_.y + bounds_.height - inset) - position.y;

    const double normalized = std::clamp(
        offset / travel,
        0.0,
        1.0
    );

    return min_ + (normalized * (max_ - min_));
}

void Slider::updateValueFromPosition(const Vec2d& position) {
    const double new_value = clampValue(valueFromPosition(position));

    if (value_ == new_value) {
        return;
    }

    value_ = new_value;
    invalidate();

    if (action_) {
        action_(value_);
    }
}

}
