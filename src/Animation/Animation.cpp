#include "grain/Animation/Animation.hpp"

#include <algorithm>
#include <utility>

namespace Grain {

Animation::Animation(
    double delay,
    std::optional<double> duration,
    Callback callback)
    : startTime_(
        AnimationClock::now() +
        std::chrono::duration_cast<AnimationClock::duration>(
            std::chrono::duration<double>(delay))),
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

void Animation::update()
{
    if (state_ != State::Running || !callback_) {
        return;
    }

    const auto now = AnimationClock::now();

    if (now < startTime_) {
        return;
    }

    ++frameNumber_;

    const auto elapsed =
        std::chrono::duration<double>(now - startTime_).count();

    if (continuous_) {
        progress_ = elapsed;
    } else {
        const auto duration =
            std::chrono::duration<double>(duration_).count();

        progress_ = duration > 0.0
            ? elapsed / duration
            : 1.0;

        if (progress_ >= 1.0) {
            progress_ = 1.0;
            state_ = State::Finished;
        }
    }

    callback_(*this);
}

void Animation::cancel()
{
    if (state_ == State::Running) {
        state_ = State::Cancelled;
    }
}

bool Animation::isFinished() const noexcept
{
    return state_ == State::Finished;
}

bool Animation::isCancelled() const noexcept
{
    return state_ == State::Cancelled;
}

bool Animation::isRunning() const noexcept
{
    return state_ == State::Running;
}

double Animation::progress() const noexcept
{
    return progress_;
}

uint64_t Animation::frameNumber() const noexcept
{
    return frameNumber_;
}

} // namespace Grain