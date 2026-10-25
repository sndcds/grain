#include "grain/String/String.hpp"
#include "grain/String/StringUtf8.hpp"

#include <ostream>
#include <algorithm>
#include <cstring>
#include <memory>

namespace Grain {

auto String::isAsciiAtIndex(size_type code_point_index, char& out_char) const -> bool {
    const auto offset = byteOffset(code_point_index);
    if (offset >= data_.size()) {
        return false;
    }

    const auto byte = static_cast<unsigned char>(data_[offset]);
    if (byte > 0x7F) {
        return false;
    }

    out_char = static_cast<char>(byte);
    return true;
}


//------------------------------------------------------------------------------
//  Construction.
//------------------------------------------------------------------------------

String::String(const char* utf8)
    : data_((utf8 != nullptr) ? utf8 : "") {
}

String::String(std::string_view utf8)
    : data_(utf8) {
}

String::String(std::string utf8)
    : data_(std::move(utf8)) {
}

auto String::operator = (const char* utf8) -> String& {
    data_ = (utf8 != nullptr) ? utf8 : "";
    return *this;
}

auto String::operator = (std::string_view utf8) -> String& {
    data_.assign(utf8);
    return *this;
}


//------------------------------------------------------------------------------
//  Basic properties.
//------------------------------------------------------------------------------

auto String::empty() const -> bool {
    return data_.empty();
}

auto String::length() const -> String::size_type {
    return StringUtf8::length(data_);
}

auto String::byteLength() const -> String::size_type {
    return data_.size();
}

auto String::isAscii() const -> bool {
    return StringUtf8::isAscii(data_);
}

auto String::isValidUtf8(size_type* invalid_byte_index) const -> bool {
    return StringUtf8::isValid(data_, invalid_byte_index);
}


//------------------------------------------------------------------------------
//  Access.
//------------------------------------------------------------------------------

auto String::c_str() const -> const char* {
    return data_.c_str();
}

auto String::view() const -> std::string_view {
    return data_;
}

auto String::byteAt(size_type index) const -> char {
    if (index >= data_.size()) {
        return '\0';
    }

    return data_[index];
}

auto String::codePointAt(size_type index) const -> char32_t {
    return StringUtf8::codePointAt(data_, index);
}

auto String::byteOffset(size_type code_point_index) const -> String::size_type {
    return StringUtf8::byteOffset(data_, code_point_index);
}


//------------------------------------------------------------------------------
//  Modification.
//------------------------------------------------------------------------------

void String::clear() {
    data_.clear();
}

void String::assign(std::string_view utf8) {
    data_.assign(utf8);
}

void String::assign(const char* utf8) {
    data_ = (utf8 != nullptr) ? utf8 : "";
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

auto String::operator += (std::string_view utf8) -> String& {
    append(utf8);
    return *this;
}

auto String::operator += (const String& other) -> String& {
    append(other);
    return *this;
}

auto String::operator += (const char* utf8) -> String& {
    if (utf8 != nullptr) {
        data_.append(utf8);
    }

    return *this;
}

void String::insert(size_type code_point_index, std::string_view utf8) {
    const size_type byte_index = byteOffset(code_point_index);
    if (byte_index == npos) {
        if (code_point_index == length()) {
            data_.append(utf8);
        }

        return;
    }

    data_.insert(byte_index, utf8);
}


void String::insertAtCharacterIndex(const char* text, int32_t index) {
    if (text == nullptr || *text == '\0') {
        return;
    }

    const size_type character_index =
        index < 0
            ? 0
            : static_cast<size_type>(index);

    insert(character_index, std::string_view(text));
}


void String::erase(size_type code_point_index, size_type code_point_count) {
    const size_type start = byteOffset(code_point_index);
    if (start == npos) {
        return;
    }

    const size_type end =
        code_point_count == npos
            ? data_.size()
            : byteOffset(code_point_index + code_point_count);

    const size_type byte_end =
        end == npos
            ? data_.size()
            : end;

    data_.erase(
        start,
        byte_end - start);
}

void String::replace(
    size_type code_point_index,
    size_type code_point_count,
    std::string_view replacement
) {
    const size_type start = byteOffset(code_point_index);
    if (start == npos) {
        return;
    }

    const size_type end = byteOffset(code_point_index + code_point_count);

    const size_type byte_end =
        end == npos
            ? data_.size()
            : end;

    data_.replace(start, byte_end - start, replacement);
}


//------------------------------------------------------------------------------
//  Substrings.
//------------------------------------------------------------------------------

auto String::substr(
    size_type code_point_index,
    size_type code_point_count
) const -> String {
    const size_type start = byteOffset(code_point_index);
    if (start == npos) {
        return {};
    }

    if (code_point_count == npos) {
        return {std::string_view(data_).substr(start)};
    }

    const size_type end = byteOffset(code_point_index + code_point_count);

    if (end == npos) {
        return {std::string_view(data_).substr(start)};
    }

    return {std::string_view(data_).substr(start,end - start)};
}


//------------------------------------------------------------------------------
//  Searching.
//------------------------------------------------------------------------------

auto String::find(
    std::string_view needle,
    size_type code_point_index
) const -> String::size_type {
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

auto String::contains(std::string_view needle) const -> bool {
    return data_.contains(needle);
}

auto String::startsWith(std::string_view prefix) const -> bool {
    return data_.starts_with(prefix);
}

auto String::endsWith(std::string_view suffix) const -> bool {
    return data_.ends_with(suffix);
}


//------------------------------------------------------------------------------
//  Whitespace.
//------------------------------------------------------------------------------

auto String::leadingWhitespace() const -> String::size_type {
    size_type count = 0;

    while (
        count < data_.size() &&
        isAsciiWhitespace(
            static_cast<unsigned char>(
                data_[count]))
    ) {
        ++count;
    }

    return count;
}

auto String::trailingWhitespace() const -> String::size_type {
    size_type count = 0;

    while (
        count < data_.size() &&
        isAsciiWhitespace(
            static_cast<unsigned char>(
                data_[data_.size() - 1 - count]))
    ) {
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
        data_.erase(
            data_.size() - count);
    }
}

void String::trim() {
    trimLeft();
    trimRight();
}


//------------------------------------------------------------------------------
//  Comparison.
//------------------------------------------------------------------------------

auto String::compare(const String& other) const -> int {
    return data_.compare(other.data_);
}

auto String::compare(std::string_view other) const -> int {
    return data_.compare(other);
}

auto String::compare(const char* other) const -> int {
    if (other == nullptr) {
        return 1;
    }

    return data_.compare(other);
}

auto String::equalsIgnoreCase(std::string_view other) const -> bool {
    if (data_.size() != other.size()) {
        return false;
    }

    for (size_type i = 0; i < data_.size(); ++i) {
        const auto lhs = static_cast<unsigned char>(data_[i]);
        const auto rhs = static_cast<unsigned char>(other[i]);

        const unsigned char lhs_lower =
            lhs >= 'A' && lhs <= 'Z'
                ? lhs + ('a' - 'A')
                : lhs;

        const unsigned char rhs_lower =
            rhs >= 'A' && rhs <= 'Z'
                ? rhs + ('a' - 'A')
                : rhs;

        if (lhs_lower != rhs_lower) {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
//  Operators.
//------------------------------------------------------------------------------

auto operator == (const String& lhs, const String& rhs) -> bool {
    return lhs.data_ == rhs.data_;
}

auto operator == (const String& lhs, const char* rhs) -> bool {
    return lhs.view() == std::string_view((rhs != nullptr) ? rhs : "");
}

auto operator == (const char* lhs, const String& rhs) -> bool {
    return rhs == lhs;
}

auto operator != (const String& lhs, const String& rhs) -> bool {
    return !(lhs == rhs);
}

auto operator != (const String& lhs, const char* rhs) -> bool {
    return !(lhs == rhs);
}

auto operator != (const char* lhs, const String& rhs) -> bool {
    return !(lhs == rhs);
}

auto operator == (const String& lhs, std::string_view rhs) -> bool {
    return std::string_view{lhs.data_} == rhs;
}

auto operator == (std::string_view lhs, const String& rhs) -> bool {
    return lhs == std::string_view{rhs.data_};
}

auto operator != (const String& lhs, std::string_view rhs) -> bool {
    return !(lhs == rhs);
}

auto operator != (std::string_view lhs, const String& rhs) -> bool {
    return !(lhs == rhs);
}

auto operator + (const String& lhs, const String& rhs) -> String {
    String result;

    result.data_.reserve(lhs.data_.size() + rhs.data_.size());

    result.data_ = lhs.data_;
    result.data_ += rhs.data_;

    return result;
}

auto operator + (const String& lhs, std::string_view rhs) -> String {
    String result;

    result.data_.reserve(
        lhs.data_.size() +
        rhs.size());

    result.data_ = lhs.data_;
    result.data_ += rhs;

    return result;
}

auto operator + (std::string_view lhs, const String& rhs) -> String {
    String result;

    result.data_.reserve(
        lhs.size() +
        rhs.data_.size());

    result.data_ = lhs;
    result.data_ += rhs.data_;

    return result;
}

auto operator + (const String& lhs, const char* rhs) -> String {
    String result(lhs);
    result.append(rhs);

    return result;
}

auto operator + (const char* lhs, const String& rhs) -> String {
    String result(lhs);
    result.append(rhs);

    return result;
}

auto operator << (std::ostream& stream,const String& string) -> std::ostream& {
    return stream << string.data_;
}


//------------------------------------------------------------------------------
//  Internal helpers.
//------------------------------------------------------------------------------

auto String::codePointIndexFromByteIndex(size_type byte_index) const -> String::size_type {
    return StringUtf8::codePointIndex(data_, byte_index);
}

auto String::isAsciiWhitespace(unsigned char byte) -> bool {
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