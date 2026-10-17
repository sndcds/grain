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


void CocoaView::requestRedraw(const Rectd& dirty_rect) {
    (void)dirty_rect;
    requestRedraw();
}


void* CocoaView::nativeView() noexcept {
    if (impl_ == nullptr) {
        return nullptr;
    }

    return static_cast<void*>(impl_->nativeView);
}


Vec2d CocoaView::mousePos() const noexcept {
    if (impl_ == nullptr || impl_->nativeView == nil) {
        return {};
    }

    NSView* view = impl_->nativeView;
    NSWindow* window = view.window;

    if (window == nil) {
        return {};
    }

    NSPoint screenPoint = [NSEvent mouseLocation];
    NSPoint windowPoint = [window convertPointFromScreen:screenPoint];
    NSPoint viewPoint = [view convertPoint:windowPoint fromView:nil];

    return {viewPoint.x, viewPoint.y};
}


std::unique_ptr<View> createView(Grain::View* grainView) {
    return std::make_unique<CocoaView>(grainView);
}

} // namespace Grain::Platform