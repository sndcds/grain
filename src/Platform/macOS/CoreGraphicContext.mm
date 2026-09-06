#include "CoreGraphicsContext.hpp"

#include <cmath>

namespace Grain {

CoreGraphicsContext::CoreGraphicsContext(
    CGContextRef context
)
    : context_(context)
{
}

void CoreGraphicsContext::save()
{
    if (context_ == nullptr) {
        return;
    }

    CGContextSaveGState(context_);
}

void CoreGraphicsContext::restore()
{
    if (context_ == nullptr) {
        return;
    }

    CGContextRestoreGState(context_);
}

void CoreGraphicsContext::setFillColor(Color color)
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

void CoreGraphicsContext::setStrokeColor(Color color)
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

void CoreGraphicsContext::setLineWidth(double width)
{
    if (context_ == nullptr) {
        return;
    }

    CGContextSetLineWidth(context_, width);
}

void CoreGraphicsContext::fillRect(const Rect& rect)
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

void CoreGraphicsContext::strokeRect(const Rect& rect)
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

void CoreGraphicsContext::translate(
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

void CoreGraphicsContext::scale(
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

void CoreGraphicsContext::rotate(
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

} // namespace Grain