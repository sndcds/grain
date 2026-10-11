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

    enum class State {
        Running,
        Finished,
        Cancelled
    };

    Animation(
        double delay,
        std::optional<double> duration,
        Callback callback);

    void cancel();

    [[nodiscard]] bool isFinished() const noexcept;
    [[nodiscard]] bool isCancelled() const noexcept;
    [[nodiscard]] bool isRunning() const noexcept;

    [[nodiscard]] double progress() const noexcept;
    [[nodiscard]] uint64_t frameNumber() const noexcept;

private:
    friend class AnimationDriver;

    void update();

    AnimationClock::time_point startTime_;
    AnimationClock::duration duration_;

    bool continuous_ = false;

    Callback callback_;

    double progress_ = 0.0;
    uint64_t frameNumber_ = 0;

    State state_ = State::Running;
};

} // namespace Grain