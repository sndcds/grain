#include <grain/Graphics/Font.hpp>
#include <grain/Platform/Font.hpp>

#include <utility>

namespace Grain {

Font::Font(float size)
    : platform_(Platform::createFont(String(), size))
{
}

Font::Font(const char* name, float size)
    : platform_(Platform::createFont(String(name), size))
{
}

Font::Font(const String& name, float size)
    : platform_(Platform::createFont(name, size))
{
}

Font::Font(const Font* font)
    : platform_(
          font != nullptr
              ? Platform::createFont(
                    String(font->fontNameUtf8()),
                    font->size())
              : Platform::createFont(String(), 12.0f))
{
}

Font::Font(const Font* font, float size)
    : platform_(
          font != nullptr
              ? Platform::createFont(
                    String(font->fontNameUtf8()),
                    size)
              : Platform::createFont(String(), size)
    )
{
}

Font::~Font() = default;

Font::Font(Font&& other) = default;

Font& Font::operator=(Font&& other) = default;


const char* Font::fontNameUtf8() const {
    return platform_->fontNameUtf8();
}

const char* Font::displayNameUtf8() const {
    return platform_->displayNameUtf8();
}

float Font::size() const {
    return platform_->size();
}

bool Font::isValid() const {
    return platform_->isValid();
}

void Font::set(const String& name, float size) {
    platform_->set(name, size);
}


double Font::ascent() const {
    return platform_->ascent();
}

double Font::descent() const {
    return platform_->descent();
}

double Font::xHeight() const {
    return platform_->xHeight();
}

double Font::capHeight() const {
    return platform_->capHeight();
}

double Font::leading() const {
    return platform_->leading();
}

double Font::cellHeight() const {
    return platform_->cellHeight();
}

double Font::lineHeight() const {
    return platform_->lineHeight();
}

double Font::italicAngle() const {
    return platform_->italicAngle();
}

double Font::underlinePosition() const {
    return platform_->underlinePosition();
}

double Font::underlineThickness() const {
    return platform_->underlineThickness();
}

int32_t Font::unitsPerEm() const {
    return platform_->unitsPerEm();
}

int32_t Font::glyphCount() const {
    return platform_->glyphCount();
}

const Rectd& Font::boundingBox() const {
    return platform_->boundingBox();
}

double Font::glyphAdvanceWidth(
    const char* symbol,
    int32_t length
) const {
    return platform_->glyphAdvanceWidth(symbol, length);
}

Sized Font::textDimension(const char* str, int32_t byte_length) const {
    return platform_->textDimension(str, byte_length);
}

int32_t Font::charIndexAtX(
    const char* str,
    double x,
    double& in_out_delta,
    double& out_cursor_x
) const {
    return platform_->charIndexAtX(
        str,
        x,
        in_out_delta,
        out_cursor_x);
}

[[nodiscard]] void* Font::nativeHandle() const {
    return platform_
        ? platform_->nativeHandle()
        : nullptr;
}


} // namespace Grain