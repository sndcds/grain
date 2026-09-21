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

    CFMutableAttributedStringRef attributes =
        CFAttributedStringCreateMutable(
            kCFAllocatorDefault,
            0);

    if (attributes == nullptr) {
        return nullptr;
    }

    CFAttributedStringReplaceString(
        attributes,
        CFRangeMake(0, 0),
        string);

    CFAttributedStringSetAttribute(
        attributes,
        CFRangeMake(
            0,
            CFStringGetLength(string)),
        kCTFontAttributeName,
        font);

    CTLineRef line =
        CTLineCreateWithAttributedString(attributes);

    CFRelease(attributes);

    return line;
}

} // namespace


CocoaFont::CocoaFont(
    const String& name,
    float size) noexcept
{
    set(name, size);
}


CocoaFont::~CocoaFont() noexcept
{
    release();
}


void CocoaFont::release() noexcept
{
    if (ct_font_ != nullptr) {
        CFRelease(ct_font_);
        ct_font_ = nullptr;
    }

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
    release();

    font_size_ = size;

    const char* name_utf8 = name.utf8();

    if (name_utf8 == nullptr || *name_utf8 == '\0') {
        ct_font_ = CTFontCreateUIFontForLanguage(
            kCTFontSystemFontType,
            size,
            nullptr);
    }
    else {
        CFStringRef cf_name =
            CFStringCreateWithCString(
                kCFAllocatorDefault,
                name_utf8,
                kCFStringEncodingUTF8);

        if (cf_name != nullptr) {
            ct_font_ = CTFontCreateWithName(
                cf_name,
                size,
                nullptr);

            CFRelease(cf_name);
        }
    }

    if (ct_font_ == nullptr) {
        return;
    }

    is_valid_ = true;

    CFStringRef postscript_name =
        CTFontCopyPostScriptName(ct_font_);

    if (postscript_name != nullptr) {
        char buffer[1024];

        if (CFStringGetCString(
                postscript_name,
                buffer,
                sizeof(buffer),
                kCFStringEncodingUTF8)) {
            font_name_ = String(buffer);
        }

        CFRelease(postscript_name);
    }

    CFStringRef display_name =
        CTFontCopyDisplayName(ct_font_);

    if (display_name != nullptr) {
        char buffer[1024];

        if (CFStringGetCString(
                display_name,
                buffer,
                sizeof(buffer),
                kCFStringEncodingUTF8)) {
            display_name_ = String(buffer);
        }

        CFRelease(display_name);
    }

    updateMetrics();
}


void CocoaFont::updateMetrics() noexcept
{
    if (ct_font_ == nullptr) {
        return;
    }

    ascent_ = CTFontGetAscent(ct_font_);
    descent_ = CTFontGetDescent(ct_font_);
    leading_ = CTFontGetLeading(ct_font_);

    x_height_ = CTFontGetXHeight(ct_font_);
    cap_height_ = CTFontGetCapHeight(ct_font_);

    cell_height_ = safeCellHeight(
        ascent_,
        descent_,
        leading_);

    line_height_ = cell_height_;

    italic_angle_ = CTFontGetSlantAngle(ct_font_);

    underline_position_ =
        CTFontGetUnderlinePosition(ct_font_);

    underline_thickness_ =
        CTFontGetUnderlineThickness(ct_font_);

    units_per_em_ =
        static_cast<int32_t>(
            CTFontGetUnitsPerEm(ct_font_));

    glyph_count_ =
        static_cast<int32_t>(
            CTFontGetGlyphCount(ct_font_));

    const CGRect box =
        CTFontGetBoundingBox(ct_font_);

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
    if (ct_font_ == nullptr || symbol == nullptr) {
        return 0.0;
    }

    CFStringRef string =
        makeCFString(symbol, length);

    if (string == nullptr) {
        return 0.0;
    }

    const CFIndex length_utf16 =
        CFStringGetLength(string);

    if (length_utf16 == 0) {
        CFRelease(string);
        return 0.0;
    }

    std::vector<UniChar> characters(
        static_cast<size_t>(length_utf16));

    CFStringGetCharacters(
        string,
        CFRangeMake(0, length_utf16),
        characters.data());

    std::vector<CGGlyph> glyphs(
        static_cast<size_t>(length_utf16));

    if (!CTFontGetGlyphsForCharacters(
            ct_font_,
            characters.data(),
            glyphs.data(),
            length_utf16)) {
        CFRelease(string);
        return 0.0;
    }

    std::vector<CGSize> advances(
        static_cast<size_t>(length_utf16));

    CTFontGetAdvancesForGlyphs(
        ct_font_,
        kCTFontOrientationHorizontal,
        glyphs.data(),
        advances.data(),
        length_utf16);

    double width = 0.0;

    for (CFIndex i = 0; i < length_utf16; ++i) {
        width += advances[i].width;
    }

    CFRelease(string);

    return width;
}


Sized CocoaFont::textDimension(
    const char* str,
    int32_t byte_length) const noexcept
{
    if (ct_font_ == nullptr || str == nullptr) {
        return Sized{};
    }

    CFStringRef string =
        makeCFString(str, byte_length);

    if (string == nullptr) {
        return Sized{};
    }

    CTLineRef line =
        createLine(string, ct_font_);

    CFRelease(string);

    if (line == nullptr) {
        return Sized{};
    }

    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;

    const double width =
        CTLineGetTypographicBounds(
            line,
            &ascent,
            &descent,
            &leading);

    CFRelease(line);

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

    if (ct_font_ == nullptr || str == nullptr) {
        return -1;
    }

    CFStringRef string =
        makeCFString(str, -1);

    if (string == nullptr) {
        return -1;
    }

    CTLineRef line =
        createLine(string, ct_font_);

    CFRelease(string);

    if (line == nullptr) {
        return -1;
    }

    CGFloat secondary_offset = 0.0;

    const CFIndex index =
        CTLineGetStringIndexForPosition(
            line,
            CGPointMake(x, 0.0));

    if (index >= 0) {
        out_cursor_x =
            CTLineGetOffsetForStringIndex(
                line,
                index,
                nullptr);
    }

    in_out_delta = secondary_offset;

    CFRelease(line);

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