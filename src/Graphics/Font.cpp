#include <grain/Graphics/Font.hpp>
#include <grain/Platform/Font.hpp>

#include <utility>

namespace Grain {

Font::Font(float size) noexcept
    : platform_(Platform::createFont(String(), size))
{
}

Font::Font(const char* name, float size) noexcept
    : platform_(Platform::createFont(String(name), size))
{
}

Font::Font(const String& name, float size) noexcept
    : platform_(Platform::createFont(name, size))
{
}

Font::Font(const Font* font) noexcept
    : platform_(
          font != nullptr
              ? Platform::createFont(
                    String(font->fontNameUtf8()),
                    font->size())
              : Platform::createFont(String(), 12.0f))
{
}

Font::Font(const Font* font, float size) noexcept
    : platform_(
          font != nullptr
              ? Platform::createFont(
                    String(font->fontNameUtf8()),
                    size)
              : Platform::createFont(String(), size))
{
}

Font::~Font() noexcept = default;

Font::Font(Font&& other) noexcept = default;

Font& Font::operator=(Font&& other) noexcept = default;


const char* Font::fontNameUtf8() const noexcept
{
    return platform_->fontNameUtf8();
}

const char* Font::displayNameUtf8() const noexcept
{
    return platform_->displayNameUtf8();
}

float Font::size() const noexcept
{
    return platform_->size();
}

bool Font::isValid() const noexcept
{
    return platform_->isValid();
}

void Font::set(
    const String& name,
    float size) noexcept
{
    platform_->set(name, size);
}


double Font::ascent() const noexcept
{
    return platform_->ascent();
}

double Font::descent() const noexcept
{
    return platform_->descent();
}

double Font::xHeight() const noexcept
{
    return platform_->xHeight();
}

double Font::capHeight() const noexcept
{
    return platform_->capHeight();
}

double Font::leading() const noexcept
{
    return platform_->leading();
}

double Font::cellHeight() const noexcept
{
    return platform_->cellHeight();
}

double Font::lineHeight() const noexcept
{
    return platform_->lineHeight();
}

double Font::italicAngle() const noexcept
{
    return platform_->italicAngle();
}

double Font::underlinePosition() const noexcept
{
    return platform_->underlinePosition();
}

double Font::underlineThickness() const noexcept
{
    return platform_->underlineThickness();
}

int32_t Font::unitsPerEm() const noexcept
{
    return platform_->unitsPerEm();
}

int32_t Font::glyphCount() const noexcept
{
    return platform_->glyphCount();
}

const Rectd& Font::boundingBox() const noexcept
{
    return platform_->boundingBox();
}

double Font::glyphAdvanceWidth(
    const char* symbol,
    int32_t length) const noexcept
{
    return platform_->glyphAdvanceWidth(symbol, length);
}

Sized Font::textDimension(
    const char* str,
    int32_t byte_length) const noexcept
{
    return platform_->textDimension(str, byte_length);
}

int32_t Font::charIndexAtX(
    const char* str,
    double x,
    double& in_out_delta,
    double& out_cursor_x) const noexcept
{
    return platform_->charIndexAtX(
        str,
        x,
        in_out_delta,
        out_cursor_x);
}

[[nodiscard]]
void* Font::nativeHandle() const noexcept {
    return platform_
        ? platform_->nativeHandle()
        : nullptr;
}


} // namespace Grain