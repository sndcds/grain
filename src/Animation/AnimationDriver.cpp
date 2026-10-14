#include <grain/Animation/AnimationDriver.hpp>

#include "../../src/Platform/macOS/CocoaAnimationDriver.hpp"

#include <algorithm>
#include <memory>
#include <vector>

namespace Grain {

class AnimationDriver::Impl {
public:
    std::vector<std::unique_ptr<Animation>> animations;
    std::unique_ptr<CocoaAnimationDriver> platformDriver;
};

AnimationDriver& AnimationDriver::instance() {
    static AnimationDriver driver;
    return driver;
}

AnimationDriver::AnimationDriver()
    : impl_(std::make_unique<Impl>())
{
    impl_->platformDriver =
        std::make_unique<CocoaAnimationDriver>(
            [this]() {
                tick();
            });
}

AnimationDriver::~AnimationDriver() {
    stop();
}

Animation* AnimationDriver::animate(
    double delay,
    double duration,
    Animation::Callback callback)
{
    if (!callback) {
        return nullptr;
    }

    auto animation = std::unique_ptr<Animation>(
        new Animation(
            delay,
            duration,
            std::move(callback)));

    Animation* result = animation.get();

    impl_->animations.push_back(std::move(animation));

    return result;
}

Animation* AnimationDriver::animateContinuous(
    double delay,
    Animation::Callback callback)
{
    if (!callback) {
        return nullptr;
    }

    auto animation = std::unique_ptr<Animation>(
        new Animation(
            delay,
            std::nullopt,
            std::move(callback)));

    Animation* result = animation.get();

    impl_->animations.push_back(std::move(animation));

    return result;
}

void AnimationDriver::cancel(Animation& animation) {
    animation.cancel();
}

void AnimationDriver::start() {
    impl_->platformDriver->start();
}

void AnimationDriver::stop() {
    if (impl_->platformDriver) {
        impl_->platformDriver->stop();
    }
}

bool AnimationDriver::isRunning() const noexcept {
    return impl_->platformDriver &&
           impl_->platformDriver->isRunning();
}

void AnimationDriver::tick() {
    /*
     * Process only the animations that existed when
     * this frame started.
     *
     * Animations created by a callback will start with
     * the next frame.
     */
    const std::size_t count = impl_->animations.size();

    for (std::size_t i = 0; i < count; ++i) {
        impl_->animations[i]->update();
    }

    std::erase_if(
        impl_->animations,
        [](const std::unique_ptr<Animation>& animation) {
            return animation->isFinished() ||
                   animation->isCancelled();
        });
}

} // namespace Grain