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
    std::function<void()> tick_callback
)
    : tick_callback_(std::move(tick_callback))
{
    CVDisplayLinkCreateWithActiveCGDisplays(&display_link_);

    if (display_link_ != nullptr) {
        CVDisplayLinkSetOutputCallback(
            display_link_,
            displayLinkCallback,
            this);
    }
}

CocoaAnimationDriver::~CocoaAnimationDriver() {
    stop();

    if (display_link_ != nullptr) {
        CVDisplayLinkRelease(display_link_);
        display_link_ = nullptr;
    }
}

void CocoaAnimationDriver::start() {
    if (display_link_ == nullptr || running_) {
        return;
    }

    if (CVDisplayLinkStart(display_link_) == kCVReturnSuccess) {
        running_ = true;
    }
}

void CocoaAnimationDriver::stop() {
    if (display_link_ == nullptr || !running_) {
        return;
    }

    CVDisplayLinkStop(display_link_);

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

    if (!tick_queued_.compare_exchange_strong(
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
    if (running_ && tick_callback_) {
        tick_callback_();
    }

    tick_queued_.store(
        false,
        std::memory_order_release);
}

} // namespace Grain