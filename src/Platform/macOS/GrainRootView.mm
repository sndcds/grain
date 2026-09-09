#import <AppKit/AppKit.h>

#include "GrainRootView.hpp"
#include "CoreGraphicContext.hpp"

#include <grain/UI/View.hpp>

#include <iostream>
#include <memory>

@interface GrainRootView : NSView

- (instancetype)initWithFrame:(NSRect)frame
                    grainView:(Grain::View*)grainView
                 eventHandler:(Grain::EventHandler)eventHandler;

@end

@implementation GrainRootView {
    Grain::View* grain_view_;
    Grain::EventHandler event_handler_;
}

- (instancetype)initWithFrame:(NSRect)frame
                    grainView:(Grain::View*)grainView
                  eventHandler:(Grain::EventHandler)eventHandler
{
    self = [super initWithFrame:frame];

    if (self) {
        grain_view_ = grainView;
        event_handler_ = std::move(eventHandler);

        self.wantsLayer = NO;
    }

    return self;
}

- (BOOL)isFlipped
{
    /*
     * Grain uses a top-left coordinate system:
     *
     *   (0,0) ──────────► x
     *     │
     *     │
     *     ▼
     *     y
     *
     * This matches the coordinate system normally used by UI
     * frameworks and makes View coordinates independent of the
     * native platform.
     */
    return YES;
}

- (void)mouseDown:(NSEvent*)event
{
    NSPoint position =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grainEvent;
    grainEvent.type = Grain::EventType::MouseButtonDown;
    grainEvent.mouse_button = Grain::MouseButton::Left;
    grainEvent.mouse_x = position.x;
    grainEvent.mouse_y = position.y;

    if (event_handler_) {
        event_handler_(grainEvent);
    }
}

- (void)mouseUp:(NSEvent*)event
{
    NSPoint position =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grainEvent;

    grainEvent.type = Grain::EventType::MouseButtonUp;
    grainEvent.mouse_button = Grain::MouseButton::Left;
    grainEvent.mouse_x = position.x;
    grainEvent.mouse_y = position.y;

    if (event_handler_) {
        event_handler_(grainEvent);
    }
}

- (void)mouseDragged:(NSEvent*)event
{
    NSPoint position =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grainEvent;

    grainEvent.type = Grain::EventType::MouseMove;
    grainEvent.mouse_x = position.x;
    grainEvent.mouse_y = position.y;
    grainEvent.delta_x = event.delta_x;
    grainEvent.delta_y = event.delta_y;

    if (event_handler_) {
        event_handler_(grainEvent);
    }
}

- (void)mouseMoved:(NSEvent*)event
{
    NSPoint position =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grainEvent;

    grainEvent.type = Grain::EventType::MouseMove;
    grainEvent.mouse_x = position.x;
    grainEvent.mouse_y = position.y;
    grainEvent.delta_x = event.delta_x;
    grainEvent.delta_y = event.delta_y;

    if (event_handler_) {
        event_handler_(grainEvent);
    }
}

- (void)drawRect:(NSRect)dirtyRect
{
    (void)dirtyRect;

    if (grain_view_ == nullptr) {
        return;
    }

    CGContextRef context =
        [[NSGraphicsContext currentContext] CGContext];

    if (context == nullptr) {
        return;
    }

    Grain::CoreGraphicContext graphicsContext(context);

    grain_view_->draw(graphicsContext);
}

@end


namespace Grain {

NSView* createGrainRootView(
    View* grainView,
    EventHandler eventHandler
)
{
    if (grainView == nullptr) {
        return nil;
    }

    return [[GrainRootView alloc]
        initWithFrame:NSZeroRect
        grainView:grainView
        eventHandler:std::move(eventHandler)];
}

} // namespace Grain