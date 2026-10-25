#include "grain/Animation/Animation.hpp"

#include <algorithm>
#include <utility>

namespace Grain {

Animation::Animation(
    double delay,
    std::optional<double> duration,
    Callback callback
)
    : startTime_(
        AnimationClock::now() +
        std::chrono::duration_cast<AnimationClock::duration>(
            std::chrono::duration<double>(delay))
    ),
    callback_(std::move(callback))
{
    if (duration.has_value()) {
        duration_ =
            std::chrono::duration_cast<AnimationClock::duration>(
                std::chrono::duration<double>(*duration));

        if (duration_ <= AnimationClock::duration::zero()) {
            state_ = State::Finished;
            progress_ = 1.0;
        }
    } else {
        continuous_ = true;
        duration_ = AnimationClock::duration::zero();
    }
}

void Animation::update() {
    if (state_ != State::Running || !callback_) {
        return;
    }

    const auto now = AnimationClock::now();

    if (now < startTime_) {
        return;
    }

    ++frameNumber_;

    const auto elapsed = std::chrono::duration<double>(now - startTime_).count();

    if (continuous_) {
        progress_ = elapsed;
    }
    else {
        const auto duration = std::chrono::duration<double>(duration_).count();

        progress_ = duration > 0.0 ? elapsed / duration : 1.0;

        if (progress_ >= 1.0) {
            progress_ = 1.0;
            state_ = State::Finished;
        }
    }

    callback_(*this);
}

void Animation::cancel() {
    if (state_ == State::Running) {
        state_ = State::Cancelled;
    }
}

auto Animation::isFinished() const -> bool {
    return state_ == State::Finished;
}

auto Animation::isCancelled() const -> bool {
    return state_ == State::Cancelled;
}

auto Animation::isRunning() const -> bool {
    return state_ == State::Running;
}

auto Animation::progress() const -> double {
    return progress_;
}

auto Animation::frameNumber() const -> uint64_t {
    return frameNumber_;
}

} // namespace Grain