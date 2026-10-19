#include "CFObject.hpp"
#include "CoreGraphicContext.hpp"
#include "CoreTextLayout.hpp"
#include "grain/Geometry/Bezier.hpp"
#include "grain/Graphics/Gradient.hpp"

#include <CoreText/CoreText.h>

#include <cmath>

namespace Grain {

CoreGraphicContext::CoreGraphicContext(
    CGContextRef context
)
    : context_(context)
{
    color_space_ = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
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


//------------------------------------------------------------------------------
//  Clip
//------------------------------------------------------------------------------

void CoreGraphicContext::clip(const Rectd& clip_rect) {
    if (context_ == nullptr) {
        return;
    }

    CGContextClipToRect(
        context_,
        CGRectMake(
            clip_rect.x,
            clip_rect.y,
            clip_rect.width,
            clip_rect.height
        )
    );
}


void CoreGraphicContext::clipPath() {
    if (context_ == nullptr) {
        return;
    }

    CGContextClip(context_);
}


void CoreGraphicContext::clipPathEvenOdd() {
    if (context_ == nullptr) {
        return;
    }

    CGContextEOClip(context_);
}


//------------------------------------------------------------------------------
//  Blend
//------------------------------------------------------------------------------

void CoreGraphicContext::setBlendMode(
    ColorBlendMode mode
) {
    if (context_ == nullptr) {
        return;
    }

    CGBlendMode cg_blend_mode_;

    switch (mode) {
        case ColorBlendMode::Normal:
            cg_blend_mode_ = kCGBlendModeNormal;
            break;

        case ColorBlendMode::Multiply:
            cg_blend_mode_ = kCGBlendModeMultiply;
            break;

        case ColorBlendMode::Screen:
            cg_blend_mode_ = kCGBlendModeScreen;
            break;

        case ColorBlendMode::Overlay:
            cg_blend_mode_ = kCGBlendModeOverlay;
            break;

        default:
            return;
    }

    CGContextSetBlendMode(context_, cg_blend_mode_);
}


//------------------------------------------------------------------------------
//  Fill
//------------------------------------------------------------------------------

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


//------------------------------------------------------------------------------
//  Stroke
//------------------------------------------------------------------------------

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


void CoreGraphicContext::setStrokeWidth(double width) {
    if (context_ == nullptr) {
        return;
    }

    CGContextSetLineWidth(context_, width);
}


void CoreGraphicContext::setStrokeJoinStyle(StrokeJoinStyle style) {
    if (context_ == nullptr) {
        return;
    }

    CGLineJoin join;

    switch (style) {
        case StrokeJoinStyle::Miter:
            join = kCGLineJoinMiter;
            break;

        case StrokeJoinStyle::Round:
            join = kCGLineJoinRound;
            break;

        case StrokeJoinStyle::Bevel:
            join = kCGLineJoinBevel;
            break;

        default:
            return;
    }

    CGContextSetLineJoin(context_, join);
}


void CoreGraphicContext::setStrokeCapStyle(StrokeCapStyle style) {
    if (context_ == nullptr) {
        return;
    }

    CGLineCap cap;

    switch (style) {
        case StrokeCapStyle::Butt:
            cap = kCGLineCapButt;
            break;

        case StrokeCapStyle::Round:
            cap = kCGLineCapRound;
            break;

        case StrokeCapStyle::Square:
            cap = kCGLineCapSquare;
            break;

        default:
            return;
    }

    CGContextSetLineCap(context_, cap);
}


void CoreGraphicContext::setStrokeDash(
    double phase,
    const double* lengths,
    size_t count
) {
    if (context_ == nullptr) {
        return;
    }

    if (count > 0 && lengths == nullptr) {
        return;
    }

    CGContextSetLineDash(
        context_,
        phase,
        lengths,
        count
    );
}


//------------------------------------------------------------------------------
//  Path
//------------------------------------------------------------------------------

void CoreGraphicContext::beginPath() {
    if (context_ == nullptr) {
        return;
    }

    CGContextBeginPath(context_);
}


void CoreGraphicContext::moveTo(const Vec2d& point) {
    if (context_ == nullptr) {
        return;
    }

    CGContextMoveToPoint(context_, point.x, point.y);
    last_point_ = point;
}


void CoreGraphicContext::lineTo(const Vec2d& point) {
    if (context_ == nullptr) {
        return;
    }

    CGContextAddLineToPoint(context_, point.x, point.y);
    last_point_ = point;
}


void CoreGraphicContext::curveTo(
    const Vec2d& control1,
    const Vec2d& control2,
    const Vec2d& point
) {
    if (context_ == nullptr) {
        return;
    }

    CGContextAddCurveToPoint(
        context_,
        control1.x,
        control1.y,
        control2.x,
        control2.y,
        point.x,
        point.y);
    last_point_ = point;
}


void CoreGraphicContext::closePath() {
    if (context_ == nullptr) {
        return;
    }

    CGContextClosePath(context_);
}


void CoreGraphicContext::addRectPath(const Rectd& rect) {
    if (context_ == nullptr) {
        return;
    }

    CGContextAddRect(
        context_,
        CGRectMake(rect.x, rect.y, rect.width, rect.height)
    );
}


void CoreGraphicContext::addEllipsePath(const Rectd& rect) {
    if (context_ == nullptr) {
        return;
    }

    CGContextAddEllipseInRect(
        context_,
        CGRectMake(rect.x, rect.y, rect.width, rect.height)
    );
}


void CoreGraphicContext::addCirclePath(const Vec2d& center, double radius) {
    if (context_ == nullptr) {
        return;
    }

    addEllipsePath(
        Rectd(
            center.x - radius,
            center.y - radius,
            radius * 2.0,
            radius * 2.0
        )
    );
}


void CoreGraphicContext::addRingPath(
    const Vec2d& center,
    double outer_radius,
    double inner_radius
) {
    if (context_ == nullptr ||
        outer_radius <= 0.0 ||
        inner_radius < 0.0 ||
        inner_radius >= outer_radius) {
        return;
    }

    addCirclePath(center, outer_radius);
    addCirclePath(center, inner_radius);
}


void CoreGraphicContext::fillRect(const Rectd& rect) {
    if (context_ == nullptr) {
        return;
    }

    CGContextFillRect(
        context_,
        CGRectMake(rect.x, rect.y, rect.width, rect.height)
    );
}


void CoreGraphicContext::fillCircle(const Vec2d& center, double radius) {
    if (context_ == nullptr || radius <= 0.0) {
        return;
    }

    beginPath();
    addCirclePath(center, radius);
    fillPath();
}


void CoreGraphicContext::fillEllipse(const Rectd& rect) {
    if (context_ == nullptr || rect.width <= 0.0 || rect.height <= 0.0) {
        return;
    }

    beginPath();
    addEllipsePath(rect);
    fillPath();
}


void CoreGraphicContext::strokeRect(const Rectd& rect) {
    if (context_ == nullptr) {
        return;
    }

    CGContextStrokeRect(
        context_,
        CGRectMake(rect.x, rect.y, rect.width, rect.height)
    );
}


void CoreGraphicContext::strokeCircle(const Vec2d& center, double radius) {
    if (context_ == nullptr || radius <= 0.0) {
        return;
    }

    beginPath();
    addCirclePath(center, radius);
    strokePath();
}


void CoreGraphicContext::strokeEllipse(const Rectd& rect) {
    if (context_ == nullptr || rect.width <= 0.0 || rect.height <= 0.0) {
        return;
    }

    beginPath();
    addEllipsePath(rect);
    strokePath();
}


void CoreGraphicContext::fillPath() {
    if (context_ != nullptr) {
        CGContextFillPath(context_);
    }
}


void CoreGraphicContext::fillPathEvenOdd() {
    if (context_ != nullptr) {
        CGContextEOFillPath(context_);
    }
}


void CoreGraphicContext::strokePath() {
    if (context_ != nullptr) {
        CGContextStrokePath(context_);
    }
}


void CoreGraphicContext::fillGradient(
    Gradient* gradient,
    const Vec2d& start_pos,
    const Vec2d& end_pos,
    bool draw_before,
    bool draw_after
) {
    if (gradient == nullptr || gradient->stopCount() < 2) {
        return;
    }

    std::vector<CGFloat> locations;
    std::vector<CGFloat> components;

    locations.reserve(
        static_cast<size_t>(gradient->stopCount()) * 2);

    components.reserve(
        static_cast<size_t>(gradient->stopCount()) * 8);

    for (int32_t i = 0; i < gradient->stopCount(); ++i) {
        const GradientStop* stop =
            gradient->stopPtrAtIndex(i);

        if (stop == nullptr) {
            return;
        }

        const CGFloat pos = std::clamp(
            static_cast<CGFloat>(stop->pos()),
            0.0,
            1.0);

        const Color left = stop->leftColor();
        const Color right = stop->rightColor();

        auto appendColor = [&](const Color& color, CGFloat location) {
            locations.push_back(location);

            components.push_back(color.red);
            components.push_back(color.green);
            components.push_back(color.blue);
            components.push_back(color.alpha);
        };

        if (stop->isTwoColored()) {
            appendColor(left, pos);
            appendColor(right, pos);
        }
        else {
            appendColor(left, pos);
        }
    }

    if (locations.size() < 2) {
        return;
    }

    CGColorSpaceRef color_space =
        CGColorSpaceCreateWithName(kCGColorSpaceSRGB);

    if (color_space == nullptr) {
        return;
    }

    CGGradientRef cg_gradient =
        CGGradientCreateWithColorComponents(
            color_space,
            components.data(),
            locations.data(),
            locations.size());

    CGColorSpaceRelease(color_space);

    if (cg_gradient == nullptr) {
        return;
    }

    CGGradientDrawingOptions options = 0;

    if (draw_before) {
        options |= kCGGradientDrawsBeforeStartLocation;
    }

    if (draw_after) {
        options |= kCGGradientDrawsAfterEndLocation;
    }

    CGContextDrawLinearGradient(
        context_,
        cg_gradient,
        CGPointMake(start_pos.x, start_pos.y),
        CGPointMake(end_pos.x, end_pos.y),
        options);

    CGGradientRelease(cg_gradient);
}


/* TODO: implement!
void AppleCGContext::drawRadialGradient(
    Gradient* gradient,
    const Vec2d& pos,
    double radius,
    bool draw_before,
    bool draw_after
) noexcept {
    if (gradient == nullptr ||
        gradient->stopCount() < 2 ||
        radius <= 0.0) {
        return;
    }

    gradient->updateLUT();
    gradient->update(this);

    auto cg_gradient = gradient->macos_cgGradient(this);
    if (cg_gradient == nullptr) {
        return;
    }

    CGGradientDrawingOptions options = 0;

    if (draw_before) {
        options |= kCGGradientDrawsBeforeStartLocation;
    }

    if (draw_after) {
        options |= kCGGradientDrawsAfterEndLocation;
    }

    CGContextDrawRadialGradient(
        cgContext(),
        cg_gradient,
        pos.cgPoint(),
        0.0,
        pos.cgPoint(),
        radius,
        options
    );
}
*/


//------------------------------------------------------------------------------
//  Text
//------------------------------------------------------------------------------


CFObject<CTLineRef> CoreGraphicContext::createTextLine(
    const String& text,
    const Font* font,
    const Color& color
) {
    if (font == nullptr) {
        return {};
    }

    CFObject<CGColorRef> cg_color(createCGColor(color));
    if (!cg_color) {
        return {};
    }

    CFStringRef keys[] = {
        kCTFontAttributeName,
        kCTForegroundColorAttributeName
    };

    CFTypeRef values[] = {
        font->nativeHandle(),
        cg_color.get()
    };

    CFObject<CFDictionaryRef> attributes(
        CFDictionaryCreate(
            kCFAllocatorDefault,
            reinterpret_cast<const void**>(keys),
            reinterpret_cast<const void**>(values),
            2,
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks
        )
    );

    if (!attributes) {
        return {};
    }

    CFObject<CFStringRef> string(
        CFStringCreateWithCString(
            kCFAllocatorDefault,
            text.utf8(),
            kCFStringEncodingUTF8
        )
    );

    if (!string) {
        return {};
    }

    CFObject<CFAttributedStringRef> attributed_string(
        CFAttributedStringCreate(
            kCFAllocatorDefault,
            string.get(),
            attributes.get()
        )
    );

    if (!attributed_string) {
        return {};
    }

    return CFObject<CTLineRef>(
        CTLineCreateWithAttributedString(
            attributed_string.get()
        )
    );
}


void drawLineAtTopLeft(
    CGContextRef context,
    CTLineRef line,
    const Vec2d& position,
    double ascent,
    const Color& color
) {
    if (context == nullptr || line == nullptr) {
        return;
    }

    CGContextSaveGState(context);

    CGContextSetRGBFillColor(
        context,
        color.red,
        color.green,
        color.blue,
        color.alpha
    );

    // Convert the top-left position to a Core Text baseline.
    CGContextTranslateCTM(
        context,
        position.x,
        position.y + ascent
    );

    // Core Text glyph coordinates point upwards; NSView is flipped.
    CGContextScaleCTM(context, 1.0, -1.0);

    CGContextSetTextMatrix(
        context,
        CGAffineTransformIdentity
    );

    CGContextSetTextPosition(context, 0.0, 0.0);

    CTLineDraw(line, context);

    CGContextRestoreGState(context);
}


std::unique_ptr<TextLayout> CoreGraphicContext::createTextLayout(
    const String& text,
    const Font& font
) {
    return std::make_unique<CoreTextLayout>(text, font);
}


void CoreGraphicContext::drawTextLayout(
    const TextLayout& layout,
    const Vec2d& position,
    const Color& color
) {
    const auto* core_layout =
        dynamic_cast<const CoreTextLayout*>(&layout);

    if (core_layout == nullptr) {
        return;
    }

    drawLineAtTopLeft(
        context_,
        core_layout->nativeLine(),
        position,
        core_layout->ascent(),
        color
    );
}


void CoreGraphicContext::drawText(
    const String& text,
    const Vec2d& pos,
    const Font* font,
    const Color& color
) {
    if (font == nullptr || !font->isValid()) {
        return;
    }

    CoreTextLayout layout(text, *font);

    drawLineAtTopLeft(
        context_,
        layout.nativeLine(),
        pos,
        layout.ascent(),
        color
    );
}


double CoreGraphicContext::drawTextInRect(
    const String& text,
    const Rectd& rect,
    Alignment alignment,
    const Font* font,
    const Color& color
) {
    if (context_ == nullptr || font == nullptr) {
        return 0.0;
    }

    setTextMatrix(1.0, 0.0, 0.0, -1.0, 0.0, 0.0);

    CFObject<CTLineRef> line = createTextLine(text, font, color);

    if (!line) {
        return 0.0;
    }

    CGFloat ascent = 0.0;
    CGFloat descent = 0.0;
    CGFloat leading = 0.0;

    const double textWidth = CTLineGetTypographicBounds(
        line.get(),
        &ascent,
        &descent,
        &leading
    );

    double textX;
    double textY;

    // Horizontal alignment
    switch (alignment) {
        case Alignment::Center:
        case Alignment::Top:
        case Alignment::Bottom:
            textX = rect.x + rect.width / 2.0 - textWidth / 2.0;
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
            textY = rect.y + rect.height / 2.0 + (ascent - descent) / 2.0;
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

    CGContextSetTextPosition(
        context_,
        textX,
        textY
    );

    CTLineDraw(
        line.get(),
        context_
    );

    const CGRect lineBounds = CTLineGetImageBounds(
        line.get(),
        context_
    );

    return lineBounds.size.width;
}


void CoreGraphicContext::drawTextLayoutRange(
    const TextLayout& layout,
    const Vec2d& position,
    int32_t start,
    int32_t end,
    const Color& color)
{
    if (context_ == nullptr || start >= end) {
        return;
    }

    const auto* text_layout =
        dynamic_cast<const CoreTextLayout*>(&layout);

    if (text_layout == nullptr) {
        return;
    }

    const CTLineRef line = text_layout->nativeLine();

    if (line == nullptr) {
        return;
    }

    // Clamp indices to the valid string-index range.
    start = std::max<int32_t>(0, start);
    end = std::max(start, end);

    const double start_x =
        text_layout->offsetForIndex(start);

    const double end_x =
        text_layout->offsetForIndex(end);

    if (start_x == end_x) {
        return;
    }

    const double ascent = text_layout->ascent();

    CGContextSaveGState(context_);

    // Restrict drawing to the selected horizontal range.
    const CGRect clip_rect = CGRectMake(
        position.x + std::min(start_x, end_x),
        position.y,
        std::abs(end_x - start_x),
        text_layout->ascent() + text_layout->descent()
    );

    CGContextClipToRect(context_, clip_rect);

    CGContextSetRGBFillColor(
        context_,
        color.red,
        color.green,
        color.blue,
        color.alpha
    );

    // Match the coordinate transformation used by drawTextLayout().
    CGContextTranslateCTM(
        context_,
        position.x,
        position.y + ascent
    );

    CGContextScaleCTM(context_, 1.0, -1.0);

    CGContextSetTextMatrix(context_, CGAffineTransformIdentity);
    CGContextSetTextPosition(context_, 0.0, 0.0);

    CTLineDraw(line, context_);

    CGContextRestoreGState(context_);
}


//------------------------------------------------------------------------------
//  Transformation
//------------------------------------------------------------------------------

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

void CoreGraphicContext::setTextMatrix(
    double a,
    double b,
    double c,
    double d,
    double tx,
    double ty
) {
    // d = -d;
    CGContextSetTextMatrix(context_, CGAffineTransformMake(a, b, c, d, tx, ty));
}


CGColorRef CoreGraphicContext::createCGColor(
    const Color& color
) const {
    if (color_space_ == nullptr) {
        return nullptr;
    }

    const CGFloat components[] = {
        color.red,
        color.green,
        color.blue,
        color.alpha
    };

    return CGColorCreate(color_space_, components);
}

} // namespace Grain