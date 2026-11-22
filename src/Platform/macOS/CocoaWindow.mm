#import <AppKit/AppKit.h>

#include "CocoaWindow.hpp"
#include <grain/UI/View.hpp>

#include <memory>
#include <string>
#include <string_view>


@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

@property(nonatomic, copy) BOOL (^shouldCloseHandler)(void);
@property(nonatomic, copy) void (^willCloseHandler)(void);
@property (nonatomic, copy) void (^didResizeHandler)(NSSize size);

@end

@implementation CocoaWindowDelegate

- (BOOL)windowShouldClose:(NSWindow*)sender {
    (void)sender;

    if (self.shouldCloseHandler) {
        return self.shouldCloseHandler();
    }

    return YES;
}

- (void)windowWillClose:(NSNotification*)notification {
    (void)notification;

    if (self.willCloseHandler) {
        self.willCloseHandler();
    }
}

- (void)windowDidResize:(NSNotification*)notification {

    NSWindow* window = notification.object;

    if (!self.didResizeHandler || !window.contentView) {
        return;
    }

    NSSize size = window.contentView.bounds.size;
    self.didResizeHandler(size);
}

@end


namespace Grain::Platform {


class CocoaWindow::Impl {
public:
    NSWindow* window_ = nil;
    NSView* native_root_view_ = nil;
    CocoaWindowDelegate* delegate = nil;

    Grain::EventHandler event_handler;

    bool closing = false;
};


CocoaWindow::CocoaWindow(
    uint64_t id,
    std::string_view title,
    const Recti& rect,
    EventHandler event_handler
) :
    window_id_(id),
    impl_(std::make_unique<Impl>())
{
    impl_->event_handler = std::move(event_handler);

    NSString* ns_title =
        [
            [NSString alloc]
            initWithBytes:title.data()
            length:title.size()
            encoding:NSUTF8StringEncoding
        ];

    if (ns_title == nil) {
        ns_title = @"Grain";
    }

    const NSRect visible_rect = [[NSScreen mainScreen] visibleFrame];

    const NSRect window_rect = NSMakeRect(
        visible_rect.origin.x + rect.x,
        NSMaxY(visible_rect) - rect.y - rect.height,
        rect.width,
        rect.height
    );

    impl_->window_ =
        [[NSWindow alloc]
            initWithContentRect:window_rect
            styleMask:
                NSWindowStyleMaskTitled |
                NSWindowStyleMaskClosable |
                NSWindowStyleMaskMiniaturizable |
                NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered
            defer:NO
        ];

    if (impl_->window_ == nil) {
        return;
    }

    [impl_->window_ setTitle:ns_title];
    [impl_->window_ setAcceptsMouseMovedEvents:YES];

    // Install the delegate

    impl_->delegate = [[CocoaWindowDelegate alloc] init];

    impl_->delegate.shouldCloseHandler = ^BOOL {
        return YES;
    };

    impl_->delegate.willCloseHandler = [this]() {
        Event event;
        event.type_ = EventType::WindowWillClose;
        emitEvent(event);

        impl_->closing = true;
    };

    impl_->delegate.didResizeHandler = [this](NSSize size) {
        Event event;
        event.type_ = EventType::WindowResize;
        event.window_size_ = {
                static_cast<double>(size.width),
                static_cast<double>(size.height)
        };

        emitEvent(event);
    };

    impl_->window_.delegate = impl_->delegate;
}


CocoaWindow::~CocoaWindow() {
    if (impl_ == nullptr) {
        return;
    }

    if (impl_->window_ != nil) {
        impl_->window_.delegate = nil;
        [impl_->window_ close];
        impl_->window_ = nil;
    }

    impl_->native_root_view_ = nil;
}


void CocoaWindow::show() {
    if (impl_->window_ == nil) {
        return;
    }

    [impl_->window_ makeKeyAndOrderFront:nil];
}


void CocoaWindow::requestRedraw() {
    if (impl_ == nullptr ||
        impl_->native_root_view_ == nil) {
        return;
    }

    [impl_->native_root_view_ setNeedsDisplay:YES];
}


void CocoaWindow::setRootView(Grain::Platform::View* platform_view) {
    if (impl_ == nullptr ||
        impl_->window_ == nil) {
        return;
    }

    [impl_->window_ setContentView:nil];
    impl_->native_root_view_ = nil;

    if (platform_view == nullptr) {
        return;
    }

    auto nativeView = static_cast<NSView*>(platform_view->nativeView());
    if (nativeView == nil) {
        return;
    }

    impl_->native_root_view_ = nativeView;

    [impl_->window_ setContentView:nativeView];

    [nativeView setAutoresizingMask:
        NSViewWidthSizable | NSViewHeightSizable];
}


auto CocoaWindow::contentBounds() const -> Rectd {

    if (impl_ == nullptr || impl_->window_ == nil) {
        return {};
    }

    NSRect bounds = impl_->window_.contentView.bounds;

    return {
        bounds.origin.x,
        bounds.origin.y,
        bounds.size.width,
        bounds.size.height
    };
}


void CocoaWindow::setEventHandler(EventHandler event_handler) {
    if (!impl_) {
        return;
    }

    impl_->event_handler = std::move(event_handler);
}


void CocoaWindow::emitEvent(const Event& event) {
    if (!impl_ || !impl_->event_handler) {
        return;
    }

    impl_->event_handler(event);
}


} // namespace Grain::Platform
