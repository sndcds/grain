#import <Cocoa/Cocoa.h>

#include "CocoaView.hpp"
#include "GrainRootView.hpp"

namespace Grain::Platform {

class CocoaView::Impl {
public:
    NSView* native_view_ = nil;
    EventHandler event_handler_;
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

    if (impl_ == nullptr || impl_->native_view_ == nil) {
        return;
    }

    const NSRect rect = NSMakeRect(
        dirty_rect.x,
        dirty_rect.y,
        dirty_rect.width,
        dirty_rect.height
    );

    [impl_->native_view_ setNeedsDisplayInRect:rect];
}


auto CocoaView::nativeView() -> void* {
    if (impl_ == nullptr) {
        return nullptr;
    }

    return static_cast<void*>(impl_->native_view_);
}


auto CocoaView::mousePos() const -> Vec2d {
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


auto createView(Grain::View* grainView) -> std::unique_ptr<View> {
    return std::make_unique<CocoaView>(grainView);
}


void CocoaView::setEventHandler(EventHandler handler) {
    if (impl_ == nullptr) {
        return;
    }

    impl_->event_handler_ = std::move(handler);

    Grain::setGrainRootViewEventHandler(
        impl_->native_view_,
        impl_->event_handler_
    );
}

} // namespace Grain::Platform