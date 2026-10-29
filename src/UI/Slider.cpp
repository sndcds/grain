#include "grain/UI/Slider.hpp"

#include "grain/Graphics/GraphicContext.hpp"
#include "grain/Platform/View.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <utility>

namespace Grain {

Slider::Slider(View* view, double min, double max, double value)
    : Component(view)
{
    setRange(min, max);
    setValue(value);
}

void Slider::draw(GraphicContext& gc) {
    const Rectd bounds = bounds_;
    const bool horizontal = orientation_ == Orientation::Horizontal;
    const double length = horizontal ? bounds.width : bounds.height;
    const double track_length = length - handle_size_.height;
    const double max_handle_width = horizontal ? bounds.height : bounds.width;
    const double handle_width = std::min(handle_size_.width, max_handle_width);
    const double handle_height = handle_size_.height;
    const double half_handle_height = handle_size_.height / 2;
    const double value = normalizedValue();

    Rectd track_rect;

    if (horizontal) {
        track_rect = {
            bounds.x + half_handle_height,
            bounds.y + ((bounds.height - track_size_) / 2),
            track_length,
            track_size_
        };
    }
    else {
        track_rect = {
            bounds.x + ((bounds.width - track_size_) / 2),
            bounds.y + half_handle_height,
            track_size_,
            track_length
        };
    }

    gc.addRoundBarPath(track_rect);
    gc.save();
    gc.clipPath();

    if (horizontal) {
        const double indicator_width = value * track_rect.width;

        gc.setFillColor(indicator_color_);
        gc.fillRect({
            track_rect.x,
            track_rect.y,
            indicator_width,
            track_rect.height
        });

        gc.setFillColor(track_color_);
        gc.fillRect({
            track_rect.x + indicator_width,
            track_rect.y,
            track_rect.width - indicator_width,
            track_rect.height
        });
    }
    else {
        const double indicator_height = value * track_rect.height;

        gc.setFillColor(indicator_color_);
        gc.fillRect({
            track_rect.x,
            track_rect.y + track_rect.height - indicator_height,
            track_rect.width,
            indicator_height
        });

        gc.setFillColor(track_color_);
        gc.fillRect({
            track_rect.x,
            track_rect.y,
            track_rect.width,
            track_rect.height - indicator_height
        });
    }

    gc.restore();

    const Rect handle = handleRect();

    gc.setFillColor(handle_color_);
    gc.addCirclePath(handle.center(), handle.height * 0.5);
    gc.fillPath();

}

bool Slider::handleEvent(const Event& event) {
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

        // Clicking outside the handle moves it to the clicked position.
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

        const Vec2d handle_center = {
            event.mouse_pos_.x - drag_offset_.x,
            event.mouse_pos_.y - drag_offset_.y
        };

        updateValueFromPosition(handle_center);
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
    if (!std::isfinite(min) || !std::isfinite(max) || min > max) {
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
    step_ = std::isfinite(step) ? std::max(0.0, step) : 0.0;
    setValue(value_);
}

void Slider::setOrientation(Orientation orientation) {
    if (orientation_ == orientation) {
        return;
    }

    orientation_ = orientation;
    // TODO: Union rect
    view()->requestRedraw();
}

void Slider::setTrackColor(const Color& color) {
    track_color_ = color;
    invalidate();
}

void Slider::setTrackSize(double track_size) {
    track_size_ = track_size;
    invalidate();
}

void Slider::setHandleColor(const Color& color) {
    handle_color_ = color;
    invalidate();
}

void Slider::setHandleSize(const Sized& handle_size) {
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

double Slider::normalizedValue() const {
    const double range = max_ - min_;

    if (range <= 0.0) {
        return 0.0;
    }

    return std::clamp((value_ - min_) / range, 0.0, 1.0);
}

Slider::Orientation Slider::orientation() const {
    return orientation_;
}

double Slider::clampValue(double value) const {
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

double Slider::valueFromPosition(const Vec2d& position) const {
    const bool horizontal = orientation_ == Orientation::Horizontal;

    const double length = horizontal
        ? bounds_.width
        : bounds_.height;

    // Handle dimension along the slider's travel direction.
    const double handle_length = horizontal
        ? handle_size_.width
        : handle_size_.height;

    const double travel = std::max(0.0, length - handle_length);

    if (travel <= 0.0 || max_ <= min_) {
        return min_;
    }

    double offset;

    if (horizontal) {
        offset = position.x
            - bounds_.x
            - (handle_length * 0.5);
    }
    else {
        // Vertical sliders increase from bottom to top.
        offset = bounds_.y
            + bounds_.height
            - position.y
            - (handle_length * 0.5);
    }

    const double normalized = std::clamp(
        offset / travel,
        0.0,
        1.0
    );

    return min_ + (normalized * (max_ - min_));
}

Rectd Slider::handleRect() const {
    const bool horizontal = orientation_ == Orientation::Horizontal;

    const double handle_length = horizontal
        ? handle_size_.width
        : handle_size_.height;

    const double length = horizontal
        ? bounds_.width
        : bounds_.height;

    const double travel = std::max(0.0, length - handle_length);
    const double position = normalizedValue() * travel;

    if (horizontal) {
        return {
            bounds_.x + position,
            bounds_.y + ((bounds_.height - handle_size_.height) * 0.5),
            handle_size_.width,
            handle_size_.height
        };
    }

    return {
        bounds_.x + ((bounds_.width - handle_size_.width) * 0.5),
        bounds_.y + bounds_.height - handle_length - position,
        handle_size_.width,
        handle_size_.height
    };
}


Vec2d Slider::handleCenter() const {
    const Rectd handle = handleRect();

    return {
        handle.x + handle.width * 0.5,
        handle.y + handle.height * 0.5
    };
}


void Slider::updateValueFromPosition(const Vec2d& position) {
    const double newValue = clampValue(valueFromPosition(position));

    if (value_ == newValue) {
        return;
    }

    value_ = newValue;
    invalidate();

    if (action_) {
        action_(value_);
    }
}

}