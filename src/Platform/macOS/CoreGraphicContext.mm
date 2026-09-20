#include "CoreGraphicContext.hpp"
#include "grain/Geometry/Bezier.hpp"

#include <cmath>

namespace Grain {

CoreGraphicContext::CoreGraphicContext(
    CGContextRef context
)
    : context_(context)
{
}

void CoreGraphicContext::save() {
    if (context_ == nullptr) {
        return;
    }

    // GraphicContext::save();
    CGContextSaveGState(context_);
}

void CoreGraphicContext::restore() {
    if (context_ == nullptr) {
        return;
    }

    // GraphicContext::restore();
    CGContextRestoreGState(context_);
}

void CoreGraphicContext::setFillColor(const Color& color) {
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

void CoreGraphicContext::setStrokeColor(const Color& color) {
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

void CoreGraphicContext::setLineWidth(double width) {
    if (context_ == nullptr) {
        return;
    }

    CGContextSetLineWidth(context_, width);
}



void CoreGraphicContext::beginPath() noexcept {
    if (context_ == nullptr) {
        return;
    }

    CGContextBeginPath(context_);
}


void CoreGraphicContext::moveTo(const Vec2d& point) noexcept {
    if (context_ == nullptr) {
        return;
    }

    CGContextMoveToPoint(context_, point.x, point.y);
    lastPoint_ = point;
}


void CoreGraphicContext::lineTo(const Vec2d& point) noexcept {
    if (context_ == nullptr) {
        return;
    }

    CGContextAddLineToPoint(context_, point.x, point.y);
    lastPoint_ = point;
}


void CoreGraphicContext::curveTo(const Vec2d& control1, const Vec2d& control2, const Vec2d& point) noexcept {
    if (context_ == nullptr) {
        return;
    }

    CGContextAddCurveToPoint(context_, control1.x, control1.y, control2.x, control2.y, point.x, point.y);
    lastPoint_ = point;
}


void CoreGraphicContext::closePath() noexcept {
    if (context_ == nullptr) {
        return;
    }

    CGContextClosePath(context_);
}


void CoreGraphicContext::fillRect(const Rectd& rect) {
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

void CoreGraphicContext::strokeRect(const Rectd& rect) {
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


void CoreGraphicContext::fillPath(const GraphicPath& path) {
    if (context_ == nullptr) {
        return;
    }

    CGContextFillPath(context_);
}


void CoreGraphicContext::strokePath(const GraphicPath& path) {

}


void CoreGraphicContext::translate(double x, double y) {
    if (context_ == nullptr) {
        return;
    }

    CGContextTranslateCTM(
        context_,
        x,
        y
    );
}

void CoreGraphicContext::scale(double x, double y) {
    if (context_ == nullptr) {
        return;
    }

    CGContextScaleCTM(
        context_,
        x,
        y
    );
}

void CoreGraphicContext::rotate(double radians) {
    if (context_ == nullptr) {
        return;
    }

    CGContextRotateCTM(
        context_,
        radians
    );
}

void CoreGraphicContext::rotateDegrees(double degrees) {
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