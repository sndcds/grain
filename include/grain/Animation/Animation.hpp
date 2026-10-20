#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>

namespace Grain {

using AnimationClock = std::chrono::steady_clock;

class Animation {
public:
    using Callback = std::function<void(Animation&)>;

    enum class State : uint8_t {
        Running,
        Finished,
        Cancelled
    };

    Animation(
        double delay,
        std::optional<double> duration,
        Callback callback);

    void cancel();

    [[nodiscard]] auto isFinished() const noexcept -> bool;
    [[nodiscard]] auto isCancelled() const noexcept -> bool;
    [[nodiscard]] auto isRunning() const noexcept -> bool;

    [[nodiscard]] auto progress() const noexcept -> double;
    [[nodiscard]] auto frameNumber() const noexcept -> uint64_t;

private:
    friend class AnimationDriver;

    void update();

    AnimationClock::time_point startTime_;
    AnimationClock::duration duration_;

    State state_ = State::Running;

    bool continuous_ = false;
    double progress_ = 0.0;
    uint64_t frameNumber_ = 0;

    Callback callback_;
};

} // namespace Grain