#import <AppKit/AppKit.h>

#include "CocoaWindow.hpp"

#include <grain/Platform/View.hpp>

#include <memory>
#include <string>
#include <string_view>


namespace Grain::Platform {

class CocoaWindow::Impl {
public:
    NSWindow* window = nil;

    NSView* nativeRootView = nil;

    Grain::EventHandler eventHandler;
};


CocoaWindow::CocoaWindow(
    uint64_t id,
    std::string_view title,
    int width,
    int height,
    EventHandler eventHandler
    ) : windowId_(id),
        impl_(std::make_unique<Impl>())
{
    std::cout << "CocoaWindow::CocoaWindow()" << std::endl;

    impl_->eventHandler = std::move(eventHandler);

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

    impl_->window =
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

    if (impl_->window == nil) {
        return;
    }

    [impl_->window setTitle:ns_title];
    [impl_->window setAcceptsMouseMovedEvents:YES];
    [impl_->window center];
}


CocoaWindow::~CocoaWindow() {
    if (impl_ == nullptr) {
        return;
    }

    impl_->nativeRootView = nil;

    if (impl_->window != nil) {
        [impl_->window close];
        impl_->window = nil;
    }
}


void CocoaWindow::show() {
    if (impl_->window == nil) {
        return;
    }

    [impl_->window makeKeyAndOrderFront:nil];
}


void CocoaWindow::requestRedraw() {
    if (impl_ == nullptr ||
        impl_->nativeRootView == nil) {
        return;
    }

    std::cout << "CocoaWindow::requestRedraw()" << std::endl;
    [impl_->nativeRootView setNeedsDisplay:YES];
}


void CocoaWindow::setRootView(
    Grain::Platform::View* platformView
)
{
    std::cout << "CocoaWindow::setRootView(): "
              << static_cast<const void*>(platformView)
              << std::endl;

    if (impl_ == nullptr ||
        impl_->window == nil) {
        return;
    }

    [impl_->window setContentView:nil];
    impl_->nativeRootView = nil;

    if (platformView == nullptr) {
        return;
    }

    NSView* nativeView =
        static_cast<NSView*>(platformView->nativeView());

    if (nativeView == nil) {
        return;
    }

    impl_->nativeRootView = nativeView;

    [impl_->window setContentView:nativeView];

    [nativeView setAutoresizingMask:
        NSViewWidthSizable | NSViewHeightSizable];
}

} // namespace Grain::Platform
