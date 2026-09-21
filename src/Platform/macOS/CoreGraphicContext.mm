#include "CoreGraphicContext.hpp"
#include "grain/Geometry/Bezier.hpp"
#include <CoreText/CoreText.h>

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


void CoreGraphicContext::fillPath() {
    if (context_ != nullptr) {
        CGContextFillPath(context_);
    }
}


void CoreGraphicContext::strokePath() {
    if (context_ != nullptr) {
        CGContextStrokePath(context_);
    }
}

void CoreGraphicContext::drawText(
    const String& text,
    const Vec2d& pos,
    const Font* font,
    const Color& color
) {
    if (!font) {
        return;
    }

    setTextMatrix(1.0, 0.0, 0.0, -1.0, 0.0, 0.0);

    CGColorRef cgTextColor = createCGColor(color);

    CFStringRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
    CFTypeRef values[] = { font->nativeHandle(), cgTextColor };

    CFDictionaryRef cfStrAttr =
            CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys,
                               (const void**)&values, sizeof(keys) / sizeof(keys[0]),
                               &kCFTypeDictionaryKeyCallBacks,
                               &kCFTypeDictionaryValueCallBacks);


    CFStringRef cfStr = CFStringCreateWithCString(NULL, text.utf8(), kCFStringEncodingUTF8);
    CFAttributedStringRef cfAttrStr = CFAttributedStringCreate(kCFAllocatorDefault, cfStr, cfStrAttr);

    CTLineRef line = CTLineCreateWithAttributedString(cfAttrStr);

    CGContextSetTextPosition(context_, pos.x, pos.y);
    CTLineDraw(line, context_);

    CFRelease(cfStr);
    CFRelease(cfStrAttr);
    CFRelease(cfAttrStr);
    CFRelease(line);
    CGColorRelease(cgTextColor);
}


double CoreGraphicContext::drawTextInRect(
    const String& text,
    const Rectd& rect,
    Alignment alignment,
    const Font* font,
    const Color& color
){
        if (!font) {
            return 0.0;
        }

        setTextMatrix(1.0, 0.0, 0.0, -1.0, 0.0, 0.0);

        CGColorRef cgTextColor = createCGColor(color);

        CFStringRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
        CFTypeRef values[] = { font->nativeHandle(), cgTextColor };

        CFDictionaryRef cf_str_attr =
                CFDictionaryCreate(
                        kCFAllocatorDefault,
                        (const void**)&keys,
                        (const void**)&values,
                        2, // sizeof(keys) / sizeof(keys[0]),
                        &kCFTypeDictionaryKeyCallBacks,
                        &kCFTypeDictionaryValueCallBacks);

        CFStringRef cf_str = CFStringCreateWithCString(NULL, text.utf8(), kCFStringEncodingUTF8);
        CFAttributedStringRef cf_attr_str = CFAttributedStringCreate(kCFAllocatorDefault, cf_str, cf_str_attr);

        CTLineRef line = CTLineCreateWithAttributedString(cf_attr_str);

        CGFloat ascent, descent, leading;
        double textWidth = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
        double textX, textY;

        // Horizontal alignment
        switch (alignment) {
            case Alignment::Center:
            case Alignment::Top:
            case Alignment::Bottom:
                textX = rect.x + rect.width / 2 - textWidth / 2;
                break;

            case Alignment::Right:
            case Alignment::TopRight:
            case Alignment::BottomRight:
                textX = rect.x + rect.width - textWidth;
                break;

            default:
                textX = rect.x;
                break;
        }

        // Vertical alignment
        switch (alignment) {
            case Alignment::Center:
            case Alignment::Left:
            case Alignment::Right:
                textY = rect.y + rect.height / 2 + (ascent - descent) / 2;
                break;

            case Alignment::BottomLeft:
            case Alignment::Bottom:
            case Alignment::BottomRight:
                textY = rect.y + descent;
                break;

            default:
                textY = rect.y + rect.height - ascent;
                break;
        }

        CGContextSetTextPosition(context_, textX, textY);
        CTLineDraw(line, context_);

        CGRect lineBounds = CTLineGetImageBounds(line, context_);

        CFRelease(cf_str);
        CFRelease(cf_str_attr);
        CFRelease(cf_attr_str);
        CFRelease(line);
        CGColorRelease(cgTextColor);

        return lineBounds.size.width;
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


//------------------------------------------------------------------------------
//  Helper
//------------------------------------------------------------------------------

void CoreGraphicContext::setTextMatrix(double a, double b, double c, double d, double tx, double ty) {
    // d = -d;
    CGContextSetTextMatrix(context_, CGAffineTransformMake(a, b, c, d, tx, ty));
}


CGColorRef CoreGraphicContext::createCGColor(const Grain::Color& color) {
    const CGFloat components[] = {
        static_cast<CGFloat>(color.red),
        static_cast<CGFloat>(color.green),
        static_cast<CGFloat>(color.blue),
        static_cast<CGFloat>(color.alpha)
    };

    CGColorSpaceRef color_space =
        CGColorSpaceCreateDeviceRGB();

    if (color_space == nullptr) {
        return nullptr;
    }

    CGColorRef cg_color =
        CGColorCreate(
            color_space,
            components);

    CGColorSpaceRelease(color_space);

    return cg_color;
}

} // namespace Grain