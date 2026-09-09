#import <AppKit/AppKit.h>

#include "../Platform.hpp"

#include <grain/UI/View.hpp>

#include "GrainRootView.hpp"

#include <string>
#include <string_view>

namespace Grain::Platform {

class CocoaWindow final : public Window {
public:
    CocoaWindow(
        WindowId id,
        std::string_view title,
        int width,
        int height
    );

    ~CocoaWindow() override;

    void show() override;

    void setRootView(
        Grain::View* view
    ) override;

private:
    NSWindow* window_ = nil;

    Grain::View* root_view_ = nullptr;

    NSView* native_root_view_ = nil;
};


CocoaWindow::CocoaWindow(
    std::string_view title,
    int width,
    int height
)
{
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

    window_ =
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

    if (window_ == nil) {
        return;
    }

    [window_ setTitle:ns_title];
    [window_ center];
}


CocoaWindow::~CocoaWindow()
{
    if (window_ != nil) {
        [window_ close];
        window_ = nil;
    }

    native_root_view_ = nil;
    root_view_ = nullptr;
}


void CocoaWindow::show()
{
    if (window_ == nil) {
        return;
    }

    [window_ makeKeyAndOrderFront:nil];
}


void CocoaWindow::setRootView(
    Grain::View* view
)
{
    root_view_ = view;

    if (window_ == nil) {
        return;
    }

    [window_ setContentView:nil];

    native_root_view_ = nil;

    if (root_view_ == nullptr) {
        return;
    }

    native_root_view_ =
        createGrainRootView(
            root_view_
        );

    if (native_root_view_ == nil) {
        return;
    }

    [window_ setContentView:native_root_view_];

    NSView* content_view =
        [window_ contentView];

    if (content_view != nil) {
        native_root_view_.frame =
            content_view.bounds;

        native_root_view_.autoresizingMask =
            NSViewWidthSizable |
            NSViewHeightSizable;
    }
}

} // namespace Grain::Platform