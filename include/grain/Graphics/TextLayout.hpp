#pragma once

#include <cstdint>

namespace Grain {

class TextLayout {
public:
    virtual ~TextLayout() = default;

    TextLayout(const TextLayout&) = delete;
    TextLayout& operator=(const TextLayout&) = delete;

    TextLayout(TextLayout&&) = delete;
    TextLayout& operator=(TextLayout&&) = delete;

    [[nodiscard]] virtual double width() const = 0;
    [[nodiscard]] virtual double ascent() const = 0;
    [[nodiscard]] virtual double descent() const = 0;

    // Horizontal offset of a string index from the line origin.
    [[nodiscard]] virtual double offsetForIndex(
        int32_t index
    ) const = 0;

protected:
    TextLayout() = default;
};

} // namespace Grain