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
    explicit Font(float size) noexcept;

    Font(const char* name, float size) noexcept;

    Font(const String& name, float size) noexcept;

    Font(const Font* font) noexcept;

    Font(const Font* font, float size) noexcept;

    ~Font() noexcept;

    Font(Font&&) noexcept;
    Font& operator=(Font&&) noexcept;

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    [[nodiscard]]
    const char* fontNameUtf8() const noexcept;

    [[nodiscard]]
    const char* displayNameUtf8() const noexcept;

    [[nodiscard]]
    float size() const noexcept;

    [[nodiscard]]
    bool isValid() const noexcept;

    void set(
        const String& name,
        float size) noexcept;

    [[nodiscard]]
    double ascent() const noexcept;

    [[nodiscard]]
    double descent() const noexcept;

    [[nodiscard]]
    double xHeight() const noexcept;

    [[nodiscard]]
    double capHeight() const noexcept;

    [[nodiscard]]
    double leading() const noexcept;

    [[nodiscard]]
    double cellHeight() const noexcept;

    [[nodiscard]]
    double lineHeight() const noexcept;

    [[nodiscard]]
    double italicAngle() const noexcept;

    [[nodiscard]]
    double underlinePosition() const noexcept;

    [[nodiscard]]
    double underlineThickness() const noexcept;

    [[nodiscard]]
    int32_t unitsPerEm() const noexcept;

    [[nodiscard]]
    int32_t glyphCount() const noexcept;

    [[nodiscard]]
    const Rectd& boundingBox() const noexcept;

    [[nodiscard]]
    double glyphAdvanceWidth(
        const char* symbol,
        int32_t length = -1) const noexcept;

    [[nodiscard]]
    Sized textDimension(
        const char* str,
        int32_t byte_length = -1) const noexcept;

    [[nodiscard]]
    int32_t charIndexAtX(
        const char* str,
        double x,
        double& in_out_delta,
        double& out_cursor_x) const noexcept;

    [[nodiscard]]
    void* nativeHandle() const noexcept;

private:
    std::unique_ptr<Platform::Font> platform_;
};

} // namespace Grain