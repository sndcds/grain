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

Font::Font(Font&&) noexcept = default;

auto Font::operator = (Font&&) noexcept -> Font& = default;


auto Font::fontNameUtf8() const -> const char* {
    return platform_->fontNameUtf8();
}

auto Font::displayNameUtf8() const -> const char* {
    return platform_->displayNameUtf8();
}

auto Font::size() const -> float {
    return platform_->size();
}

auto Font::isValid() const -> bool {
    return platform_->isValid();
}

void Font::set(const String& name, float size) {
    platform_->set(name, size);
}


auto Font::ascent() const -> double {
    return platform_->ascent();
}

auto Font::descent() const -> double {
    return platform_->descent();
}

auto Font::xHeight() const -> double {
    return platform_->xHeight();
}

auto Font::capHeight() const -> double {
    return platform_->capHeight();
}

auto Font::leading() const -> double {
    return platform_->leading();
}

auto Font::cellHeight() const -> double {
    return platform_->cellHeight();
}

auto Font::lineHeight() const -> double {
    return platform_->lineHeight();
}

auto Font::italicAngle() const -> double {
    return platform_->italicAngle();
}

auto Font::underlinePosition() const -> double {
    return platform_->underlinePosition();
}

auto Font::underlineThickness() const -> double {
    return platform_->underlineThickness();
}

auto Font::unitsPerEm() const -> int32_t {
    return platform_->unitsPerEm();
}

auto Font::glyphCount() const -> int32_t {
    return platform_->glyphCount();
}

auto Font::boundingBox() const -> const Rectd& {
    return platform_->boundingBox();
}

auto Font::glyphAdvanceWidth(const char* symbol, int32_t length) const -> double {
    return platform_->glyphAdvanceWidth(symbol, length);
}

auto Font::textDimension(const char* str, int32_t byte_length) const -> Sized {
    return platform_->textDimension(str, byte_length);
}

auto Font::charIndexAtX(
    const char* str,
    double x,
    double& in_out_delta,
    double& out_cursor_x
) const -> int32_t {
    return platform_->charIndexAtX(
        str,
        x,
        in_out_delta,
        out_cursor_x);
}

[[nodiscard]] auto Font::nativeHandle() const -> void* {
    return platform_
        ? platform_->nativeHandle()
        : nullptr;
}


} // namespace Grain