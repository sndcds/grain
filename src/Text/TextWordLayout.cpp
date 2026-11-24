
#include <grain/Text/TextWordLayout.hpp>

#include <algorithm>


namespace Grain {


TextWordLayout::TextWordLayout(
    const char* text,
    const Font* font
) noexcept
{
    setFont(font);
    setText(text);
}


//------------------------------------------------------------------------------
//  Configuration
//------------------------------------------------------------------------------

void TextWordLayout::setFont(const Font* font) noexcept {
    font_ = font;
    clearResult();
}


void TextWordLayout::setText(const char* text) noexcept {
    try {
        text_ = (text != nullptr) ? text : "";
        splitWords();
    }
    catch (...) {
        text_.clear();
        words_.clear();
        clearResult();
    }
}


void TextWordLayout::addWord(const char* text) noexcept {
    if (text == nullptr || *text == '\0') {
        return;
    }

    try {
        words_.push_back({.text=text});
        clearResult();
    }
    catch (...) {
        clearResult();
    }
}


void TextWordLayout::setDimensions(
    double width,
    double height
) noexcept
{
    dimensionWidth_ = width;
    dimensionHeight_ = height;

    clearResult();
}


void TextWordLayout::layout(
    double width,
    double height
) noexcept
{
    setDimensions(width, height);
    layout();
}


//------------------------------------------------------------------------------
//  Word splitting
//------------------------------------------------------------------------------

void TextWordLayout::splitWords() noexcept {
    words_.clear();
    clearResult();

    try {
        const char* p = text_.c_str();

        while (*p != '\0') {

            // Skip whitespace.
            while (*p != '\0' &&
                   (std::isspace(static_cast<unsigned char>(*p)) != 0)
            ) {
                ++p;
            }

            if (*p == '\0') {
                break;
            }

            const char* start = p;

            // Find the end of the word.
            while (*p != '\0' &&
                   (std::isspace(static_cast<unsigned char>(*p)) == 0)
            ) {
                ++p;
            }

            words_.push_back({
                .text=std::string(start, static_cast<std::size_t>(p - start))
            });
        }
    }
    catch (...) {
        words_.clear();
    }
}


//------------------------------------------------------------------------------
//  Word metrics
//------------------------------------------------------------------------------

void TextWordLayout::measureWords() noexcept {
    clearResult();

    if (font_ == nullptr) {
        return;
    }

    for (auto& word : words_) {

        const auto dimension = font_->textDimension(
            word.text.c_str(),
            static_cast<int32_t>(word.text.size())
        );

        word.width = dimension.width;
        word.height = dimension.height;

        if (!std::isfinite(word.width) ||
            !std::isfinite(word.height) ||
            word.width < 0.0 ||
            word.height < 0.0)
        {
            word.width = 0.0;
            word.height = 0.0;
            clearResult();
            return;
        }
    }
}


//------------------------------------------------------------------------------
//  Result
//------------------------------------------------------------------------------

void TextWordLayout::clearResult() noexcept {
    result_.lines.clear();

    result_.width = 0.0;
    result_.height = 0.0;

    fits_ = false;
}


//------------------------------------------------------------------------------
//  Greedy wrapping
//------------------------------------------------------------------------------

auto TextWordLayout::wrap(
    double width,
    double spaceWidth,
    double lineHeight,
    bool store,
    double& outLongest
) noexcept -> int32_t {

    outLongest = 0.0;

    int32_t lineCount = 0;
    int32_t index = 0;

    while (index < wordCount()) {

        const int32_t firstWord = index;

        double lineWidth = 0.0;

        while (index < wordCount()) {

            auto& word = words_[static_cast<std::size_t>(index)];

            const double x =
                index == firstWord
                    ? 0.0
                    : lineWidth + spaceWidth;

            const double nextWidth = x + word.width;

            // Always consume at least one word.
            // An overwide word occupies its own line.
            if (index > firstWord && nextWidth > width) {
                break;
            }

            if (store) {
                word.x = x;
                word.y = static_cast<double>(lineCount) * lineHeight;
            }

            lineWidth = nextWidth;
            ++index;
        }

        if (store) {

            try {
                result_.lines.push_back({
                    .firstWord=firstWord,
                    .wordCount=index - firstWord,
                    .width=lineWidth
                });
            }
            catch (...) {
                return -1;
            }
        }

        outLongest = std::max(outLongest, lineWidth);

        ++lineCount;
    }

    return lineCount;
}


//------------------------------------------------------------------------------
//  Layout
//------------------------------------------------------------------------------

void TextWordLayout::layout() noexcept {
    clearResult();

    if (font_ == nullptr) {
        return;
    }

    if (!std::isfinite(dimensionWidth_) ||
        dimensionWidth_ <= 0.0 ||
        !std::isfinite(dimensionHeight_) ||
        dimensionHeight_ < 0.0)
    {
        return;
    }

    if (words_.empty()) {
        fits_ = true;
        return;
    }

    measureWords();

    const auto spaceDimension = font_->textDimension(" ");

    const double spaceWidth = spaceDimension.width;

    double lineHeight = font_->lineHeight();

    if (!std::isfinite(spaceWidth) ||
        spaceWidth < 0.0 ||
        !std::isfinite(lineHeight) ||
        lineHeight < 0.0)
    {
        return;
    }

    for (const auto& word : words_) {
        lineHeight = std::max(lineHeight, word.height);
    }

    if (lineHeight <= 0.0) {
        return;
    }

    // Find the narrowest accepted width without increasing
    // the original greedy line count.
    int32_t previousLineCount = wordCount();

    double width = dimensionWidth_;
    double acceptedWidth = width;

    while (true) {

        double longest = 0.0;

        const int32_t count = wrap(
            width,
            spaceWidth,
            lineHeight,
            false,
            longest
        );

        if (count < 0) {
            return;
        }

        if (count > previousLineCount) {
            break;
        }

        previousLineCount = count;
        acceptedWidth = width;

        const double nextWidth = longest - (spaceWidth * 0.5);

        // No improvement is possible for single-word lines.
        if (count == wordCount() ||
            nextWidth <= 0.0 ||
            nextWidth >= width)
        {
            break;
        }

        width = nextWidth;
    }

    double longest = 0.0;

    const int32_t count = wrap(
        acceptedWidth,
        spaceWidth,
        lineHeight,
        true,
        longest
    );

    if (count < 0) {
        clearResult();
        return;
    }

    result_.width = longest;
    result_.height = static_cast<double>(count) * lineHeight;

    fits_ =
        result_.width <= dimensionWidth_ &&
        result_.height <= dimensionHeight_;
}


} // namespace Grain
