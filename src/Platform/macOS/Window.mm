#import <AppKit/AppKit.h>

#include <grain/Platform/Window.hpp>
#include <grain/UI/View.hpp>

#include "GrainRootView.hpp"

#include <memory>
#include <string_view>

namespace Grain {

class Window::Impl {
public:
    NSWindow* window = nil;

    // Grain owns the View.
    std::unique_ptr<View> root_view;

    // NSWindow owns this through its contentView hierarchy.
    NSView* native_root_view = nil;
};


Window::Window(
    std::string_view title,
    int width,
    int height
)
    : impl_(std::make_unique<Impl>())
{
    NSString* ns_title = [
        [NSString alloc]
        initWithBytes:title.data()
        length:title.size()
        encoding:NSUTF8StringEncoding
    ];

    if (ns_title == nil) {
        ns_title = @"Grain";
    }

    NSRect rect = NSMakeRect(
        100,
        100,
        width,
        height
    );

    impl_->window = [[NSWindow alloc]
        initWithContentRect:rect
        styleMask:
            NSWindowStyleMaskTitled |
            NSWindowStyleMaskClosable |
            NSWindowStyleMaskMiniaturizable |
            NSWindowStyleMaskResizable
        backing:NSBackingStoreBuffered
        defer:NO
    ];

    [impl_->window setTitle:ns_title];
    [impl_->window center];
}


Window::~Window()
{
    if (impl_->window != nil) {
        [impl_->window close];
        impl_->window = nil;
    }

    impl_->native_root_view = nil;
}


void Window::show()
{
    if (impl_->window == nil) {
        return;
    }

    [impl_->window makeKeyAndOrderFront:nil];
}


void Window::setRootView(std::unique_ptr<View> view)
{
    impl_->root_view = std::move(view);

    if (impl_->window == nil) {
        return;
    }

    // Remove the previous native root view.
    [impl_->window setContentView:nil];
    impl_->native_root_view = nil;

    if (!impl_->root_view) {
        return;
    }

    // Create the native macOS backing view for the Grain View.
    impl_->native_root_view =
        createGrainRootView(
            impl_->root_view.get()
        );

    if (impl_->native_root_view == nil) {
        return;
    }

    [impl_->window setContentView:impl_->native_root_view];

    NSView* content_view = [impl_->window contentView];

    if (content_view != nil) {
        impl_->native_root_view.frame =
            content_view.bounds;

        impl_->native_root_view.autoresizingMask =
            NSViewWidthSizable |
            NSViewHeightSizable;
    }
}

} // namespace Grain