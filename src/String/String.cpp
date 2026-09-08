#include "grain/String/String.hpp"

#include <algorithm>
#include <ostream>

namespace Grain {

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

String::String(const char* utf8)
    : data_(utf8 ? utf8 : "") {
}

String::String(std::string_view utf8)
    : data_(utf8) {
}

String::String(std::string utf8)
    : data_(std::move(utf8)) {
}

String& String::operator=(const char* utf8) {
    data_ = utf8 ? utf8 : "";
    return *this;
}

String& String::operator=(std::string_view utf8) {
    data_.assign(utf8);
    return *this;
}

// -----------------------------------------------------------------------------
// Basic properties
// -----------------------------------------------------------------------------

bool String::empty() const noexcept {
    return data_.empty();
}

String::size_type String::length() const noexcept {
    return codePointCount(data_);
}

String::size_type String::byteLength() const noexcept {
    return data_.size();
}

bool String::isAscii() const noexcept {
    return std::all_of(
        data_.begin(),
        data_.end(),
        [](unsigned char c) {
            return c < 0x80;
        });
}

bool String::isValidUtf8(size_type* invalid_byte_index) const noexcept {
    size_type index = 0;

    while (index < data_.size()) {
        char32_t code_point{};
        size_type sequence_length{};

        if (!decodeCodePoint(
            data_,
            index,
            code_point,
            sequence_length)) {

            if (invalid_byte_index) {
                *invalid_byte_index = index;
            }

            return false;
        }

        index += sequence_length;
    }

    if (invalid_byte_index) {
        *invalid_byte_index = npos;
    }

    return true;
}

// -----------------------------------------------------------------------------
// Access
// -----------------------------------------------------------------------------

const char* String::c_str() const noexcept {
    return data_.c_str();
}

std::string_view String::view() const noexcept {
    return data_;
}

char String::byteAt(size_type index) const noexcept {
    if (index >= data_.size()) {
        return '\0';
    }

    return data_[index];
}

char32_t String::codePointAt(size_type index) const noexcept {
    const size_type byte_index = byteOffset(index);

    if (byte_index == npos) {
        return 0xfffd;
    }

    char32_t code_point{};
    size_type sequence_length{};

    if (!decodeCodePoint(
        data_,
        byte_index,
        code_point,
        sequence_length)) {

        return 0xfffd;
    }

    return code_point;
}

String::size_type String::byteOffset(
    size_type code_point_index) const noexcept {
    return byteOffsetForCodePoint(
        data_,
        code_point_index);
}

// -----------------------------------------------------------------------------
// Modification
// -----------------------------------------------------------------------------

void String::clear() noexcept {
    data_.clear();
}

void String::assign(std::string_view utf8) {
    data_.assign(utf8);
}

void String::assign(const char* utf8) {
    data_ = utf8 ? utf8 : "";
}

void String::append(std::string_view utf8) {
    data_.append(utf8);
}

void String::append(const String& other) {
    data_.append(other.data_);
}

void String::append(const char* utf8) {
    if (utf8 != nullptr) {
        data_.append(utf8);
    }
}

String& String::operator+=(std::string_view utf8) {
    append(utf8);
    return *this;
}

String& String::operator+=(const String& other) {
    append(other);
    return *this;
}

String& String::operator+=(const char* utf8) {
    if (utf8 != nullptr) {
        data_.append(utf8);
    }

    return *this;
}

void String::insert(
    size_type code_point_index,
    std::string_view utf8) {
    const size_type byte_index = byteOffset(code_point_index);

    if (byte_index == npos) {
        if (code_point_index == length()) {
            data_.append(utf8);
        }
        return;
    }

    data_.insert(byte_index, utf8);
}

void String::erase(
    size_type code_point_index,
    size_type code_point_count) {
    const size_type start = byteOffset(code_point_index);

    if (start == npos) {
        return;
    }

    const size_type end =
        code_point_count == npos
            ? data_.size()
            : byteOffset(code_point_index + code_point_count);

    const size_type byte_end =
        end == npos ? data_.size() : end;

    data_.erase(start, byte_end - start);
}

void String::replace(
    size_type code_point_index,
    size_type code_point_count,
    std::string_view replacement) {
    const size_type start = byteOffset(code_point_index);

    if (start == npos) {
        return;
    }

    const size_type end =
        byteOffset(code_point_index + code_point_count);

    const size_type byte_end =
        end == npos ? data_.size() : end;

    data_.replace(
        start,
        byte_end - start,
        replacement);
}

// -----------------------------------------------------------------------------
// Substrings
// -----------------------------------------------------------------------------

String String::substr(
    size_type code_point_index,
    size_type code_point_count) const {
    const size_type start = byteOffset(code_point_index);

    if (start == npos) {
        return {};
    }

    if (code_point_count == npos) {
        return String(
            std::string_view(data_).substr(start));
    }

    const size_type end =
        byteOffset(code_point_index + code_point_count);

    if (end == npos) {
        return String(
            std::string_view(data_).substr(start));
    }

    return String(
        std::string_view(data_).substr(
            start,
            end - start));
}

// -----------------------------------------------------------------------------
// Searching
// -----------------------------------------------------------------------------

String::size_type String::find(
    std::string_view needle,
    size_type code_point_index) const noexcept {
    const size_type start = byteOffset(code_point_index);

    if (start == npos) {
        return npos;
    }

    const size_type byte_index = data_.find(needle, start);

    if (byte_index == npos) {
        return npos;
    }

    return codePointIndexFromByteIndex(byte_index);
}

bool String::contains(std::string_view needle) const noexcept {
    return data_.find(needle) != std::string::npos;
}

bool String::startsWith(std::string_view prefix) const noexcept {
    return data_.starts_with(prefix);
}

bool String::endsWith(std::string_view suffix) const noexcept {
    return data_.ends_with(suffix);
}

// -------------------------------------------------------------------------
// Whitespace
// -------------------------------------------------------------------------

String::size_type String::leadingWhitespace() const noexcept {
    size_type count = 0;

    while (count < data_.size() &&
           isAsciiWhitespace(static_cast<unsigned char>(data_[count]))) {
        ++count;
    }

    return count;
}

String::size_type String::trailingWhitespace() const noexcept {
    size_type count = 0;

    while (count < data_.size() &&
           isAsciiWhitespace(
               static_cast<unsigned char>(data_[data_.size() - 1 - count]))) {
        ++count;
    }

    return count;
}

void String::trimLeft() {
    const size_type count = leadingWhitespace();

    if (count > 0) {
        data_.erase(0, count);
    }
}

void String::trimRight() {
    const size_type count = trailingWhitespace();

    if (count > 0) {
        data_.erase(data_.size() - count);
    }
}

void String::trim() {
    trimLeft();
    trimRight();
}

// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------

int String::compare(const String& other) const noexcept {
    return data_.compare(other.data_);
}

int String::compare(std::string_view other) const noexcept {
    return data_.compare(other);
}

int String::compare(const char* other) const noexcept {
    if (other == nullptr) {
        return 1;
    }

    return data_.compare(other);
}

bool String::equalsIgnoreCase(std::string_view other) const noexcept {
    if (data_.size() != other.size()) {
        return false;
    }

    for (size_type i = 0; i < data_.size(); ++i) {
        const unsigned char lhs =
            static_cast<unsigned char>(data_[i]);

        const unsigned char rhs =
            static_cast<unsigned char>(other[i]);

        const unsigned char lhs_lower =
            lhs >= 'A' && lhs <= 'Z' ? lhs + ('a' - 'A') : lhs;

        const unsigned char rhs_lower =
            rhs >= 'A' && rhs <= 'Z' ? rhs + ('a' - 'A') : rhs;

        if (lhs_lower != rhs_lower) {
            return false;
        }
    }

    return true;
}

bool operator==(const String& lhs, const String& rhs) noexcept {
    return lhs.data_ == rhs.data_;
}

bool operator==(const String& lhs, const char* rhs) noexcept {
    return lhs.view() == std::string_view(rhs ? rhs : "");
}

bool operator==(const char* lhs, const String& rhs) noexcept {
    return rhs == lhs;
}

bool operator!=(const String& lhs, const String& rhs) noexcept {
    return !(lhs == rhs);
}

bool operator!=(const String& lhs, const char* rhs) noexcept {
    return !(lhs == rhs);
}

bool operator!=(const char* lhs, const String& rhs) noexcept {
    return !(lhs == rhs);
}

bool operator==(const String& lhs, std::string_view rhs) noexcept {
    return lhs.data_ == rhs;
}

bool operator==(std::string_view lhs, const String& rhs) noexcept {
    return lhs == rhs.data_;
}

bool operator!=(const String& lhs, std::string_view rhs) noexcept {
    return !(lhs == rhs);
}

bool operator!=(std::string_view lhs, const String& rhs) noexcept {
    return !(lhs == rhs);
}

String operator+(const String& lhs, const String& rhs) {
    String result;
    result.data_.reserve(
        lhs.data_.size() + rhs.data_.size());

    result.data_ = lhs.data_;
    result.data_ += rhs.data_;

    return result;
}

String operator+(const String& lhs, std::string_view rhs) {
    String result;
    result.data_.reserve(
        lhs.data_.size() + rhs.size());

    result.data_ = lhs.data_;
    result.data_ += rhs;

    return result;
}

String operator+(std::string_view lhs, const String& rhs) {
    String result;
    result.data_.reserve(
        lhs.size() + rhs.data_.size());

    result.data_ = lhs;
    result.data_ += rhs.data_;

    return result;
}

String operator+(const String& lhs, const char* rhs) {
    String result(lhs);
    result.append(rhs);
    return result;
}

String operator+(const char* lhs, const String& rhs) {
    String result(lhs);
    result.append(rhs);
    return result;
}

std::ostream& operator<<(std::ostream& stream, const String& string) {
    return stream << string.data_;
}

// -----------------------------------------------------------------------------
// UTF-8 implementation
// -----------------------------------------------------------------------------

bool String::isUtf8ContinuationByte(unsigned char byte) noexcept {
    return (byte & 0xc0) == 0x80;
}

String::size_type String::sequenceLength(unsigned char first_byte) noexcept {
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

bool String::decodeCodePoint(
    std::string_view data,
    size_type byte_index,
    char32_t& code_point,
    size_type& sequence_length) noexcept {
    if (byte_index >= data.size()) {
        return false;
    }

    const auto first =
        static_cast<unsigned char>(data[byte_index]);

    const size_type length = sequenceLength(first);

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
            static_cast<unsigned char>(data[byte_index + i]);

        if (!isUtf8ContinuationByte(byte)) {
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

    // UTF-16 surrogate range is not valid Unicode scalar data.
    if (value >= 0xd800 && value <= 0xdfff) {
        return false;
    }

    // Unicode ends at U+10FFFF.
    if (value > 0x10ffff) {
        return false;
    }

    code_point = value;
    sequence_length = length;
    return true;
}

String::size_type String::codePointCount(
    std::string_view data) noexcept {
    size_type count = 0;
    size_type index = 0;

    while (index < data.size()) {
        char32_t code_point{};
        size_type sequence_length{};

        if (!decodeCodePoint(
            data,
            index,
            code_point,
            sequence_length)) {

            // Keep the operation deterministic for malformed UTF-8:
            // treat the invalid byte as one unit.
            ++index;
        } else {
            index += sequence_length;
        }

        ++count;
    }

    return count;
}

String::size_type String::byteOffsetForCodePoint(
    std::string_view data,
    size_type code_point_index) noexcept {
    if (code_point_index == 0) {
        return 0;
    }

    size_type character_index = 0;
    size_type byte_index = 0;

    while (byte_index < data.size()) {
        if (character_index == code_point_index) {
            return byte_index;
        }

        char32_t code_point{};
        size_type sequence_length{};

        if (!decodeCodePoint(
            data,
            byte_index,
            code_point,
            sequence_length)) {

            ++byte_index;
        } else {
            byte_index += sequence_length;
        }

        ++character_index;
    }

    return character_index == code_point_index
               ? byte_index
               : npos;
}

String::size_type String::codePointIndexFromByteIndex(
    size_type byte_index
    ) const noexcept {
    if (byte_index > data_.size()) {
        return npos;
    }

    size_type code_point_index = 0;

    for (size_type i = 0; i < byte_index;) {
        const unsigned char byte =
            static_cast<unsigned char>(data_[i]);

        if (byte < 0x80) {
            i += 1;
        } else if ((byte & 0xE0) == 0xC0) {
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            i += 4;
        } else {
            return npos;
        }

        ++code_point_index;
    }

    return code_point_index;
}

bool String::isAsciiWhitespace(unsigned char byte) noexcept {
    switch (byte) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
    case '\f':
    case '\v':
        return true;

    default:
        return false;
    }
}

} // namespace Grain