#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Grain {

class StringUtf8 {
public:
    using size_type = std::size_t;

    static constexpr char32_t replacementCharacter = 0xfffd;
    static constexpr size_type npos = std::string_view::npos;

    // -------------------------------------------------------------------------
    // Validation
    // -------------------------------------------------------------------------

    [[nodiscard]] static bool isValid(
        std::string_view data,
        size_type* invalid_byte_index = nullptr) noexcept;

    [[nodiscard]] static bool isAscii(
        std::string_view data) noexcept;

    // -------------------------------------------------------------------------
    // Code-point access
    // -------------------------------------------------------------------------

    [[nodiscard]] static size_type length(
        std::string_view data) noexcept;

    [[nodiscard]] static bool decode(
        std::string_view data,
        size_type byte_index,
        char32_t& code_point,
        size_type& sequence_length) noexcept;

    [[nodiscard]] static char32_t codePointAt(
        std::string_view data,
        size_type code_point_index) noexcept;

    [[nodiscard]] static size_type byteOffset(
        std::string_view data,
        size_type code_point_index) noexcept;

    /**
     * Returns the code-point index corresponding to a byte offset.
     *
     * The byte index must point to the beginning of a UTF-8 sequence,
     * or to data.size(). If it points into the middle of a sequence,
     * npos is returned.
     */
    [[nodiscard]] static size_type codePointIndex(
        std::string_view data,
        size_type byte_index) noexcept;

    // -------------------------------------------------------------------------
    // UTF-8 sequence helpers
    // -------------------------------------------------------------------------

    [[nodiscard]] static bool isContinuationByte(
        unsigned char byte) noexcept;

    [[nodiscard]] static size_type sequenceLength(
        unsigned char first_byte) noexcept;
};

} // namespace Grain