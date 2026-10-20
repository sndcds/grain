#import <AppKit/AppKit.h>

#include "CocoaWindow.hpp"
#include <grain/Platform/View.hpp>

#include <memory>
#include <string>
#include <string_view>


@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

@property(nonatomic, copy) BOOL (^shouldCloseHandler)(void);
@property(nonatomic, copy) void (^willCloseHandler)(void);

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
    int width,
    int height,
    EventHandler event_handler
)
    : window_id_(id),
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

    NSRect rect = NSMakeRect(
        100.0,
        100.0,
        static_cast<CGFloat>(width),
        static_cast<CGFloat>(height)
    );

    impl_->window_ =
        [[NSWindow alloc]
            initWithContentRect:rect
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
    [impl_->window_ center];


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
