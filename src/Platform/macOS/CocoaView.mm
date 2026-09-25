#import <Cocoa/Cocoa.h>

#include "CocoaView.hpp"
#include "GrainRootView.hpp"

namespace Grain::Platform {

class CocoaView::Impl {
public:
    NSView* nativeView = nil;
};

CocoaView::CocoaView(Grain::View* view)
    : impl_(std::make_unique<Impl>())
{
    if (view == nullptr) {
        return;
    }

    impl_->nativeView = Grain::createGrainRootView(view);
}

CocoaView::~CocoaView() = default;

void CocoaView::requestRedraw() {
    if (impl_ == nullptr ||
        impl_->nativeView == nil) {
        return;
    }

    [impl_->nativeView setNeedsDisplay:YES];
}

void* CocoaView::nativeView() noexcept {
    if (impl_ == nullptr) {
        return nullptr;
    }

    return static_cast<void*>(impl_->nativeView);
}

std::unique_ptr<View> createView(Grain::View* grainView) {
    return std::make_unique<CocoaView>(grainView);
}

} // namespace Grain::Platform