#import <AppKit/AppKit.h>

#include "GrainRootView.hpp"
#include "CoreGraphicContext.hpp"

#include <grain/UI/View.hpp>

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

NSView* createGrainRootView(View* grainView)
{
    if (grainView == nullptr) {
        return nil;
    }

    return [[GrainRootView alloc]
        initWithFrame:NSZeroRect
        grainView:grainView];
}

} // namespace Grain