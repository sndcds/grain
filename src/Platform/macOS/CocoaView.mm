#import <Cocoa/Cocoa.h>

#include "CocoaView.hpp"
#include "GrainRootView.hpp"

namespace Grain::Platform {

class CocoaView::Impl {
public:
    NSView* native_view_ = nil;
};


CocoaView::CocoaView(Grain::View* view)
    : impl_(std::make_unique<Impl>())
{
    if (view == nullptr) {
        return;
    }

    impl_->native_view_ = Grain::createGrainRootView(view);
}


CocoaView::~CocoaView() = default;


void CocoaView::requestRedraw() {
    if (impl_ == nullptr || impl_->native_view_ == nil) {
        return;
    }

    [impl_->native_view_ setNeedsDisplay:YES];
}


void CocoaView::requestRedraw(const Rectd& dirty_rect) {
    (void)dirty_rect;
    requestRedraw();
}


void* CocoaView::nativeView() {
    if (impl_ == nullptr) {
        return nullptr;
    }

    return static_cast<void*>(impl_->native_view_);
}


Vec2d CocoaView::mousePos() const {
    if (impl_ == nullptr || impl_->native_view_ == nil) {
        return {};
    }

    NSView* view = impl_->native_view_;
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