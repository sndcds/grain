
#import <AppKit/AppKit.h>
#import <CoreText/CoreText.h>

#include "CoreTextLayout.hpp"

#include <algorithm>
#include <string_view>

namespace Grain {
namespace {

CFObject<CFStringRef> createCFString(const String& text) {
    const std::string_view bytes = text.view();

    return CFObject<CFStringRef>(
        CFStringCreateWithBytes(
            kCFAllocatorDefault,
            reinterpret_cast<const UInt8*>(bytes.data()),
            static_cast<CFIndex>(bytes.size()),
            kCFStringEncodingUTF8,
            false
        )
    );
}

CFObject<CFAttributedStringRef> createAttributedString(
    const String& text,
    const Font& font)
{
    auto string = createCFString(text);

    if (!string) {
        return {};
    }

    CTFontRef native_font =
        static_cast<CTFontRef>(font.nativeHandle());

    CFObject<CTFontRef> fallback_font;

    if (native_font == nullptr) {
        fallback_font.reset(
            CTFontCreateUIFontForLanguage(
                kCTFontUIFontSystem,
                font.size(),
                nullptr
            )
        );

        native_font = fallback_font.get();
    }

    if (native_font == nullptr) {
        return {};
    }

    const void* keys[] = {
        kCTFontAttributeName
    };

    const void* values[] = {
        native_font
    };

    CFObject<CFDictionaryRef> attributes(
        CFDictionaryCreate(
            kCFAllocatorDefault,
            keys,
            values,
            1,
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks
        )
    );

    if (!attributes) {
        return {};
    }

    return CFObject<CFAttributedStringRef>(
        CFAttributedStringCreate(
            kCFAllocatorDefault,
            string.get(),
            attributes.get()
        )
    );
}

} // namespace


CoreTextLayout::CoreTextLayout(
    const String& text,
    const Font& font)
{
    auto attributed_string = createAttributedString(text, font);

    if (!attributed_string) {
        return;
    }

    line_.reset(
        CTLineCreateWithAttributedString(
            attributed_string.get()
        )
    );

    // Build the mapping from UTF-8 code-point indices to UTF-16
    // indices used by Core Text.
    string_indices_.clear();
    string_indices_.push_back(0);

    const std::string_view bytes = text.view();

    CFIndex utf16_index = 0;

    for (std::size_t i = 0; i < bytes.size();) {
        const auto lead =
            static_cast<unsigned char>(bytes[i]);

        std::size_t byte_count = 1;
        uint32_t code_point = lead;

        if ((lead & 0xE0) == 0xC0) {
            byte_count = 2;
            code_point = lead & 0x1F;
        }
        else if ((lead & 0xF0) == 0xE0) {
            byte_count = 3;
            code_point = lead & 0x0F;
        }
        else if ((lead & 0xF8) == 0xF0) {
            byte_count = 4;
            code_point = lead & 0x07;
        }

        if (i + byte_count > bytes.size()) {
            byte_count = 1;
            code_point = 0xFFFD;
        }
        else {
            for (std::size_t j = 1; j < byte_count; ++j) {
                const auto continuation =
                    static_cast<unsigned char>(bytes[i + j]);

                if ((continuation & 0xC0) != 0x80) {
                    byte_count = 1;
                    code_point = 0xFFFD;
                    break;
                }

                code_point =
                    (code_point << 6) | (continuation & 0x3F);
            }
        }

        utf16_index += code_point > 0xFFFF ? 2 : 1;
        i += byte_count;
        string_indices_.push_back(utf16_index);
    }
}


double CoreTextLayout::width() const {
    if (!line_) {
        return 0.0;
    }

    return CTLineGetTypographicBounds(
        line_.get(),
        nullptr,
        nullptr,
        nullptr
    );
}


double CoreTextLayout::ascent() const {
    if (!line_) {
        return 0.0;
    }

    double value = 0.0;

    CTLineGetTypographicBounds(
        line_.get(),
        &value,
        nullptr,
        nullptr
    );

    return value;
}


double CoreTextLayout::descent() const {
    if (!line_) {
        return 0.0;
    }

    double value = 0.0;

    CTLineGetTypographicBounds(
        line_.get(),
        nullptr,
        &value,
        nullptr
    );

    return value;
}


double CoreTextLayout::offsetForIndex(int32_t index) const {
    if (!line_ || string_indices_.empty()) {
        return 0.0;
    }

    const auto code_point_index = static_cast<std::size_t>(
        std::clamp<int64_t>(
            index,
            0,
            static_cast<int64_t>(string_indices_.size() - 1)
        )
    );

    const CFIndex utf16_index =
        string_indices_[code_point_index];

    return CTLineGetOffsetForStringIndex(
        line_.get(),
        utf16_index,
        nullptr
    );
}

} // namespace Grain
