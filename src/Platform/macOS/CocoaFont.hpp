#pragma once

#include <grain/Platform/Font.hpp>
#include "CFObject.hpp"

#include <CoreText/CoreText.h>

namespace Grain {
class Font;
}

namespace Grain::Platform {

class CocoaFont final : public Font {
public:
    CocoaFont(
        const String& name,
        float size) noexcept;

    ~CocoaFont() noexcept = default;

    CocoaFont(const CocoaFont&) = delete;
    CocoaFont& operator=(const CocoaFont&) = delete;

    [[nodiscard]]
    const char* fontNameUtf8() const noexcept override;

    [[nodiscard]]
    const char* displayNameUtf8() const noexcept override;

    [[nodiscard]]
    float size() const noexcept override;

    [[nodiscard]]
    bool isValid() const noexcept override;

    void resetMetrics() noexcept;

    void set(
        const String& name,
        float size) noexcept override;

    [[nodiscard]]
    double ascent() const noexcept override;

    [[nodiscard]]
    double descent() const noexcept override;

    [[nodiscard]]
    double xHeight() const noexcept override;

    [[nodiscard]]
    double capHeight() const noexcept override;

    [[nodiscard]]
    double leading() const noexcept override;

    [[nodiscard]]
    double cellHeight() const noexcept override;

    [[nodiscard]]
    double lineHeight() const noexcept override;

    [[nodiscard]]
    double italicAngle() const noexcept override;

    [[nodiscard]]
    double underlinePosition() const noexcept override;

    [[nodiscard]]
    double underlineThickness() const noexcept override;

    [[nodiscard]]
    int32_t unitsPerEm() const noexcept override;

    [[nodiscard]]
    int32_t glyphCount() const noexcept override;

    [[nodiscard]]
    const Rectd& boundingBox() const noexcept override;

    [[nodiscard]]
    double glyphAdvanceWidth(
        const char* symbol,
        int32_t length = -1) const noexcept override;

    [[nodiscard]]
    Sized textDimension(
        const char* str,
        int32_t byte_length = -1) const noexcept override;

    [[nodiscard]]
    int32_t charIndexAtX(
        const char* str,
        double x,
        double& in_out_delta,
        double& out_cursor_x) const noexcept override;

    void* nativeHandle() const noexcept override {
        return const_cast<void*>(
            static_cast<const void*>(ct_font_.get()));
    }

private:
    void release() noexcept;
    void updateMetrics() noexcept;

    String font_name_;
    String display_name_;

    float font_size_ = 0.0f;
    bool is_valid_ = false;

    double ascent_ = 0.0;
    double descent_ = 0.0;
    double x_height_ = 0.0;
    double cap_height_ = 0.0;
    double leading_ = 0.0;
    double cell_height_ = 0.0;
    double line_height_ = 0.0;
    double italic_angle_ = 0.0;
    double underline_position_ = 0.0;
    double underline_thickness_ = 0.0;

    int32_t units_per_em_ = 0;
    int32_t glyph_count_ = 0;

    Rectd bounding_box_{};

    CFObject<CTFontRef> ct_font_;
};

} // namespace Grain::Platform