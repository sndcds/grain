#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Size.hpp>
#include <grain/String/String.hpp>

#include <cstdint>
#include <memory>

namespace Grain::Platform {
class Font;
}

namespace Grain {

class Font {
public:
    explicit Font(float size);
    Font(const char* name, float size);
    Font(const String& name, float size);
    Font(const Font* font);
    Font(const Font* font, float size);

    ~Font();

    Font(Font&&) noexcept;
    auto operator = (Font&&) noexcept -> Font&;

    Font(const Font&) = delete;
    auto operator = (const Font&) -> Font& = delete;

    [[nodiscard]] auto fontNameUtf8() const -> const char*;
    [[nodiscard]] auto displayNameUtf8() const -> const char*;

    [[nodiscard]] auto size() const -> float;
    [[nodiscard]] auto isValid() const -> bool;

    void set(const String& name, float size);

    [[nodiscard]] auto ascent() const -> double;
    [[nodiscard]] auto descent() const -> double;
    [[nodiscard]] auto xHeight() const -> double;
    [[nodiscard]] auto capHeight() const -> double;
    [[nodiscard]] auto leading() const -> double;
    [[nodiscard]] auto cellHeight() const -> double;
    [[nodiscard]] auto lineHeight() const -> double;
    [[nodiscard]] auto italicAngle() const -> double;
    [[nodiscard]] auto underlinePosition() const -> double;
    [[nodiscard]] auto underlineThickness() const -> double;
    [[nodiscard]] auto unitsPerEm() const -> int32_t;
    [[nodiscard]] auto glyphCount() const -> int32_t;
    [[nodiscard]] auto boundingBox() const -> const Rectd&;

    [[nodiscard]] auto glyphAdvanceWidth(const char* symbol, int32_t length = -1 ) const -> double;

    [[nodiscard]] auto textDimension(const char* str, int32_t byte_length = -1) const -> Sized;

    [[nodiscard]] auto charIndexAtX(
        const char* str,
        double x,
        double& in_out_delta,
        double& out_cursor_x
    ) const -> int32_t;

    [[nodiscard]] auto nativeHandle() const -> void*;

private:
    std::unique_ptr<Platform::Font> platform_;
};

} // namespace Grain