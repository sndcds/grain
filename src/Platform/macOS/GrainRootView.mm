#import <AppKit/AppKit.h>

#include "GrainRootView.hpp"
#include "CoreGraphicContext.hpp"

#include <grain/Platform/View.hpp>
#include <grain/Input/Event.hpp>
#include <grain/Geometry/Rect.hpp>


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
                   grain_view:(Grain::View*)grain_view;

@end

@implementation GrainRootView {
    Grain::View* grain_view_;
}

- (instancetype)initWithFrame:(NSRect)frame
                   grain_view:(Grain::View*)grain_view
{
    self = [super initWithFrame:frame];

    if (self) {
        grain_view_ = grain_view;
        self.wantsLayer = NO;
    }

    return self;
}


- (BOOL)isFlipped {
    return YES;
}


- (void)drawRect:(NSRect)dirtyRect {
    if (grain_view_ == nullptr) {
        return;
    }

    CGContextRef context =
        [[NSGraphicsContext currentContext] CGContext];

    if (context == nullptr) {
        return;
    }

    Grain::CoreGraphicContext gc(context);

    Grain::Rectd dirty_rect = {
        dirtyRect.origin.x,
        dirtyRect.origin.y,
        dirtyRect.size.width,
        dirtyRect.size.height
    };

    grain_view_->draw(gc, dirty_rect);
}


//------------------------------------------------------------------------------
//  Central mouse-event helper.
//------------------------------------------------------------------------------

- (void)emitMouseEvent:(NSEvent*)event
                  type:(Grain::EventType)type
                button:(Grain::MouseButton)button
{
    if (grain_view_ == nullptr) {
        return;
    }

    NSPoint point =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    Grain::Event grain_event;

    grain_event.type_ = type;
    grain_event.mouse_pos_.x = point.x;
    grain_event.mouse_pos_.y = point.y;

    grain_event.mouse_delta_.x = event.deltaX;
    grain_event.mouse_delta_.y = event.deltaY;

    grain_event.mouse_button_ = button;
    grain_event.modifiers_ = Grain::keyModifiersFromEvent(event);

    grain_view_->handleEvent(grain_event);
}


//------------------------------------------------------------------------------
//  Mouse button events.
//------------------------------------------------------------------------------

- (void)mouseDown:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonDown
                  button:Grain::MouseButton::Left];
}

- (void)mouseUp:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonUp
                  button:Grain::MouseButton::Left];
}

- (void)rightMouseDown:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonDown
                  button:Grain::MouseButton::Right];
}

- (void)rightMouseUp:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonUp
                  button:Grain::MouseButton::Right];
}

- (void)otherMouseDown:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonDown
                  button:Grain::mouseButtonFromEvent(event)];
}

- (void)otherMouseUp:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseButtonUp
                  button:Grain::mouseButtonFromEvent(event)];
}


//------------------------------------------------------------------------------
//  Mouse movement.
//------------------------------------------------------------------------------

- (void)mouseMoved:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseMove
                  button:Grain::MouseButton::None];
}


//------------------------------------------------------------------------------
//  Mouse dragging.
//------------------------------------------------------------------------------

- (void)mouseDragged:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseMove
                  button:Grain::MouseButton::Left];
}

- (void)rightMouseDragged:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseMove
                  button:Grain::MouseButton::Right];
}

- (void)otherMouseDragged:(NSEvent*)event {
    [self emitMouseEvent:event
                    type:Grain::EventType::MouseMove
                  button:Grain::mouseButtonFromEvent(event)];
}


//------------------------------------------------------------------------------
//  Keaboard.
//------------------------------------------------------------------------------

- (void)keyDown:(NSEvent*)event {
    if (grain_view_ == nullptr) {
        return;
    }

    Grain::Event grain_event;

    grain_event.type_ = Grain::EventType::KeyDown;
    grain_event.modifiers_ =
        Grain::keyModifiersFromEvent(event);

    // Translate the Cocoa key information here.
    // grain_event.key = ...

    grain_view_->handleEvent(grain_event);
}


//------------------------------------------------------------------------------
//  First responder.
//------------------------------------------------------------------------------

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent*)event {
    (void)event;
    return YES;
}



@end


namespace Grain {

NSView* createGrainRootView(View* grain_view) {
    if (grain_view == nullptr) {
        return nil;
    }

    return [[GrainRootView alloc]
        initWithFrame:NSZeroRect
            grain_view:grain_view];
}

} // namespace Grain