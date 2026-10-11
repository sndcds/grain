#pragma once

#include <CoreVideo/CoreVideo.h>

#include <atomic>
#include <functional>

namespace Grain {

class MacOSAnimationDriver {
public:
    explicit MacOSAnimationDriver(
        std::function<void()> tickCallback);

    ~MacOSAnimationDriver();

    MacOSAnimationDriver(const MacOSAnimationDriver&) = delete;
    MacOSAnimationDriver& operator=(const MacOSAnimationDriver&) = delete;

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