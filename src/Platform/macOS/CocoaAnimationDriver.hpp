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
    CVDisplayLinkRef display_link_ = nullptr;

    std::function<void()> tick_callback_;

    std::atomic_bool tick_queued_{false};

    bool running_ = false;
};

} // namespace Grain