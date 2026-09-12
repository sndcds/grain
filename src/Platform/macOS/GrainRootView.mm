#import <AppKit/AppKit.h>

#include "GrainRootView.hpp"
#include "CoreGraphicContext.hpp"

#include <grain/UI/View.hpp>

#include <iostream>
#include <memory>

@interface GrainRootView : NSView

- (instancetype)initWithFrame:(NSRect)frame
                    grainView:(Grain::View*)grainView
                     windowId:(Grain::WindowId)windowId
                 eventHandler:(Grain::EventHandler)eventHandler;

@end

@implementation GrainRootView {
    Grain::View* grainView_;
    Grain::WindowId windowId_;
    Grain::EventHandler eventHandler_;
}

- (instancetype)initWithFrame:(NSRect)frame
                    grainView:(Grain::View*)grainView
                     windowId:(Grain::WindowId)windowId
                  eventHandler:(Grain::EventHandler)eventHandler
{
    self = [super initWithFrame:frame];

    if (self) {
        grainView_ = grainView;
        windowId_ = windowId;
        eventHandler_ = std::move(eventHandler);

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

- (void)emitMouseButtonEvent:(NSEvent*)event
                        type:(Grain::EventType)type
                      button:(Grain::MouseButton)button
{
    const NSPoint position =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grain_event;
    grain_event.type = type;
    grain_event.windowId = windowId_;
    grain_event.mouseButton = button;
    grain_event.mouseX = position.x;
    grain_event.mouseY = position.y;

    if (eventHandler_) {
        eventHandler_(grain_event);
    }
}

- (void)mouseDown:(NSEvent*)event
{
    [self emitMouseButtonEvent:event
                          type:Grain::EventType::MouseButtonDown
                        button:Grain::MouseButton::Left];
}

- (void)mouseUp:(NSEvent*)event
{
    [self emitMouseButtonEvent:event
                          type:Grain::EventType::MouseButtonUp
                        button:Grain::MouseButton::Left];
}

- (void)mouseDragged:(NSEvent*)event
{
    NSPoint position =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grainEvent;

    grainEvent.type = Grain::EventType::MouseMove;
    grainEvent.windowId = windowId_;
    grainEvent.mouseX = position.x;
    grainEvent.mouseY = position.y;
    grainEvent.deltaX = event.deltaX;
    grainEvent.deltaY = event.deltaY;

    if (eventHandler_) {
        eventHandler_(grainEvent);
    }
}

- (void)mouseMoved:(NSEvent*)event
{
    NSPoint position =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grainEvent;

    grainEvent.type = Grain::EventType::MouseMove;
    grainEvent.windowId = windowId_;
    grainEvent.mouseX = position.x;
    grainEvent.mouseY = position.y;
    grainEvent.deltaX = event.deltaX;
    grainEvent.deltaY = event.deltaY;

    if (eventHandler_) {
        eventHandler_(grainEvent);
    }
}

- (void)drawRect:(NSRect)dirtyRect
{
    (void)dirtyRect;

    if (grainView_ == nullptr) {
        return;
    }

    CGContextRef context =
        [[NSGraphicsContext currentContext] CGContext];

    if (context == nullptr) {
        return;
    }

    Grain::CoreGraphicContext graphicsContext(context);

    grainView_->draw(graphicsContext);
}

@end


namespace Grain {

NSView* createGrainRootView(
    View* grainView,
    WindowId windowId,
    EventHandler eventHandler
)
{
    if (grainView == nullptr) {
        return nil;
    }

    return [[GrainRootView alloc]
        initWithFrame:NSZeroRect
        grainView:grainView
        windowId:windowId
        eventHandler:std::move(eventHandler)];
}

} // namespace Grain
