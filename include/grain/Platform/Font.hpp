#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Size.hpp>
#include <grain/String/String.hpp>

#include <memory>

namespace Grain::Platform {

class Font {
public:
    virtual ~Font() = default;

    [[nodiscard]] virtual auto fontNameUtf8() const -> const char* = 0;
    [[nodiscard]] virtual auto displayNameUtf8() const -> const char* = 0;

    [[nodiscard]] virtual auto size() const -> float = 0;
    [[nodiscard]] virtual auto isValid() const -> bool = 0;

    virtual void set(const String& name, float size) = 0;

    [[nodiscard]] virtual auto ascent() const -> double = 0;
    [[nodiscard]] virtual auto descent() const -> double = 0;
    [[nodiscard]] virtual auto xHeight() const -> double = 0;
    [[nodiscard]] virtual auto capHeight() const -> double = 0;
    [[nodiscard]] virtual auto leading() const -> double = 0;
    [[nodiscard]] virtual auto cellHeight() const -> double = 0;
    [[nodiscard]] virtual auto lineHeight() const -> double = 0;
    [[nodiscard]] virtual auto italicAngle() const -> double = 0;
    [[nodiscard]] virtual auto underlinePosition() const -> double = 0;
    [[nodiscard]] virtual auto underlineThickness() const -> double = 0;
    [[nodiscard]] virtual auto unitsPerEm() const -> int32_t = 0;
    [[nodiscard]] virtual auto glyphCount() const -> int32_t = 0;

    [[nodiscard]] virtual auto boundingBox() const -> const Rectd& = 0;

    [[nodiscard]] virtual auto glyphAdvanceWidth(
        const char* symbol,
        int32_t length
    ) const -> double = 0;

    [[nodiscard]] virtual auto textDimension(
        const char* str,
        int32_t byte_length
    ) const -> Sized = 0;

    [[nodiscard]] virtual auto charIndexAtX(
        const char* str,
        double x,
        double& in_out_delta,
        double& out_cursor_x
    ) const -> int32_t = 0;

    [[nodiscard]] virtual auto nativeHandle() const -> void* = 0;
};

[[nodiscard]] auto availableFontNames() -> std::vector<String>;
[[nodiscard]] auto availableFontFamilies() -> std::vector<String>;
[[nodiscard]] auto createFont(const String& name, float size) -> std::unique_ptr<Font>;

} // namespace Grain::Platform