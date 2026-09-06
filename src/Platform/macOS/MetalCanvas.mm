#import <Metal/Metal.h>

#include <grain/Graphics/Canvas.hpp>

namespace Grain {

class MetalCanvas final : public Canvas {
public:
    MetalCanvas(
        id<MTLCommandBuffer> commandBuffer,
        id<MTLRenderCommandEncoder> encoder
    )
        : commandBuffer_(commandBuffer),
          encoder_(encoder)
    {
    }

    ~MetalCanvas() override
    {
        if (encoder_) {
            [encoder_ endEncoding];
            encoder_ = nil;
        }
    }

    void clear(Color color) override
    {
        // The render pass normally performs the clear operation
        // before the encoder is created. Therefore there is
        // intentionally nothing to do here yet.
        //
        // A future Canvas implementation may support changing
        // the clear state explicitly.
        (void)color;
    }

    void fillRect(
        const Rect& rect,
        Color color
    ) override
    {
        /*
         * TODO:
         *
         * The first implementation will render a rectangle
         * using a Metal render pipeline.
         *
         * For now this method is intentionally empty until
         * the vertex format and render pipeline are introduced.
         */
        (void)rect;
        (void)color;
    }

private:
    id<MTLCommandBuffer> commandBuffer_ = nil;
    id<MTLRenderCommandEncoder> encoder_ = nil;
};

} // namespace Grain