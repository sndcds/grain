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
    CocoaFont(const String& name, float size);

    ~CocoaFont() = default;

    CocoaFont(const CocoaFont&) = delete;
    CocoaFont& operator=(const CocoaFont&) = delete;

    [[nodiscard]] const char* fontNameUtf8() const override;

    [[nodiscard]] const char* displayNameUtf8() const override;

    [[nodiscard]] float size() const override;

    [[nodiscard]] bool isValid() const override;

    void resetMetrics();

    void set(const String& name, float size) override;

    [[nodiscard]] double ascent() const override;

    [[nodiscard]] double descent() const override;

    [[nodiscard]] double xHeight() const override;

    [[nodiscard]] double capHeight() const override;

    [[nodiscard]] double leading() const override;

    [[nodiscard]] double cellHeight() const override;

    [[nodiscard]] double lineHeight() const override;

    [[nodiscard]] double italicAngle() const override;

    [[nodiscard]] double underlinePosition() const override;

    [[nodiscard]] double underlineThickness() const override;

    [[nodiscard]] int32_t unitsPerEm() const override;

    [[nodiscard]] int32_t glyphCount() const override;

    [[nodiscard]] const Rectd& boundingBox() const override;

    [[nodiscard]] double glyphAdvanceWidth(
        const char* symbol,
        int32_t length = -1
    ) const override;

    [[nodiscard]] Sized textDimension(
        const char* str,
        int32_t byte_length = -1
    ) const override;

    [[nodiscard]] int32_t charIndexAtX(
        const char* str,
        double x,
        double& in_out_delta,
        double& out_cursor_x
    ) const override;

    void* nativeHandle() const override {
        return const_cast<void*>(
            static_cast<const void*>(ct_font_.get())
        );
    }

private:
    void release();
    void updateMetrics();

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