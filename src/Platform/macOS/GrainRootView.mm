#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include "GrainRootView.hpp"

#include <grain/UI/View.hpp>

@interface GrainRootView : NSView

- (instancetype)initWithFrame:(NSRect)frame
                    grainView:(Grain::View*)grainView;

@end


@implementation GrainRootView {
    Grain::View* grainView_;

    id<MTLDevice> device_;
    id<MTLCommandQueue> commandQueue_;
}

- (instancetype)initWithFrame:(NSRect)frame
                    grainView:(Grain::View*)grainView
{
    self = [super initWithFrame:frame];

    if (self) {
        grainView_ = grainView;

        device_ = MTLCreateSystemDefaultDevice();

        if (device_ == nil) {
            return self;
        }

        commandQueue_ = [device_ newCommandQueue];

        if (commandQueue_ == nil) {
            device_ = nil;
            return self;
        }

        self.wantsLayer = YES;

        CAMetalLayer* metalLayer =
            [CAMetalLayer layer];

        metalLayer.device = device_;
        metalLayer.pixelFormat =
            MTLPixelFormatBGRA8Unorm;

        metalLayer.framebufferOnly = YES;

        metalLayer.contentsScale =
            self.window.backingScaleFactor;

        self.layer = metalLayer;

        [self setNeedsDisplay:YES];
    }

    return self;
}


- (void)layout
{
    [super layout];

    CAMetalLayer* metalLayer =
        (CAMetalLayer*)self.layer;

    if (metalLayer == nil) {
        return;
    }

    metalLayer.frame = self.bounds;

    CGFloat scale =
        self.window.backingScaleFactor;

    if (scale <= 0.0) {
        scale = 1.0;
    }

    metalLayer.contentsScale = scale;

    metalLayer.drawableSize = CGSizeMake(
        self.bounds.size.width * scale,
        self.bounds.size.height * scale
    );
}


- (void)viewDidMoveToWindow
{
    [super viewDidMoveToWindow];

    if (self.window == nil) {
        return;
    }

    CAMetalLayer* metalLayer =
        (CAMetalLayer*)self.layer;

    if (metalLayer == nil) {
        return;
    }

    CGFloat scale =
        self.window.backingScaleFactor;

    if (scale <= 0.0) {
        scale = 1.0;
    }

    metalLayer.contentsScale = scale;

    metalLayer.drawableSize = CGSizeMake(
        self.bounds.size.width * scale,
        self.bounds.size.height * scale
    );

    [self setNeedsDisplay:YES];
}


- (void)viewDidChangeBackingProperties
{
    [super viewDidChangeBackingProperties];

    CAMetalLayer* metalLayer =
        (CAMetalLayer*)self.layer;

    if (metalLayer == nil) {
        return;
    }

    CGFloat scale =
        self.window.backingScaleFactor;

    if (scale <= 0.0) {
        scale = 1.0;
    }

    metalLayer.contentsScale = scale;

    metalLayer.drawableSize = CGSizeMake(
        self.bounds.size.width * scale,
        self.bounds.size.height * scale
    );

    [self setNeedsDisplay:YES];
}


- (void)drawRect:(NSRect)dirtyRect
{
    (void)dirtyRect;

    if (device_ == nil ||
        commandQueue_ == nil ||
        grainView_ == nullptr) {
        return;
    }

    CAMetalLayer* metalLayer =
        (CAMetalLayer*)self.layer;

    if (metalLayer == nil) {
        return;
    }

    id<CAMetalDrawable> drawable =
        [metalLayer nextDrawable];

    if (drawable == nil) {
        return;
    }

    id<MTLCommandBuffer> commandBuffer =
        [commandQueue_ commandBuffer];

    if (commandBuffer == nil) {
        return;
    }

    MTLRenderPassDescriptor* descriptor =
        [MTLRenderPassDescriptor renderPassDescriptor];

    descriptor.colorAttachments[0].texture =
        drawable.texture;

    descriptor.colorAttachments[0].loadAction =
        MTLLoadActionClear;

    descriptor.colorAttachments[0].storeAction =
        MTLStoreActionStore;

    descriptor.colorAttachments[0].clearColor =
        MTLClearColorMake(
            0.1,
            0.1,
            0.1,
            1.0
        );

    id<MTLRenderCommandEncoder> encoder =
        [commandBuffer
            renderCommandEncoderWithDescriptor:descriptor];

    if (encoder == nil) {
        return;
    }

    /*
     * Rendering through Grain::Canvas will be connected here.
     *
     * For now the Metal render pass is established and cleared.
     */

    [encoder endEncoding];

    [commandBuffer presentDrawable:drawable];

    [commandBuffer commit];
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