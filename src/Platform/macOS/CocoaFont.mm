#include "CocoaFont.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreText/CoreText.h>
#include <grain/Graphics/GraphicCompoundPath.hpp>

#include <vector>
#include <algorithm>
#include <limits>

namespace Grain::Platform {

namespace {

CFStringRef makeCFString(const char* str, int32_t length) {
    if (str == nullptr) {
        return nullptr;
    }

    if (length < 0) {
        return CFStringCreateWithCString(
            kCFAllocatorDefault,
            str,
            kCFStringEncodingUTF8
        );
    }

    return CFStringCreateWithBytes(
        kCFAllocatorDefault,
        reinterpret_cast<const UInt8*>(str),
        length,
        kCFStringEncodingUTF8,
        false
    );
}


double safeCellHeight(double ascent, double descent, double leading) {
    return ascent + descent + leading;
}


CTLineRef createLine(CFStringRef string, CTFontRef font) {
    if (string == nullptr || font == nullptr) {
        return nullptr;
    }

    CFObject<CFMutableAttributedStringRef> attributes(
        CFAttributedStringCreateMutable(
            kCFAllocatorDefault,
            0
        )
    );

    if (!attributes) {
        return nullptr;
    }

    CFAttributedStringReplaceString(
        attributes.get(),
        CFRangeMake(0, 0),
        string
    );

    CFAttributedStringSetAttribute(
        attributes.get(),
        CFRangeMake(
            0,
            CFStringGetLength(string)
        ),
        kCTFontAttributeName,
        font
    );

    return CTLineCreateWithAttributedString(
        attributes.get()
    );
}


struct CGPathContext {
    FontPath* path = nullptr;
    Vec2d offset{};
    bool failed = false;
};


void applyCGPathElement(
    void* info,
    const CGPathElement* element
) {
    auto* context = static_cast<CGPathContext*>(info);

    if (context == nullptr ||
        context->path == nullptr ||
        element == nullptr ||
        context->failed) {
        return;
    }

    FontPathElement item;

    const auto makePoint = [&](std::size_t index) -> Vec2d {
        return {
            element->points[index].x + context->offset.x,
            element->points[index].y + context->offset.y
        };
    };

    switch (element->type) {
        case kCGPathElementMoveToPoint:
            item.type = FontPathElement::Type::MoveTo;
            item.points[0] = makePoint(0);
            break;

        case kCGPathElementAddLineToPoint:
            item.type = FontPathElement::Type::LineTo;
            item.points[0] = makePoint(0);
            break;

        case kCGPathElementAddQuadCurveToPoint:
            item.type = FontPathElement::Type::QuadraticTo;
            item.points[0] = makePoint(0); // Control point
            item.points[1] = makePoint(1); // End point
            break;

        case kCGPathElementAddCurveToPoint:
            item.type = FontPathElement::Type::CubicTo;
            item.points[0] = makePoint(0);
            item.points[1] = makePoint(1);
            item.points[2] = makePoint(2);
            break;

        case kCGPathElementCloseSubpath:
            item.type = FontPathElement::Type::Close;
            break;

        default:
            return;
    }

    // Do not allow a C++ exception to escape through CGPathApply.
    try {
        context->path->elements.push_back(item);
    }
    catch (...) {
        context->failed = true;
    }
}

} // namespace


CocoaFont::CocoaFont(const String& name, float size) {
    set(name, size);
}


void CocoaFont::resetMetrics() {
    is_valid_ = false;

    font_name_ = String();
    display_name_ = String();

    ascent_ = 0.0;
    descent_ = 0.0;
    x_height_ = 0.0;
    cap_height_ = 0.0;
    leading_ = 0.0;
    cell_height_ = 0.0;
    line_height_ = 0.0;
    italic_angle_ = 0.0;
    underline_position_ = 0.0;
    underline_thickness_ = 0.0;

    units_per_em_ = 0;
    glyph_count_ = 0;

    bounding_box_ = Rectd{};
}


void CocoaFont::set(const String& name, float size) {
    ct_font_.reset();
    resetMetrics();

    font_size_ = size;

    const char* name_utf8 = name.utf8();

    if (name_utf8 == nullptr || *name_utf8 == '\0') {
        ct_font_.reset(
            CTFontCreateUIFontForLanguage(
                kCTFontSystemFontType,
                size,
                nullptr
            )
        );
    }
    else {
        CFObject<CFStringRef> cf_name(
            CFStringCreateWithCString(
                kCFAllocatorDefault,
                name_utf8,
                kCFStringEncodingUTF8
            )
        );

        if (cf_name) {
            ct_font_.reset(
                CTFontCreateWithName(
                    cf_name.get(),
                    size,
                    nullptr
                )
            );
        }
    }

    if (!ct_font_) {
        return;
    }

    is_valid_ = true;

    CFObject<CFStringRef> postscript_name(
        CTFontCopyPostScriptName(ct_font_.get())
    );


    if (postscript_name) {
        char buffer[1024];

        if (CFStringGetCString(
                postscript_name.get(),
                buffer,
                sizeof(buffer),
                kCFStringEncodingUTF8
            )
        ) {
            font_name_ = String(buffer);
        }
    }

    CFObject<CFStringRef> displayName(
        CTFontCopyDisplayName(ct_font_.get())
    );

    if (displayName) {
        char buffer[1024];

        if (CFStringGetCString(
                displayName.get(),
                buffer,
                sizeof(buffer),
                kCFStringEncodingUTF8
            )
        ) {
            display_name_ = String(buffer);
        }
    }

    updateMetrics();
}


void CocoaFont::updateMetrics() {
    if (!ct_font_) {
        return;
    }

    const CTFontRef font = ct_font_.get();

    ascent_ = CTFontGetAscent(font);
    descent_ = CTFontGetDescent(font);
    leading_ = CTFontGetLeading(font);
    x_height_ = CTFontGetXHeight(font);
    cap_height_ = CTFontGetCapHeight(font);
    cell_height_ = safeCellHeight(ascent_, descent_, leading_);
    line_height_ = cell_height_;
    italic_angle_ = CTFontGetSlantAngle(font);

    underline_position_ = CTFontGetUnderlinePosition(font);
    underline_thickness_ = CTFontGetUnderlineThickness(font);

    units_per_em_ = static_cast<int32_t>(CTFontGetUnitsPerEm(font));
    glyph_count_ = static_cast<int32_t>(CTFontGetGlyphCount(font));

    const CGRect box = CTFontGetBoundingBox(font);

    bounding_box_ =
        Rectd(
            box.origin.x, box.origin.y, box.size.width, box.size.height
        );
}


const char* CocoaFont::fontNameUtf8() const {
    return font_name_.utf8();
}


const char* CocoaFont::displayNameUtf8() const {
    return display_name_.utf8();
}


float CocoaFont::size() const {
    return font_size_;
}


bool CocoaFont::isValid() const {
    return is_valid_;
}


double CocoaFont::ascent() const {
    return ascent_;
}


double CocoaFont::descent() const {
    return descent_;
}


double CocoaFont::xHeight() const {
    return x_height_;
}


double CocoaFont::capHeight() const {
    return cap_height_;
}


double CocoaFont::leading() const {
    return leading_;
}


double CocoaFont::cellHeight() const {
    return cell_height_;
}


double CocoaFont::lineHeight() const {
    return line_height_;
}


double CocoaFont::italicAngle() const {
    return italic_angle_;
}


double CocoaFont::underlinePosition() const{
    return underline_position_;
}


double CocoaFont::underlineThickness() const {
    return underline_thickness_;
}


int32_t CocoaFont::unitsPerEm() const {
    return units_per_em_;
}


int32_t CocoaFont::glyphCount() const {
    return glyph_count_;
}


const Rectd& CocoaFont::boundingBox() const {
    return bounding_box_;
}


double CocoaFont::glyphAdvanceWidth(
    const char* symbol,
    int32_t length
) const {
    if (!ct_font_ || symbol == nullptr) {
        return 0.0;
    }

    CFObject<CFStringRef> string(
        makeCFString(symbol, length)
    );

    if (!string) {
        return 0.0;
    }

    const CFIndex length_utf16 = CFStringGetLength(string.get());

    if (length_utf16 == 0) {
        return 0.0;
    }

    std::vector<UniChar> characters(static_cast<size_t>(length_utf16));

    CFStringGetCharacters(
        string.get(),
        CFRangeMake(0, length_utf16),
        characters.data()
    );

    std::vector<CGGlyph> glyphs(static_cast<size_t>(length_utf16));

    if (!CTFontGetGlyphsForCharacters(
            ct_font_.get(),
            characters.data(),
            glyphs.data(),
            length_utf16
        )
    ) {
        return 0.0;
    }

    std::vector<CGSize> advances(static_cast<size_t>(length_utf16));

    CTFontGetAdvancesForGlyphs(
        ct_font_.get(),
        kCTFontOrientationHorizontal,
        glyphs.data(),
        advances.data(),
        length_utf16
    );

    double width = 0.0;

    for (CFIndex i = 0; i < length_utf16; ++i) {
        width += advances[i].width;
    }

    return width;
}


auto CocoaFont::glyphPairAdvanceWidth(
    const char* left,
    const char* right
) const -> double {

    if (!ct_font_ || left == nullptr || right == nullptr || *left == '\0' || *right == '\0') {
        return 0.0;
    }

    const auto left_length = static_cast<int32_t>(std::strlen(left));
    const auto right_length = static_cast<int32_t>(std::strlen(right));

    CFObject<CFStringRef> left_string(
        makeCFString(left, left_length)
    );

    CFObject<CFStringRef> right_string(
        makeCFString(right, right_length)
    );

    if (!left_string || !right_string) {
        return 0.0;
    }

    // Create the combined string.
    CFObject<CFMutableStringRef> combined_string(
        CFStringCreateMutableCopy(
            kCFAllocatorDefault,
            0,
            left_string.get()
        )
    );

    if (!combined_string) {
        return 0.0;
    }

    CFStringAppend(combined_string.get(), right_string.get());

    // Create the CoreText font attributes.
    const void* keys[] = {
        kCTFontAttributeName
    };

    const void* values[] = {
        ct_font_.get()
    };

    CFObject<CFDictionaryRef> attributes(
        CFDictionaryCreate(
            kCFAllocatorDefault,
            keys,
            values,
            1,
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks
        )
    );

    if (!attributes) {
        return 0.0;
    }

    // Measure a string using CoreText.
    const auto measure = [&attributes](
        CFStringRef string
    ) -> double {

        CFObject<CFAttributedStringRef> attributed_string(
            CFAttributedStringCreate(
                kCFAllocatorDefault,
                string,
                attributes.get()
            )
        );

        if (!attributed_string) {
            return 0.0;
        }

        CFObject<CTLineRef> line(
            CTLineCreateWithAttributedString(
                attributed_string.get()
            )
        );

        if (!line) {
            return 0.0;
        }

        return CTLineGetTypographicBounds(
            line.get(),
            nullptr,
            nullptr,
            nullptr
        );
    };

    const double combined_width = measure(
        combined_string.get()
    );

    const double right_width = measure(
        right_string.get()
    );

    return combined_width - right_width;
}


Sized CocoaFont::textDimension(const char* str, int32_t byte_length) const  {
    if (!ct_font_ || str == nullptr) {
        return Sized{};
    }

    CFObject<CFStringRef> string(makeCFString(str, byte_length));
    if (!string) {
        return Sized{};
    }

    CFObject<CTLineRef> line(createLine(string.get(), ct_font_.get()));
    if (!line) {
        return Sized{};
    }

    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;

    const double width =
        CTLineGetTypographicBounds(
            line.get(),
            &ascent,
            &descent,
            &leading
        );

    return Sized(width, ascent + descent + leading);
}


int32_t CocoaFont::charIndexAtX(
    const char* str,
    double x,
    double& in_out_delta,
    double& out_cursor_x
) const {
    in_out_delta = 0.0;
    out_cursor_x = 0.0;

    if (!ct_font_ || str == nullptr) {
        return -1;
    }

    CFObject<CFStringRef> string(makeCFString(str, -1));
    if (!string) {
        return -1;
    }

    CFObject<CTLineRef> line(createLine(string.get(), ct_font_.get()));
    if (!line) {
        return -1;
    }

    CGFloat secondary_offset = 0.0;

    const CFIndex index =
        CTLineGetStringIndexForPosition(
            line.get(),
            CGPointMake(x, 0.0)
        );

    if (index >= 0) {
        out_cursor_x =
            CTLineGetOffsetForStringIndex(
                line.get(),
                index,
                &secondary_offset
            );
    }

    in_out_delta = secondary_offset;

    if (index < 0) {
        return -1;
    }

    return static_cast<int32_t>(index);
}


std::vector<String> availableFontNames() {
    std::vector<String> result;

    CFObject<CTFontCollectionRef> collection(
        CTFontCollectionCreateFromAvailableFonts(nullptr)
    );

    if (!collection) {
        return result;
    }

    CFObject<CFArrayRef> descriptors(
        CTFontCollectionCreateMatchingFontDescriptors(
            collection.get()
        )
    );

    if (!descriptors) {
        return result;
    }

    const CFIndex count = CFArrayGetCount(descriptors.get());

    result.reserve(static_cast<std::size_t>(count));

    for (CFIndex i = 0; i < count; ++i) {
        auto descriptor =
            static_cast<CTFontDescriptorRef>(
                const_cast<void*>(
                    CFArrayGetValueAtIndex(descriptors.get(), i)
                )
            );

        if (!descriptor) {
            continue;
        }

        CFObject<CFStringRef> name(
            static_cast<CFStringRef>(
                CTFontDescriptorCopyAttribute(
                    descriptor,
                    kCTFontNameAttribute
                )
            )
        );
        if (!name) {
            continue;
        }

        // Assuming String has a constructor from UTF-8.
        char buffer[1024];

        if (CFStringGetCString(
                name.get(),
                buffer,
                sizeof(buffer),
                kCFStringEncodingUTF8
            )
        ) {
            result.emplace_back(buffer);
        }
    }

    return result;
}


std::vector<String> availableFontFamilies() {
    std::vector<String> result;

    CFObject<CTFontCollectionRef> collection(
        CTFontCollectionCreateFromAvailableFonts(nullptr)
    );
    if (!collection) {
        return result;
    }

    CFObject<CFArrayRef> descriptors(
        CTFontCollectionCreateMatchingFontDescriptors(collection.get())
    );
    if (!descriptors) {
        return result;
    }

    const CFIndex count = CFArrayGetCount(descriptors.get());

    result.reserve(static_cast<std::size_t>(count));

    for (CFIndex i = 0; i < count; ++i) {
        auto descriptor =
            static_cast<CTFontDescriptorRef>(
                const_cast<void*>(
                    CFArrayGetValueAtIndex(descriptors.get(), i)
                )
            );

        CFObject<CFStringRef> family(
            static_cast<CFStringRef>(
                CTFontDescriptorCopyAttribute(
                    descriptor,
                    kCTFontFamilyNameAttribute
                )
            )
        );
        if (!family) {
            continue;
        }

        char buffer[1024];

        if (CFStringGetCString(
                family.get(),
                buffer,
                sizeof(buffer),
                kCFStringEncodingUTF8
            )
        ) {
            result.emplace_back(buffer);
        }
    }

    return result;
}


std::unique_ptr<Font> createFont(const String& name, float size) {
    return std::make_unique<CocoaFont>(name, size);
}


auto CocoaFont::buildPath(
    const char* text
) const -> FontPath {

    FontPath result;

    if (!ct_font_ || text == nullptr || text[0] == '\0') {
        return result;
    }

    CFObject<CFStringRef> cf_string(
        makeCFString(text, -1)
    );

    if (!cf_string) {
        return result;
    }

    const CFIndex char_count = CFStringGetLength(cf_string.get());

    if (char_count == 0) {
        return result;
    }

    const auto count = static_cast<std::size_t>(char_count);

    std::vector<UniChar> characters(count);
    std::vector<CGGlyph> glyphs(count);
    std::vector<CGSize> advances(count);
    std::vector<CGRect> glyph_bounds(count);

    CFStringGetCharacters(
        cf_string.get(),
        CFRangeMake(0, char_count),
        characters.data()
    );

    const CTFontRef ct_font = ct_font_.get();

    if (!CTFontGetGlyphsForCharacters(
            ct_font,
            characters.data(),
            glyphs.data(),
            char_count
        )) {
        return {};
    }

    CTFontGetAdvancesForGlyphs(
        ct_font,
        kCTFontOrientationHorizontal,
        glyphs.data(),
        advances.data(),
        char_count
    );

    CTFontGetBoundingRectsForGlyphs(
        ct_font,
        kCTFontOrientationHorizontal,
        glyphs.data(),
        glyph_bounds.data(),
        char_count
    );

    double cursor_x = 0.0;
    double cursor_y = 0.0;

    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double max_y = std::numeric_limits<double>::lowest();

    for (CFIndex i = 0; i < char_count; ++i) {
        const auto index = static_cast<std::size_t>(i);

        const CGRect& glyph_rect = glyph_bounds[index];

        min_x = std::min(
            min_x,
            cursor_x + glyph_rect.origin.x
        );

        min_y = std::min(
            min_y,
            cursor_y + glyph_rect.origin.y
        );

        max_x = std::max(
            max_x,
            cursor_x + glyph_rect.origin.x + glyph_rect.size.width
        );

        max_y = std::max(
            max_y,
            cursor_y + glyph_rect.origin.y + glyph_rect.size.height
        );

        CFObject<CGPathRef> glyph_path(
            CTFontCreatePathForGlyph(
                ct_font,
                glyphs[index],
                nullptr
            )
        );

        if (glyph_path) {
            CGPathContext context;
            context.path = &result;
            context.offset = { cursor_x, cursor_y };

            CGPathApply(
                glyph_path.get(),
                &context,
                applyCGPathElement
            );

            if (context.failed) {
                return {};
            }
        }

        cursor_x += advances[index].width;
        cursor_y += advances[index].height;
    }

    if (min_x != std::numeric_limits<double>::max()) {
        result.bounds.x = min_x;
        result.bounds.y = -max_y;
        result.bounds.width = max_x - min_x;
        result.bounds.height = max_y - min_y;
    }

    return result;
}

} // namespace Grain::Platform