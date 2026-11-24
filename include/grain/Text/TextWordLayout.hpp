
#pragma once

#include <grain/Text/Font.hpp>

#include <cstdint>
#include <string>
#include <vector>


namespace Grain {


class TextWordLayout {

public:

    struct Word {
        std::string text;
        double x = 0.0;
        double y = 0.0;
        double width = 0.0;
        double height = 0.0;
    };

    struct Line {
        int32_t firstWord = 0;
        int32_t wordCount = 0;
        double width = 0.0;
    };

    struct Result {
        std::vector<Line> lines;

        double width = 0.0;
        double height = 0.0;

        [[nodiscard]]
        auto lineCount() const noexcept -> int32_t {
            return static_cast<int32_t>(lines.size());
        }

        [[nodiscard]]
        auto line(int32_t index) const noexcept -> const Line* {
            if (index < 0 ||
                static_cast<std::size_t>(index) >= lines.size()
            ) {
                return nullptr;
            }

            return &lines[static_cast<std::size_t>(index)];
        }
    };


    TextWordLayout() = default;

    explicit TextWordLayout(
        const char* text,
        const Font* font
    ) noexcept;


    void setFont(const Font* font) noexcept;

    void setText(const char* text) noexcept;

    void addWord(const char* text) noexcept;

    void splitWords() noexcept;

    void measureWords() noexcept;

    void setDimensions(
        double width,
        double height
    ) noexcept;


    void layout() noexcept;

    void layout(
        double width,
        double height
    ) noexcept;


    [[nodiscard]]
    auto result() const noexcept -> const Result& {
        return result_;
    }

    [[nodiscard]]
    auto fits() const noexcept -> bool {
        return fits_;
    }

    [[nodiscard]]
    auto wordCount() const noexcept -> int32_t {
        return static_cast<int32_t>(words_.size());
    }

    [[nodiscard]]
    auto word(int32_t index) const noexcept -> const Word* {
        if (index < 0 || std::cmp_greater_equal(index ,words_.size())) {
            return nullptr;
        }

        return &words_[static_cast<std::size_t>(index)];
    }

    [[nodiscard]]
    auto dimensionWidth() const noexcept -> double {
        return dimensionWidth_;
    }

    [[nodiscard]]
    auto dimensionHeight() const noexcept -> double {
        return dimensionHeight_;
    }


private:

    [[nodiscard]]
    auto wrap(
        double width,
        double spaceWidth,
        double lineHeight,
        bool store,
        double& outLongest
    ) noexcept -> int32_t;

    void clearResult() noexcept;


    std::string text_;
    std::vector<Word> words_;

    const Font* font_ = nullptr;

    double dimensionWidth_ = 0.0;
    double dimensionHeight_ = 0.0;

    Result result_;

    bool fits_ = false;
};


} // namespace Grain
