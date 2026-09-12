#import <AppKit/AppKit.h>

#include "CocoaWindow.hpp"

#include "GrainRootView.hpp"

#include <grain/UI/View.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace Grain::Platform {

class CocoaWindow::Impl {
public:
    NSWindow* window = nil;

    Grain::View* root_view = nullptr;

    NSView* native_root_view = nil;

    Grain::EventHandler eventHandler;
};


CocoaWindow::CocoaWindow(
    WindowId id,
    std::string_view title,
    int width,
    int height,
    EventHandler eventHandler
)
    : windowId_(id),
      impl_(std::make_unique<Impl>())
{
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


CocoaWindow::~CocoaWindow()
{
    if (impl_->window != nil) {
        [impl_->window close];
        impl_->window = nil;
    }

    impl_->native_root_view = nil;
    impl_->root_view = nullptr;
}


void CocoaWindow::show()
{
    if (impl_->window == nil) {
        return;
    }

    [impl_->window makeKeyAndOrderFront:nil];
}


void CocoaWindow::setRootView(
    Grain::View* view
)
{
    impl_->root_view = view;

    if (impl_->window == nil) {
        return;
    }

    [impl_->window setContentView:nil];

    impl_->native_root_view = nil;

    if (impl_->root_view == nullptr) {
        return;
    }

    impl_->native_root_view =
        createGrainRootView(
            impl_->root_view,
            windowId_,
            impl_->eventHandler
        );

    if (impl_->native_root_view == nil) {
        return;
    }

    [impl_->window
        setContentView:impl_->native_root_view];

    NSView* content_view =
        [impl_->window contentView];

    if (content_view != nil) {
        impl_->native_root_view.frame =
            content_view.bounds;

        impl_->native_root_view.autoresizingMask =
            NSViewWidthSizable |
            NSViewHeightSizable;
    }
}

} // namespace Grain::Platform
