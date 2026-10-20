
#pragma once

#include <CoreText/CoreText.h>

#include <grain/Graphics/TextLayout.hpp>
#include <grain/Graphics/Font.hpp>
#include <grain/String/String.hpp>

#include "CFObject.hpp"

#include <vector>

namespace Grain {

class CoreTextLayout final : public TextLayout {
public:
    CoreTextLayout(
        const String& text,
        const Font& font
    );

    ~CoreTextLayout() override = default;

    [[nodiscard]] double width() const override;
    [[nodiscard]] double ascent() const override;
    [[nodiscard]] double descent() const override;

    [[nodiscard]] double offsetForIndex(int32_t index) const override;

    [[nodiscard]] CTLineRef nativeLine() const noexcept {
        return line_.get();
    }

private:
    CFObject<CTLineRef> line_;

    // Maps Grain UTF-8 code-point indices to Core Text UTF-16 indices.
    std::vector<CFIndex> string_indices_;
};

} // namespace Grain
