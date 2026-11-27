#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>

#include "CocoaPopup.hpp"
#include "CocoaWindow.hpp"

#import <dispatch/dispatch.h>


namespace Grain::Platform {

class CocoaPopup::Impl
    : public std::enable_shared_from_this<CocoaPopup::Impl> {
public:
    NSPanel* panel_ = nil;
    NSWindow* parent_ = nil;
    NSView* content_view_ = nil;
    NSTimeInterval show_event_timestamp_ = 0.0;
    id event_monitor_ = nil;

    Rectd anchor_;
    Sized size_;
    PopupOptions options_;

    void updatePosition() {
        if (panel_ == nil || parent_ == nil ||
            parent_.contentView == nil) {
            return;
        }

        const NSRect anchor_rect = NSMakeRect(
            anchor_.x,
            anchor_.y,
            anchor_.width,
            anchor_.height
        );

        const NSRect window_anchor =
            [parent_.contentView convertRect:anchor_rect toView:nil];

        const NSRect screen_anchor =
            [parent_ convertRectToScreen:window_anchor];

        const CGFloat width = size_.width;
        const CGFloat height = size_.height;

        CGFloat x = 0.0;
        CGFloat y = 0.0;

        switch (options_.placement) {
            case PopupPlacement::Below:
            case PopupPlacement::Above:
                switch (options_.alignment) {
                    case PopupAlignment::Start:
                        x = NSMinX(screen_anchor);
                        break;
                    case PopupAlignment::Center:
                        x = NSMidX(screen_anchor) - width / 2.0;
                        break;
                    case PopupAlignment::End:
                        x = NSMaxX(screen_anchor) - width;
                        break;
                }

                if (options_.placement == PopupPlacement::Below) {
                    y = NSMinY(screen_anchor) - height
                        - options_.offset_y;
                }
                else {
                    y = NSMaxY(screen_anchor) + options_.offset_y;
                }

                x += options_.offset_x;
                break;

            case PopupPlacement::Right:
            case PopupPlacement::Left:
                switch (options_.alignment) {
                    case PopupAlignment::Start:
                        y = NSMaxY(screen_anchor) - height;
                        break;
                    case PopupAlignment::Center:
                        y = NSMidY(screen_anchor) - height / 2.0;
                        break;
                    case PopupAlignment::End:
                        y = NSMinY(screen_anchor);
                        break;
                }

                if (options_.placement == PopupPlacement::Right) {
                    x = NSMaxX(screen_anchor) + options_.offset_x;
                }
                else {
                    x = NSMinX(screen_anchor) - width
                        - options_.offset_x;
                }

                y -= options_.offset_y;
                break;
        }

        [panel_ setFrame:NSMakeRect(x, y, width, height)
                 display:YES];
    }


    void stopEventMonitor() {
        if (event_monitor_ == nil) {
            return;
        }

        id monitor = event_monitor_;
        event_monitor_ = nil;

        [NSEvent removeMonitor:monitor];
    }


    void dismiss() {
        if (panel_ != nil) {
            [panel_ orderOut:nil];
        }

        stopEventMonitor();
    }

    void startEventMonitor() {
        if (event_monitor_ != nil) {
            return;
        }

        std::weak_ptr<Impl> weak_self = weak_from_this();

        dispatch_async(dispatch_get_main_queue(), ^{
            auto self = weak_self.lock();

            if (!self ||
                self->event_monitor_ != nil ||
                self->panel_ == nil ||
                !self->panel_.isVisible) {
                return;
            }

            const NSEventMask event_mask =
                NSEventMaskLeftMouseDown |
                NSEventMaskRightMouseDown |
                NSEventMaskOtherMouseDown |
                NSEventMaskKeyDown;

            self->event_monitor_ =
                [NSEvent addLocalMonitorForEventsMatchingMask:event_mask
                                                     handler:^NSEvent*(NSEvent* event) {
                    auto self = weak_self.lock();

                    if (!self ||
                        self->panel_ == nil ||
                        !self->panel_.isVisible) {
                        return event;
                    }

                    // Escape dismisses the popup.
                    if (event.type == NSEventTypeKeyDown) {
                        if (event.keyCode == kVK_Escape &&
                            (event.window == self->parent_ ||
                             event.window == self->panel_)) {
                            self->dismiss();
                            return nil;
                        }

                        return event;
                    }


                    // Ignore mouse events that occurred before or while the popup
                    // was being opened.

                    NSLog(
                        @"Event timestamp: %f, show timestamp: %f",
                        event.timestamp,
                        self->show_event_timestamp_
                    );

                    if (event.timestamp <= self->show_event_timestamp_) {
                        return event;
                    }

                    if (!self->options_.dismiss_on_outside_click) {
                        return event;
                    }

                    // Clicks inside the popup must not dismiss it.
                    if (event.window == self->panel_) {
                        return event;
                    }

                    const NSPoint mouse_location = [NSEvent mouseLocation];

                    if (!NSPointInRect(mouse_location, self->panel_.frame)) {
                        NSLog(
                            @"Popup outside click: type=%ld, timestamp=%f, "
                            @"event window=%@, panel frame=%@, mouse location=%@",
                            (long)event.type,
                            event.timestamp,
                            event.window,
                            NSStringFromRect(self->panel_.frame),
                            NSStringFromPoint(mouse_location)
                        );

                        self->dismiss();
                    }

                    return event;
                }];
        });
    }
};


CocoaPopup::CocoaPopup()
    : impl_(std::make_shared<Impl>())
{
}


CocoaPopup::~CocoaPopup() {
    hide();

    if (impl_ == nullptr || impl_->panel_ == nil) {
        return;
    }

    // Detach the popup from its parent window.
    NSWindow* parent = impl_->panel_.parentWindow;

    if (parent != nil) {
        [parent removeChildWindow:impl_->panel_];
    }

    // Release the native content view.
    [impl_->panel_ setContentView:nil];

    [impl_->panel_ release];
    impl_->panel_ = nil;

    impl_->parent_ = nil;
    impl_->content_view_ = nil;
}


void CocoaPopup::show(
    Window& parent,
    View* content,
    const Rectd& anchor,
    const Sized& size,
    const PopupOptions& options
) {
    if (impl_ == nullptr || content == nullptr) {
        return;
    }

    auto* cocoa_parent = dynamic_cast<CocoaWindow*>(&parent);
    if (cocoa_parent == nullptr) {
        return;
    }

    auto* native_parent =
        static_cast<NSWindow*>(cocoa_parent->nativeWindow());

    auto* native_content =
        static_cast<NSView*>(content->nativeView());

    if (native_parent == nil || native_content == nil) {
        return;
    }

    impl_->parent_ = native_parent;
    impl_->content_view_ = native_content;
    impl_->anchor_ = anchor;
    impl_->size_ = size;
    impl_->options_ = options;

    if (impl_->panel_ == nil) {
        impl_->panel_ =
            [[NSPanel alloc]
                initWithContentRect:NSMakeRect(
                    0.0,
                    0.0,
                    size.width,
                    size.height
                )
                styleMask:NSWindowStyleMaskBorderless
                backing:NSBackingStoreBuffered
                defer:NO
            ];

        if (impl_->panel_ == nil) {
            return;
        }

        [impl_->panel_ setReleasedWhenClosed:NO];
        [impl_->panel_ setOpaque:NO];
        [impl_->panel_ setBackgroundColor:[NSColor clearColor]];
        [impl_->panel_ setHasShadow:YES];
        [impl_->panel_ setLevel:NSPopUpMenuWindowLevel];
        [impl_->panel_ setAcceptsMouseMovedEvents:YES];
    }

    [impl_->panel_ setContentView:native_content];

    [native_content setAutoresizingMask:
        NSViewWidthSizable | NSViewHeightSizable];

    // A child window follows its parent when the parent moves,
    // minimizes, or closes.
    if (impl_->panel_.parentWindow != native_parent) {
        if (impl_->panel_.parentWindow != nil) {
            [impl_->panel_.parentWindow
                removeChildWindow:impl_->panel_];
        }

        [native_parent addChildWindow:impl_->panel_
                              ordered:NSWindowAbove];
    }

    impl_->updatePosition();


    NSEvent* current_event = NSApp.currentEvent;
    impl_->show_event_timestamp_ =
        current_event != nil ? current_event.timestamp : 0.0;
    NSLog(@"show_event_timestamp_ %f", impl_->show_event_timestamp_);

    [impl_->panel_ orderFront:nil];

    impl_->startEventMonitor();
}


void CocoaPopup::hide() {
    if (impl_ == nullptr) {
        return;
    }

    impl_->dismiss();
}


void CocoaPopup::reposition(const Rectd& anchor) {
    if (impl_ == nullptr) {
        return;
    }

    impl_->anchor_ = anchor;
    impl_->updatePosition();
}


void CocoaPopup::resize(const Sized& size) {
    if (impl_ == nullptr) {
        return;
    }

    impl_->size_ = size;
    impl_->updatePosition();
}


auto CocoaPopup::isVisible() const -> bool {
    return impl_ != nullptr &&
           impl_->panel_ != nil &&
           impl_->panel_.isVisible;
}


auto createPopup() -> std::unique_ptr<Popup> {
    return std::make_unique<CocoaPopup>();
}

} // namespace Grain::Platform