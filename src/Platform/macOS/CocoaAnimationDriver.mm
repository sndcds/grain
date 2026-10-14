#include "CocoaAnimationDriver.hpp"

#include <CoreVideo/CoreVideo.h>
#include <dispatch/dispatch.h>

#include <utility>

namespace Grain {

namespace {

void runOnMainThread(void* context) {
    auto* source =
        static_cast<CocoaAnimationDriver*>(context);

    if (source != nullptr) {
        source->runMainThreadTick();
    }
}

CVReturn displayLinkCallback(
    CVDisplayLinkRef,
    const CVTimeStamp*,
    const CVTimeStamp*,
    CVOptionFlags,
    CVOptionFlags*,
    void* context)
{
    auto* source =
        static_cast<CocoaAnimationDriver*>(context);

    if (source != nullptr) {
        source->scheduleTick();
    }

    return kCVReturnSuccess;
}

} // namespace

CocoaAnimationDriver::CocoaAnimationDriver(
    std::function<void()> tickCallback)
    : tickCallback_(std::move(tickCallback))
{
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink_);

    if (displayLink_ != nullptr) {
        CVDisplayLinkSetOutputCallback(
            displayLink_,
            displayLinkCallback,
            this);
    }
}

CocoaAnimationDriver::~CocoaAnimationDriver() {
    stop();

    if (displayLink_ != nullptr) {
        CVDisplayLinkRelease(displayLink_);
        displayLink_ = nullptr;
    }
}

void CocoaAnimationDriver::start() {
    if (displayLink_ == nullptr || running_) {
        return;
    }

    if (CVDisplayLinkStart(displayLink_) == kCVReturnSuccess) {
        running_ = true;
    }
}

void CocoaAnimationDriver::stop() {
    if (displayLink_ == nullptr || !running_) {
        return;
    }

    CVDisplayLinkStop(displayLink_);

    running_ = false;
}

bool CocoaAnimationDriver::isRunning() const noexcept {
    return running_;
}

void CocoaAnimationDriver::scheduleTick() {
    if (!running_) {
        return;
    }

    bool expected = false;

    if (!tickQueued_.compare_exchange_strong(
            expected,
            true,
            std::memory_order_acq_rel)) {
        return;
    }

    dispatch_async_f(
        dispatch_get_main_queue(),
        this,
        runOnMainThread);
}

void CocoaAnimationDriver::runMainThreadTick() {
    if (running_ && tickCallback_) {
        tickCallback_();
    }

    tickQueued_.store(
        false,
        std::memory_order_release);
}

} // namespace Grain