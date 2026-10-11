#pragma once

#include <grain/Animation/Animation.hpp>

#include <functional>
#include <memory>

namespace Grain {

class MacOSAnimationDriver;

class AnimationDriver {
public:
    static AnimationDriver& instance();

    Animation* animate(
        double delay,
        double duration,
        Animation::Callback callback);

    Animation* animateContinuous(
        double delay,
        Animation::Callback callback);

    void cancel(Animation& animation);

    void start();
    void stop();

    [[nodiscard]] bool isRunning() const noexcept;

private:
    AnimationDriver();
    ~AnimationDriver();

    AnimationDriver(const AnimationDriver&) = delete;
    AnimationDriver& operator=(const AnimationDriver&) = delete;

    void tick();

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Grain