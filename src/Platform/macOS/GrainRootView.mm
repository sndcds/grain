#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

#include "GrainRootView.hpp"
#include "CoreGraphicContext.hpp"

#include <grain/Platform/View.hpp>
#include <grain/Input/Event.hpp>
#include <grain/Geometry/Rect.hpp>



namespace Grain {

KeyCode keyCodeFromEvent(NSEvent* event) {
    switch (event.keyCode) {
        case kVK_Escape:       return KeyCode::Escape;
        case kVK_Return:       return KeyCode::Enter;
        case kVK_Tab:          return KeyCode::Tab;
        case kVK_Delete:       return KeyCode::Backspace;
        case kVK_Space:        return KeyCode::Space;

        case kVK_LeftArrow:    return KeyCode::Left;
        case kVK_RightArrow:   return KeyCode::Right;
        case kVK_UpArrow:      return KeyCode::Up;
        case kVK_DownArrow:    return KeyCode::Down;

        case kVK_Home:         return KeyCode::Home;
        case kVK_End:          return KeyCode::End;
        case kVK_PageUp:       return KeyCode::PageUp;
        case kVK_PageDown:     return KeyCode::PageDown;
        case kVK_ForwardDelete:return KeyCode::Delete;

        case kVK_Shift:
        case kVK_RightShift:   return KeyCode::Shift;
        case kVK_Control:
        case kVK_RightControl: return KeyCode::Control;
        case kVK_Option:
        case kVK_RightOption:  return KeyCode::Alt;
        case kVK_Command:      return KeyCode::Super;

        case kVK_ANSI_A:       return KeyCode::A;
        case kVK_ANSI_B:       return KeyCode::B;
        case kVK_ANSI_C:       return KeyCode::C;
        case kVK_ANSI_D:       return KeyCode::D;
        case kVK_ANSI_E:       return KeyCode::E;
        case kVK_ANSI_F:       return KeyCode::F;
        case kVK_ANSI_G:       return KeyCode::G;
        case kVK_ANSI_H:       return KeyCode::H;
        case kVK_ANSI_I:       return KeyCode::I;
        case kVK_ANSI_J:       return KeyCode::J;
        case kVK_ANSI_K:       return KeyCode::K;
        case kVK_ANSI_L:       return KeyCode::L;
        case kVK_ANSI_M:       return KeyCode::M;
        case kVK_ANSI_N:       return KeyCode::N;
        case kVK_ANSI_O:       return KeyCode::O;
        case kVK_ANSI_P:       return KeyCode::P;
        case kVK_ANSI_Q:       return KeyCode::Q;
        case kVK_ANSI_R:       return KeyCode::R;
        case kVK_ANSI_S:       return KeyCode::S;
        case kVK_ANSI_T:       return KeyCode::T;
        case kVK_ANSI_U:       return KeyCode::U;
        case kVK_ANSI_V:       return KeyCode::V;
        case kVK_ANSI_W:       return KeyCode::W;
        case kVK_ANSI_X:       return KeyCode::X;
        case kVK_ANSI_Y:       return KeyCode::Y;
        case kVK_ANSI_Z:       return KeyCode::Z;

        case kVK_ANSI_0:       return KeyCode::Num0;
        case kVK_ANSI_1:       return KeyCode::Num1;
        case kVK_ANSI_2:       return KeyCode::Num2;
        case kVK_ANSI_3:       return KeyCode::Num3;
        case kVK_ANSI_4:       return KeyCode::Num4;
        case kVK_ANSI_5:       return KeyCode::Num5;
        case kVK_ANSI_6:       return KeyCode::Num6;
        case kVK_ANSI_7:       return KeyCode::Num7;
        case kVK_ANSI_8:       return KeyCode::Num8;
        case kVK_ANSI_9:       return KeyCode::Num9;

        case kVK_F1:           return KeyCode::F1;
        case kVK_F2:           return KeyCode::F2;
        case kVK_F3:           return KeyCode::F3;
        case kVK_F4:           return KeyCode::F4;
        case kVK_F5:           return KeyCode::F5;
        case kVK_F6:           return KeyCode::F6;
        case kVK_F7:           return KeyCode::F7;
        case kVK_F8:           return KeyCode::F8;
        case kVK_F9:           return KeyCode::F9;
        case kVK_F10:          return KeyCode::F10;
        case kVK_F11:          return KeyCode::F11;
        case kVK_F12:          return KeyCode::F12;

        default:               return KeyCode::Unknown;
    }
}


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
        case 0: return MouseButton::Left;
        case 1: return MouseButton::Right;
        case 2: return MouseButton::Middle;
        default: return MouseButton::Other;
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

    Grain::Event grain_event;

    if (type == Grain::EventType::MouseButtonDown) {
        grain_event.mouse_click_count_ = event.clickCount;
    }

    NSPoint point =
        [self convertPoint:event.locationInWindow
                  fromView:nil];

    grain_event.type_ = type;
    grain_event.mouse_pos_.x = point.x;
    grain_event.mouse_pos_.y = point.y;

    grain_event.mouse_delta_.x = event.deltaX;
    grain_event.mouse_delta_.y = event.deltaY;

    grain_event.mouse_button_ = button;
    grain_event.modifiers_ =
        Grain::keyModifiersFromEvent(event);

    grain_view_->handleMouseEvent(grain_event);
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

- (void)keyDown:(NSEvent*)event
{
    if (grain_view_ == nullptr) {
        return;
    }

    Grain::Event grain_event;

    grain_event.type_ = Grain::EventType::KeyDown;
    grain_event.key_ = Grain::keyCodeFromEvent(event);
    grain_event.modifiers_ = Grain::keyModifiersFromEvent(event);
    grain_event.is_repeat_ = event.isARepeat;

    NSString* characters = event.characters;
    if (characters != nil) {
        grain_event.text_ = [characters UTF8String];
    }
    else {
        grain_event.text_ = "";
    }

    grain_view_->handleKeyEvent(grain_event);
}


- (void)keyUp:(NSEvent*)event
{
    if (grain_view_ == nullptr) {
        return;
    }

    Grain::Event grain_event;
    grain_event.type_ = Grain::EventType::KeyUp;
    grain_event.key_ = Grain::keyCodeFromEvent(event);
    grain_event.modifiers_ = Grain::keyModifiersFromEvent(event);

    grain_view_->handleKeyEvent(grain_event);
}


- (void)flagsChanged:(NSEvent*)event
{
    if (grain_view_ == nullptr) {
        return;
    }

    Grain::Event grain_event;
    grain_event.type_ = Grain::EventType::ModifiersChanged;
    grain_event.modifiers_ = Grain::keyModifiersFromEvent(event);

    grain_view_->handleKeyEvent(grain_event);
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