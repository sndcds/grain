#pragma once

#include <cstdint>

namespace Grain {

class TextLayout {
public:
    virtual ~TextLayout() = default;

    TextLayout(const TextLayout&) = delete;
    auto operator = (const TextLayout&) -> TextLayout& = delete;

    TextLayout(TextLayout&&) = delete;
    auto operator = (TextLayout&&) -> TextLayout& = delete;

    [[nodiscard]] virtual auto width() const -> double = 0;
    [[nodiscard]] virtual auto ascent() const -> double = 0;
    [[nodiscard]] virtual auto descent() const -> double = 0;

    // Horizontal offset of a string index from the line origin.
    [[nodiscard]] virtual auto offsetForIndex(int32_t index) const -> double = 0;

protected:
    TextLayout() = default;
};

} // namespace Grain