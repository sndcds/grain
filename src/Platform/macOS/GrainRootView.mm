#import <AppKit/AppKit.h>

#include "GrainRootView.hpp"
#include "CoreGraphicContext.hpp"

#include <grain/Platform/View.hpp>
#include <grain/Input/Event.hpp>


namespace Grain {

//------------------------------------------------------------------------------
//  Modifier conversion.
//------------------------------------------------------------------------------

static KeyModifiers keyModifiersFromEvent(NSEvent* event) {
    KeyModifiers modifiers = KeyModifiers::None;
    NSEventModifierFlags flags = event.modifierFlags;

    if (flags & NSEventModifierFlagShift) {
        modifiers = modifiers | KeyModifiers::Shift;
    }

    if (flags & NSEventModifierFlagControl) {
        modifiers = modifiers | KeyModifiers::Control;
    }

    if (flags & NSEventModifierFlagOption) {
        modifiers = modifiers | KeyModifiers::Alt;
    }

    if (flags & NSEventModifierFlagCommand) {
        modifiers = modifiers | KeyModifiers::Super;
    }

    return modifiers;
}


static MouseButton mouseButtonFromEvent(NSEvent* event) {
    switch (event.buttonNumber) {
        case 0:
            return MouseButton::Left;

        case 1:
            return MouseButton::Right;

        case 2:
            return MouseButton::Middle;

        default:
            return MouseButton::Other;
    }
}

} // namespace Grain


@interface GrainRootView : NSView

- (instancetype)initWithFrame:(NSRect)frame
                    grainView:(Grain::View*)grainView;

@end

@implementation GrainRootView {
    Grain::View* grainView_;
}

- (instancetype)initWithFrame:(NSRect)frame
                    grainView:(Grain::View*)grainView
{
    self = [super initWithFrame:frame];

    if (self) {
        grainView_ = grainView;
        self.wantsLayer = NO;
    }

    return self;
}


- (BOOL)isFlipped {
    return YES;
}


- (void)drawRect:(NSRect)dirtyRect {
    (void)dirtyRect;

    if (grainView_ == nullptr) {
        return;
    }

    CGContextRef context =
        [[NSGraphicsContext currentContext] CGContext];

    if (context == nullptr) {
        return;
    }

    Grain::CoreGraphicContext graphicContext(context);

    grainView_->draw(graphicContext);
}


//------------------------------------------------------------------------------
//  Central mouse-event helper.
//------------------------------------------------------------------------------

- (void)emitMouseEvent:(NSEvent*)event
                  type:(Grain::EventType)type
                button:(Grain::MouseButton)button
{
    if (grainView_ == nullptr) {
        return;
    }

    NSPoint point =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grainEvent;

    grainEvent.type = type;
    grainEvent.mouseX = point.x;
    grainEvent.mouseY = point.y;

    grainEvent.deltaX = event.deltaX;
    grainEvent.deltaY = event.deltaY;

    grainEvent.mouseButton = button;
    grainEvent.modifiers = Grain::keyModifiersFromEvent(event);

    grainView_->handleEvent(grainEvent);
}


//------------------------------------------------------------------------------
//  Mouse button events.
//------------------------------------------------------------------------------

- (void)mouseDown:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonDown
                  button:Grain::MouseButton::Left];
}

- (void)mouseUp:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonUp
                  button:Grain::MouseButton::Left];
}

- (void)rightMouseDown:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonDown
                  button:Grain::MouseButton::Right];
}

- (void)rightMouseUp:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonUp
                  button:Grain::MouseButton::Right];
}

- (void)otherMouseDown:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonDown
                  button:Grain::mouseButtonFromEvent(event)];
}

- (void)otherMouseUp:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonUp
                  button:Grain::mouseButtonFromEvent(event)];
}


//------------------------------------------------------------------------------
//  Mouse movement.
//------------------------------------------------------------------------------

- (void)mouseMoved:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseMove
                  button:Grain::MouseButton::None];
}


//------------------------------------------------------------------------------
//  Mouse dragging.
//------------------------------------------------------------------------------

- (void)mouseDragged:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseMove
                  button:Grain::MouseButton::Left];
}

- (void)rightMouseDragged:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseMove
                  button:Grain::MouseButton::Right];
}

- (void)otherMouseDragged:(NSEvent*)event
{
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseMove
                  button:Grain::mouseButtonFromEvent(event)];
}


//------------------------------------------------------------------------------
//  Keaboard.
//------------------------------------------------------------------------------

- (void)keyDown:(NSEvent*)event
{
    if (grainView_ == nullptr) {
        return;
    }

    Grain::Event grainEvent;

    grainEvent.type = Grain::EventType::KeyDown;
    grainEvent.modifiers =
        Grain::keyModifiersFromEvent(event);

    // Translate the Cocoa key information here.
    // grainEvent.key = ...

    grainView_->handleEvent(grainEvent);
}


//------------------------------------------------------------------------------
//  First responder.
//------------------------------------------------------------------------------

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent*)event
{
    (void)event;
    return YES;
}



@end


namespace Grain {

NSView* createGrainRootView(View* grainView) {
    if (grainView == nullptr) {
        return nil;
    }

    return [[GrainRootView alloc]
        initWithFrame:NSZeroRect
            grainView:grainView];
}

} // namespace Grain