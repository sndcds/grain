#include "grain/String/StringUtf8.hpp"

namespace Grain {

// -----------------------------------------------------------------------------
// Validation
// -----------------------------------------------------------------------------

bool StringUtf8::isValid(
    std::string_view data,
    size_type* invalid_byte_index) noexcept {

    size_type byte_index = 0;

    while (byte_index < data.size()) {
        char32_t code_point{};
        size_type sequence_length{};

        if (!decode(
            data,
            byte_index,
            code_point,
            sequence_length)) {

            if (invalid_byte_index) {
                *invalid_byte_index = byte_index;
            }

            return false;
        }

        byte_index += sequence_length;
    }

    if (invalid_byte_index) {
        *invalid_byte_index = npos;
    }

    return true;
}

bool StringUtf8::isAscii(
    std::string_view data) noexcept {

    for (const unsigned char byte : data) {
        if (byte >= 0x80) {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------
// Code-point access
// -----------------------------------------------------------------------------

StringUtf8::size_type StringUtf8::length(
    std::string_view data) noexcept {

    size_type count = 0;
    size_type byte_index = 0;

    while (byte_index < data.size()) {
        char32_t code_point{};
        size_type sequence_length{};

        if (!decode(
            data,
            byte_index,
            code_point,
            sequence_length)) {

            // Treat an invalid byte as one deterministic unit.
            ++byte_index;
        } else {
            byte_index += sequence_length;
        }

        ++count;
    }

    return count;
}

bool StringUtf8::decode(
    std::string_view data,
    size_type byte_index,
    char32_t& code_point,
    size_type& sequence_length) noexcept {

    if (byte_index >= data.size()) {
        return false;
    }

    const auto first =
        static_cast<unsigned char>(data[byte_index]);

    const size_type length =
        sequenceLength(first);

    if (length == 0 ||
        byte_index + length > data.size()) {
        return false;
    }

    if (length == 1) {
        code_point = first;
        sequence_length = 1;
        return true;
    }

    char32_t value = 0;

    switch (length) {
    case 2:
        value = first & 0x1f;
        break;

    case 3:
        value = first & 0x0f;
        break;

    case 4:
        value = first & 0x07;
        break;

    default:
        return false;
    }

    for (size_type i = 1; i < length; ++i) {
        const auto byte =
            static_cast<unsigned char>(
                data[byte_index + i]);

        if (!isContinuationByte(byte)) {
            return false;
        }

        value = (value << 6) | (byte & 0x3f);
    }

    // Reject overlong encodings.
    if ((length == 2 && value < 0x80) ||
        (length == 3 && value < 0x800) ||
        (length == 4 && value < 0x10000)) {
        return false;
    }

    // Reject UTF-16 surrogate code points.
    if (value >= 0xd800 && value <= 0xdfff) {
        return false;
    }

    // Unicode currently ends at U+10FFFF.
    if (value > 0x10ffff) {
        return false;
    }

    code_point = value;
    sequence_length = length;

    return true;
}

char32_t StringUtf8::codePointAt(
    std::string_view data,
    size_type code_point_index) noexcept {

    const size_type byte_index =
        byteOffset(data, code_point_index);

    if (byte_index == npos) {
        return replacementCharacter;
    }

    char32_t code_point{};
    size_type sequence_length{};

    if (!decode(
        data,
        byte_index,
        code_point,
        sequence_length)) {
        return replacementCharacter;
    }

    return code_point;
}

StringUtf8::size_type StringUtf8::byteOffset(
    std::string_view data,
    size_type code_point_index) noexcept {

    if (code_point_index == 0) {
        return 0;
    }

    size_type current_index = 0;
    size_type byte_index = 0;

    while (byte_index < data.size()) {
        if (current_index == code_point_index) {
            return byte_index;
        }

        char32_t code_point{};
        size_type sequence_length{};

        if (!decode(
            data,
            byte_index,
            code_point,
            sequence_length)) {
            ++byte_index;
        } else {
            byte_index += sequence_length;
        }

        ++current_index;
    }

    return current_index == code_point_index
               ? byte_index
               : npos;
}

StringUtf8::size_type StringUtf8::codePointIndex(
    std::string_view data,
    size_type byte_index) noexcept {

    if (byte_index > data.size()) {
        return npos;
    }

    if (byte_index == 0) {
        return 0;
    }

    size_type code_point_index = 0;
    size_type current_byte = 0;

    while (current_byte < data.size()) {
        if (current_byte == byte_index) {
            return code_point_index;
        }

        char32_t code_point{};
        size_type sequence_length{};

        if (!decode(
            data,
            current_byte,
            code_point,
            sequence_length)) {

            // Invalid byte sequences are treated as one byte/unit.
            ++current_byte;
        } else {
            const size_type next_byte =
                current_byte + sequence_length;

            // byte_index lies inside this UTF-8 sequence.
            if (byte_index < next_byte) {
                return npos;
            }

            current_byte = next_byte;
        }

        ++code_point_index;
    }

    // data.size() is a valid boundary.
    return current_byte == byte_index
               ? code_point_index
               : npos;
}

// -----------------------------------------------------------------------------
// UTF-8 sequence helpers
// -----------------------------------------------------------------------------

bool StringUtf8::isContinuationByte(
    unsigned char byte) noexcept {

    return (byte & 0xc0) == 0x80;
}

StringUtf8::size_type StringUtf8::sequenceLength(
    unsigned char first_byte) noexcept {

    if (first_byte < 0x80) {
        return 1;
    }

    if ((first_byte & 0xe0) == 0xc0) {
        return 2;
    }

    if ((first_byte & 0xf0) == 0xe0) {
        return 3;
    }

    if ((first_byte & 0xf8) == 0xf0) {
        return 4;
    }

    return 0;
}

} // namespace Grain