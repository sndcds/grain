#include "CoreGraphicContext.hpp"

#include <cmath>

namespace Grain {

CoreGraphicContext::CoreGraphicContext(
    CGContextRef context
)
    : context_(context)
{
}

void CoreGraphicContext::save()
{
    if (context_ == nullptr) {
        return;
    }

    GraphicContext::save();
    CGContextSaveGState(context_);
}

void CoreGraphicContext::restore()
{
    if (context_ == nullptr) {
        return;
    }

    GraphicContext::restore();
    CGContextRestoreGState(context_);
}

void CoreGraphicContext::setFillColor(const Color& color)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextSetRGBFillColor(
        context_,
        color.red,
        color.green,
        color.blue,
        color.alpha
    );
}

void CoreGraphicContext::setStrokeColor(const Color& color)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextSetRGBStrokeColor(
        context_,
        color.red,
        color.green,
        color.blue,
        color.alpha
    );
}

void CoreGraphicContext::setLineWidth(double width)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextSetLineWidth(context_, width);
}

void CoreGraphicContext::fillRect(const Rectd& rect)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextFillRect(
        context_,
        CGRectMake(
            rect.x,
            rect.y,
            rect.width,
            rect.height
        )
    );
}

void CoreGraphicContext::strokeRect(const Rectd& rect)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextStrokeRect(
        context_,
        CGRectMake(
            rect.x,
            rect.y,
            rect.width,
            rect.height
        )
    );
}

void CoreGraphicContext::translate(
    double x,
    double y
)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextTranslateCTM(
        context_,
        x,
        y
    );
}

void CoreGraphicContext::scale(
    double x,
    double y
)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextScaleCTM(
        context_,
        x,
        y
    );
}

void CoreGraphicContext::rotate(
    double radians
)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextRotateCTM(
        context_,
        radians
    );
}

void CoreGraphicContext::rotateDegrees(
    double degrees
)
{
    if (context_ == nullptr) {
        return;
    }

    const double radians = degrees * std::numbers::pi / 180.0;

    CGContextRotateCTM(
        context_,
        radians
    );
}

} // namespace Grain