#include "grain/String/String.hpp"

#include "grain/String/StringUtf8.hpp"

#include <ostream>

namespace Grain {

//------------------------------------------------------------------------------
// Construction
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
// Basic properties
//------------------------------------------------------------------------------

bool String::empty() const noexcept {
    return data_.empty();
}

String::size_type String::length() const noexcept {
    return StringUtf8::length(data_);
}

String::size_type String::byteLength() const noexcept {
    return data_.size();
}

bool String::isAscii() const noexcept {
    return StringUtf8::isAscii(data_);
}

bool String::isValidUtf8(
    size_type* invalid_byte_index) const noexcept {
    return StringUtf8::isValid(
        data_,
        invalid_byte_index);
}

//------------------------------------------------------------------------------
// Access
//------------------------------------------------------------------------------

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
    return StringUtf8::codePointAt(data_, index);
}

String::size_type String::byteOffset(
    size_type code_point_index) const noexcept {
    return StringUtf8::byteOffset(
        data_,
        code_point_index);
}

//------------------------------------------------------------------------------
// Modification
//------------------------------------------------------------------------------

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

    const size_type byte_index =
        byteOffset(code_point_index);

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

    const size_type start =
        byteOffset(code_point_index);

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
    std::string_view replacement) {

    const size_type start =
        byteOffset(code_point_index);

    if (start == npos) {
        return;
    }

    const size_type end =
        byteOffset(code_point_index + code_point_count);

    const size_type byte_end =
        end == npos
            ? data_.size()
            : end;

    data_.replace(
        start,
        byte_end - start,
        replacement);
}

//------------------------------------------------------------------------------
// Substrings
//------------------------------------------------------------------------------

String String::substr(
    size_type code_point_index,
    size_type code_point_count) const {

    const size_type start =
        byteOffset(code_point_index);

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

//------------------------------------------------------------------------------
// Searching
//------------------------------------------------------------------------------

String::size_type String::find(
    std::string_view needle,
    size_type code_point_index) const noexcept {

    const size_type start =
        byteOffset(code_point_index);

    if (start == npos) {
        return npos;
    }

    const size_type byte_index =
        data_.find(needle, start);

    if (byte_index == npos) {
        return npos;
    }

    return codePointIndexFromByteIndex(byte_index);
}

bool String::contains(
    std::string_view needle) const noexcept {
    return data_.find(needle) != std::string::npos;
}

bool String::startsWith(
    std::string_view prefix) const noexcept {
    return data_.starts_with(prefix);
}

bool String::endsWith(
    std::string_view suffix) const noexcept {
    return data_.ends_with(suffix);
}

//------------------------------------------------------------------------------
// Whitespace
//------------------------------------------------------------------------------

String::size_type String::leadingWhitespace() const noexcept {
    size_type count = 0;

    while (
        count < data_.size() &&
        isAsciiWhitespace(
            static_cast<unsigned char>(data_[count]))) {

        ++count;
    }

    return count;
}

String::size_type String::trailingWhitespace() const noexcept {
    size_type count = 0;

    while (
        count < data_.size() &&
        isAsciiWhitespace(
            static_cast<unsigned char>(
                data_[data_.size() - 1 - count]))) {

        ++count;
    }

    return count;
}

void String::trimLeft() {
    const size_type count =
        leadingWhitespace();

    if (count > 0) {
        data_.erase(0, count);
    }
}

void String::trimRight() {
    const size_type count =
        trailingWhitespace();

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
// Comparison
//------------------------------------------------------------------------------

int String::compare(
    const String& other) const noexcept {
    return data_.compare(other.data_);
}

int String::compare(
    std::string_view other) const noexcept {
    return data_.compare(other);
}

int String::compare(
    const char* other) const noexcept {

    if (other == nullptr) {
        return 1;
    }

    return data_.compare(other);
}

bool String::equalsIgnoreCase(
    std::string_view other) const noexcept {

    if (data_.size() != other.size()) {
        return false;
    }

    for (size_type i = 0; i < data_.size(); ++i) {
        const unsigned char lhs =
            static_cast<unsigned char>(data_[i]);

        const unsigned char rhs =
            static_cast<unsigned char>(other[i]);

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
// Operators
//------------------------------------------------------------------------------

bool operator==(
    const String& lhs,
    const String& rhs) noexcept {
    return lhs.data_ == rhs.data_;
}

bool operator==(
    const String& lhs,
    const char* rhs) noexcept {
    return lhs.view() ==
           std::string_view(rhs ? rhs : "");
}

bool operator==(
    const char* lhs,
    const String& rhs) noexcept {
    return rhs == lhs;
}

bool operator!=(
    const String& lhs,
    const String& rhs) noexcept {
    return !(lhs == rhs);
}

bool operator!=(
    const String& lhs,
    const char* rhs) noexcept {
    return !(lhs == rhs);
}

bool operator!=(
    const char* lhs,
    const String& rhs) noexcept {
    return !(lhs == rhs);
}

bool operator==(
    const String& lhs,
    std::string_view rhs) noexcept {
    return lhs.data_ == rhs;
}

bool operator==(
    std::string_view lhs,
    const String& rhs) noexcept {
    return lhs == rhs.data_;
}

bool operator!=(
    const String& lhs,
    std::string_view rhs) noexcept {
    return !(lhs == rhs);
}

bool operator!=(
    std::string_view lhs,
    const String& rhs) noexcept {
    return !(lhs == rhs);
}

String operator+(
    const String& lhs,
    const String& rhs) {

    String result;

    result.data_.reserve(
        lhs.data_.size() +
        rhs.data_.size());

    result.data_ = lhs.data_;
    result.data_ += rhs.data_;

    return result;
}

String operator+(
    const String& lhs,
    std::string_view rhs) {

    String result;

    result.data_.reserve(
        lhs.data_.size() +
        rhs.size());

    result.data_ = lhs.data_;
    result.data_ += rhs;

    return result;
}

String operator+(
    std::string_view lhs,
    const String& rhs) {

    String result;

    result.data_.reserve(
        lhs.size() +
        rhs.data_.size());

    result.data_ = lhs;
    result.data_ += rhs.data_;

    return result;
}

String operator+(
    const String& lhs,
    const char* rhs) {

    String result(lhs);
    result.append(rhs);

    return result;
}

String operator+(
    const char* lhs,
    const String& rhs) {

    String result(lhs);
    result.append(rhs);

    return result;
}

std::ostream& operator<<(
    std::ostream& stream,
    const String& string) {

    return stream << string.data_;
}

//------------------------------------------------------------------------------
// Internal helpers
//------------------------------------------------------------------------------

String::size_type String::codePointIndexFromByteIndex(
    size_type byte_index) const noexcept {

    return StringUtf8::codePointIndex(
        data_,
        byte_index);
}

bool String::isAsciiWhitespace(
    unsigned char byte) noexcept {

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