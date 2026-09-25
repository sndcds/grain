#include "CocoaFont.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreText/CoreText.h>

#include <vector>

namespace Grain::Platform {

namespace {

CFStringRef makeCFString(
    const char* str,
    int32_t length) noexcept
{
    if (str == nullptr) {
        return nullptr;
    }

    if (length < 0) {
        return CFStringCreateWithCString(
            kCFAllocatorDefault,
            str,
            kCFStringEncodingUTF8);
    }

    return CFStringCreateWithBytes(
        kCFAllocatorDefault,
        reinterpret_cast<const UInt8*>(str),
        length,
        kCFStringEncodingUTF8,
        false);
}

double safeCellHeight(
    double ascent,
    double descent,
    double leading) noexcept
{
    return ascent + descent + leading;
}

CTLineRef createLine(
    CFStringRef string,
    CTFontRef font) noexcept
{
    if (string == nullptr || font == nullptr) {
        return nullptr;
    }

    CFObject<CFMutableAttributedStringRef> attributes(
        CFAttributedStringCreateMutable(
            kCFAllocatorDefault,
            0));

    if (!attributes) {
        return nullptr;
    }

    CFAttributedStringReplaceString(
        attributes.get(),
        CFRangeMake(0, 0),
        string);

    CFAttributedStringSetAttribute(
        attributes.get(),
        CFRangeMake(
            0,
            CFStringGetLength(string)),
        kCTFontAttributeName,
        font);

    return CTLineCreateWithAttributedString(attributes.get());
}

} // namespace


CocoaFont::CocoaFont(
    const String& name,
    float size) noexcept
{
    set(name, size);
}


void CocoaFont::resetMetrics() noexcept {
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


void CocoaFont::set(
    const String& name,
    float size) noexcept
{
    ct_font_.reset();
    resetMetrics();

    font_size_ = size;

    const char* name_utf8 = name.utf8();

    if (name_utf8 == nullptr || *name_utf8 == '\0') {
        ct_font_.reset(
            CTFontCreateUIFontForLanguage(
                kCTFontSystemFontType,
                size,
                nullptr));
    }
    else {
        CFObject<CFStringRef> cfName(
            CFStringCreateWithCString(
                kCFAllocatorDefault,
                name_utf8,
                kCFStringEncodingUTF8));

        if (cfName) {
            ct_font_.reset(
                CTFontCreateWithName(
                    cfName.get(),
                    size,
                    nullptr));
        }
    }

    if (!ct_font_) {
        return;
    }

    is_valid_ = true;

    CFObject<CFStringRef> postscriptName(
        CTFontCopyPostScriptName(ct_font_.get()));

    if (postscriptName) {
        char buffer[1024];

        if (CFStringGetCString(
                postscriptName.get(),
                buffer,
                sizeof(buffer),
                kCFStringEncodingUTF8)) {
            font_name_ = String(buffer);
        }
    }

    CFObject<CFStringRef> displayName(
        CTFontCopyDisplayName(ct_font_.get()));

    if (displayName) {
        char buffer[1024];

        if (CFStringGetCString(
                displayName.get(),
                buffer,
                sizeof(buffer),
                kCFStringEncodingUTF8)) {
            display_name_ = String(buffer);
        }
    }

    updateMetrics();
}


void CocoaFont::updateMetrics() noexcept
{
    if (!ct_font_) {
        return;
    }

    const CTFontRef font = ct_font_.get();

    ascent_ = CTFontGetAscent(font);
    descent_ = CTFontGetDescent(font);
    leading_ = CTFontGetLeading(font);

    x_height_ = CTFontGetXHeight(font);
    cap_height_ = CTFontGetCapHeight(font);

    cell_height_ = safeCellHeight(
        ascent_,
        descent_,
        leading_);

    line_height_ = cell_height_;

    italic_angle_ = CTFontGetSlantAngle(font);

    underline_position_ =
        CTFontGetUnderlinePosition(font);

    underline_thickness_ =
        CTFontGetUnderlineThickness(font);

    units_per_em_ =
        static_cast<int32_t>(
            CTFontGetUnitsPerEm(font));

    glyph_count_ =
        static_cast<int32_t>(
            CTFontGetGlyphCount(font));

    const CGRect box =
        CTFontGetBoundingBox(font);

    bounding_box_ = Rectd(
        box.origin.x,
        box.origin.y,
        box.size.width,
        box.size.height);
}


const char* CocoaFont::fontNameUtf8() const noexcept
{
    return font_name_.utf8();
}


const char* CocoaFont::displayNameUtf8() const noexcept
{
    return display_name_.utf8();
}


float CocoaFont::size() const noexcept
{
    return font_size_;
}


bool CocoaFont::isValid() const noexcept
{
    return is_valid_;
}


double CocoaFont::ascent() const noexcept
{
    return ascent_;
}


double CocoaFont::descent() const noexcept
{
    return descent_;
}


double CocoaFont::xHeight() const noexcept
{
    return x_height_;
}


double CocoaFont::capHeight() const noexcept
{
    return cap_height_;
}


double CocoaFont::leading() const noexcept
{
    return leading_;
}


double CocoaFont::cellHeight() const noexcept
{
    return cell_height_;
}


double CocoaFont::lineHeight() const noexcept
{
    return line_height_;
}


double CocoaFont::italicAngle() const noexcept
{
    return italic_angle_;
}


double CocoaFont::underlinePosition() const noexcept
{
    return underline_position_;
}


double CocoaFont::underlineThickness() const noexcept
{
    return underline_thickness_;
}


int32_t CocoaFont::unitsPerEm() const noexcept
{
    return units_per_em_;
}


int32_t CocoaFont::glyphCount() const noexcept
{
    return glyph_count_;
}


const Rectd& CocoaFont::boundingBox() const noexcept
{
    return bounding_box_;
}


double CocoaFont::glyphAdvanceWidth(
    const char* symbol,
    int32_t length) const noexcept
{
    if (!ct_font_ || symbol == nullptr) {
        return 0.0;
    }

    CFObject<CFStringRef> string(
        makeCFString(symbol, length));

    if (!string) {
        return 0.0;
    }

    const CFIndex length_utf16 =
        CFStringGetLength(string.get());

    if (length_utf16 == 0) {
        return 0.0;
    }

    std::vector<UniChar> characters(
        static_cast<size_t>(length_utf16));

    CFStringGetCharacters(
        string.get(),
        CFRangeMake(0, length_utf16),
        characters.data());

    std::vector<CGGlyph> glyphs(
        static_cast<size_t>(length_utf16));

    if (!CTFontGetGlyphsForCharacters(
            ct_font_.get(),
            characters.data(),
            glyphs.data(),
            length_utf16)) {
        return 0.0;
    }

    std::vector<CGSize> advances(
        static_cast<size_t>(length_utf16));

    CTFontGetAdvancesForGlyphs(
        ct_font_.get(),
        kCTFontOrientationHorizontal,
        glyphs.data(),
        advances.data(),
        length_utf16);

    double width = 0.0;

    for (CFIndex i = 0; i < length_utf16; ++i) {
        width += advances[i].width;
    }

    return width;
}


Sized CocoaFont::textDimension(
    const char* str,
    int32_t byte_length) const noexcept
{
    if (!ct_font_ || str == nullptr) {
        return Sized{};
    }

    CFObject<CFStringRef> string(
        makeCFString(str, byte_length));

    if (!string) {
        return Sized{};
    }

    CFObject<CTLineRef> line(
        createLine(string.get(), ct_font_.get()));

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
            &leading);

    return Sized(
        width,
        ascent + descent + leading);
}


int32_t CocoaFont::charIndexAtX(
    const char* str,
    double x,
    double& in_out_delta,
    double& out_cursor_x) const noexcept
{
    in_out_delta = 0.0;
    out_cursor_x = 0.0;

    if (!ct_font_ || str == nullptr) {
        return -1;
    }

    CFObject<CFStringRef> string(
        makeCFString(str, -1));

    if (!string) {
        return -1;
    }

    CFObject<CTLineRef> line(
        createLine(string.get(), ct_font_.get()));

    if (!line) {
        return -1;
    }

    CGFloat secondary_offset = 0.0;

    const CFIndex index =
        CTLineGetStringIndexForPosition(
            line.get(),
            CGPointMake(x, 0.0));

    if (index >= 0) {
        out_cursor_x =
            CTLineGetOffsetForStringIndex(
                line.get(),
                index,
                &secondary_offset);
    }

    in_out_delta = secondary_offset;

    if (index < 0) {
        return -1;
    }

    return static_cast<int32_t>(index);
}


std::unique_ptr<Font> createFont(
    const String& name,
    float size) noexcept
{
    return std::make_unique<CocoaFont>(name, size);
}


} // namespace Grain::Platform