#pragma once

#include <grain/Geometry/Rect.hpp>
#include <grain/Geometry/Size.hpp>
#include <grain/String/String.hpp>

#include <cstdint>
#include <memory>

namespace Grain::Platform {

class Font {
public:
    virtual ~Font() noexcept = default;

    [[nodiscard]]
    virtual const char* fontNameUtf8() const noexcept = 0;

    [[nodiscard]]
    virtual const char* displayNameUtf8() const noexcept = 0;

    [[nodiscard]]
    virtual float size() const noexcept = 0;

    [[nodiscard]]
    virtual bool isValid() const noexcept = 0;

    virtual void set(
        const String& name,
        float size) noexcept = 0;

    [[nodiscard]]
    virtual double ascent() const noexcept = 0;

    [[nodiscard]]
    virtual double descent() const noexcept = 0;

    [[nodiscard]]
    virtual double xHeight() const noexcept = 0;

    [[nodiscard]]
    virtual double capHeight() const noexcept = 0;

    [[nodiscard]]
    virtual double leading() const noexcept = 0;

    [[nodiscard]]
    virtual double cellHeight() const noexcept = 0;

    [[nodiscard]]
    virtual double lineHeight() const noexcept = 0;

    [[nodiscard]]
    virtual double italicAngle() const noexcept = 0;

    [[nodiscard]]
    virtual double underlinePosition() const noexcept = 0;

    [[nodiscard]]
    virtual double underlineThickness() const noexcept = 0;

    [[nodiscard]]
    virtual int32_t unitsPerEm() const noexcept = 0;

    [[nodiscard]]
    virtual int32_t glyphCount() const noexcept = 0;

    [[nodiscard]]
    virtual const Rectd& boundingBox() const noexcept = 0;

    [[nodiscard]]
    virtual double glyphAdvanceWidth(
        const char* symbol,
        int32_t length = -1) const noexcept = 0;

    [[nodiscard]]
    virtual Sized textDimension(
        const char* str,
        int32_t byte_length = -1) const noexcept = 0;

    [[nodiscard]]
    virtual int32_t charIndexAtX(
        const char* str,
        double x,
        double& in_out_delta,
        double& out_cursor_x) const noexcept = 0;

    [[nodiscard]]
    virtual void* nativeHandle() const noexcept = 0;
};

[[nodiscard]]
std::unique_ptr<Font> createFont(
    const String& name,
    float size) noexcept;

} // namespace Grain::Platform