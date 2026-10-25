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

    //--------------------------------------------------------------------------
    //  Validation.
    //--------------------------------------------------------------------------

    [[nodiscard]] static auto isValid(
        std::string_view data,
        size_type* invalid_byte_index = nullptr
    ) -> bool;

    [[nodiscard]] static auto isAscii(std::string_view data) -> bool;


    //--------------------------------------------------------------------------
    //  Code-point access.
    //--------------------------------------------------------------------------

    [[nodiscard]] static auto length(std::string_view data) -> size_type;

    [[nodiscard]] static auto decode(
        std::string_view data,
        size_type byte_index,
        char32_t& code_point,
        size_type& sequence_length
    ) -> bool;

    [[nodiscard]] static auto codePointAt(
        std::string_view data,
        size_type code_point_index
    ) -> char32_t;

    [[nodiscard]] static auto byteOffset(
        std::string_view data,
        size_type code_point_index
    ) -> size_type;

    /**
     * Returns the code-point index corresponding to a byte offset.
     *
     * The byte index must point to the beginning of a UTF-8 sequence,
     * or to data.size(). If it points into the middle of a sequence,
     * npos is returned.
     */
    [[nodiscard]] static auto codePointIndex(
        std::string_view data,
        size_type byte_index
    ) -> size_type;


    //--------------------------------------------------------------------------
    //  UTF-8 sequence helpers.
    //--------------------------------------------------------------------------

    [[nodiscard]] static auto isContinuationByte(unsigned char byte) -> bool;
    [[nodiscard]] static auto sequenceLength(unsigned char first_byte) -> size_type;
};

} // namespace Grain