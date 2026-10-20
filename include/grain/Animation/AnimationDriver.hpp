#pragma once

#include <grain/Animation/Animation.hpp>

#include <functional>
#include <memory>

namespace Grain {

class CocoaAnimationDriver;

class AnimationDriver {
public:
    AnimationDriver(const AnimationDriver&) = delete;
    auto operator=(const AnimationDriver&) -> AnimationDriver& = delete;

    static auto instance() -> AnimationDriver&;

    auto animate(
        double delay,
        double duration,
        Animation::Callback callback
    ) -> Animation*;

    auto animateContinuous(
        double delay,
        Animation::Callback callback
    ) -> Animation*;

    void cancel(Animation& animation);

    void start();
    void stop();

    [[nodiscard]] auto isRunning() const noexcept -> bool;

private:
    AnimationDriver();
    ~AnimationDriver();

    void tick();

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain