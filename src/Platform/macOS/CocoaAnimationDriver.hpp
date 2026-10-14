#pragma once

#include <CoreVideo/CoreVideo.h>

#include <atomic>
#include <functional>

namespace Grain {

class CocoaAnimationDriver {
public:
    explicit CocoaAnimationDriver(
        std::function<void()> tickCallback);

    ~CocoaAnimationDriver();

    CocoaAnimationDriver(const CocoaAnimationDriver&) = delete;
    CocoaAnimationDriver& operator=(const CocoaAnimationDriver&) = delete;

    void start();
    void stop();

    [[nodiscard]] bool isRunning() const noexcept;

    // Called by the CoreVideo / GCD callbacks.
    void scheduleTick();
    void runMainThreadTick();

private:
    CVDisplayLinkRef displayLink_ = nullptr;

    std::function<void()> tickCallback_;

    std::atomic_bool tickQueued_{false};

    bool running_ = false;
};

} // namespace Grain