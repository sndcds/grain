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

    Font(Font&&);
    Font& operator=(Font&&);

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    [[nodiscard]] const char* fontNameUtf8() const;
    [[nodiscard]] const char* displayNameUtf8() const;

    [[nodiscard]] float size() const;
    [[nodiscard]] bool isValid() const;

    void set(const String& name, float size);

    [[nodiscard]] double ascent() const;
    [[nodiscard]] double descent() const;
    [[nodiscard]] double xHeight() const;
    [[nodiscard]] double capHeight() const;
    [[nodiscard]] double leading() const;
    [[nodiscard]] double cellHeight() const;
    [[nodiscard]] double lineHeight() const;
    [[nodiscard]] double italicAngle() const;
    [[nodiscard]] double underlinePosition() const;
    [[nodiscard]] double underlineThickness() const;
    [[nodiscard]] int32_t unitsPerEm() const;
    [[nodiscard]] int32_t glyphCount() const;
    [[nodiscard]] const Rectd& boundingBox() const;

    [[nodiscard]] double glyphAdvanceWidth(
        const char* symbol,
        int32_t length = -1
    ) const;

    [[nodiscard]] Sized textDimension(
        const char* str,
        int32_t byte_length = -1
    ) const;

    [[nodiscard]] int32_t charIndexAtX(
        const char* str,
        double x,
        double& in_out_delta,
        double& out_cursor_x
    ) const;

    [[nodiscard]] void* nativeHandle() const;

private:
    std::unique_ptr<Platform::Font> platform_;
};

} // namespace Grain