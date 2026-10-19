#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <string_view>

namespace Grain {

class String {
public:
    using size_type = std::size_t;

    static constexpr size_type npos = std::string_view::npos;

    //--------------------------------------------------------------------------
    // Construction
    //--------------------------------------------------------------------------

    String() = default;

    String(const char* utf8);

    String(std::string_view utf8);

    String(std::string utf8);

    String(const String&) = default;

    String(String&&) = default;

    String& operator=(const String&) = default;

    String& operator=(String&&) = default;

    String& operator=(const char* utf8);

    String& operator=(std::string_view utf8);

    ~String() = default;


    //--------------------------------------------------------------------------
    // Basic properties
    //--------------------------------------------------------------------------

    [[nodiscard]] bool empty() const;

    [[nodiscard]] bool isEmpty() const {
        return empty();
    }

    /**
     * Number of UTF-8 code points.
     *
     * This is intentionally called `length()` rather than `size()` because
     * `size()` traditionally means the number of stored bytes for std::string.
     */
    [[nodiscard]] size_type length() const;

    /**
     * Number of bytes used by the UTF-8 representation.
     */
    [[nodiscard]] size_type byteLength() const;

    /**
     * Returns true if every byte belongs to the ASCII range.
     */
    [[nodiscard]] bool isAscii() const;

    /**
     * Checks whether the stored byte sequence is valid UTF-8.
     */
    [[nodiscard]] bool isValidUtf8(
        size_type* invalid_byte_index = nullptr) const;

    [[nodiscard]]
        bool isAsciiAtIndex(
            size_type code_point_index,
            char& out_char
        ) const;

    //--------------------------------------------------------------------------
    // Access
    //--------------------------------------------------------------------------

    /**
     * Null-terminated UTF-8 string.
     *
     * The returned pointer remains valid until the String is modified.
     */
    [[nodiscard]] const char* c_str() const;

    /**
     * Alias used by the existing Grain API.
     */
    [[nodiscard]] const char* utf8() const {
        return c_str();
    }

    /**
     * Read-only view of the underlying UTF-8 bytes.
     */
    [[nodiscard]] std::string_view view() const;

    /**
     * Direct read-only byte access.
     *
     * This is intentionally byte-based, not Unicode-character-based.
     */
    [[nodiscard]] char byteAt(size_type index) const;

    /**
     * Returns the Unicode code point at `index`.
     *
     * Returns U+FFFD if the index is invalid or the sequence is malformed.
     */
    [[nodiscard]] char32_t codePointAt(size_type index) const;

    /**
     * Returns the byte offset corresponding to a code-point index.
     */
    [[nodiscard]] size_type byteOffset(
        size_type code_point_index) const;

    //--------------------------------------------------------------------------
    // Modification
    //--------------------------------------------------------------------------

    void clear();

    void assign(std::string_view utf8);

    void assign(const char* utf8);

    void append(std::string_view utf8);

    void append(const String& other);

    void append(const char* utf8);

    String& operator+=(std::string_view utf8);

    String& operator+=(const String& other);

    String& operator+=(const char* utf8);

    void insert(size_type code_point_index, std::string_view utf8);
    void insertAtCharacterIndex(const char* text, int32_t index);

    void erase(
        size_type code_point_index,
        size_type code_point_count = 1);

    void replace(
        size_type code_point_index,
        size_type code_point_count,
        std::string_view replacement);

    //--------------------------------------------------------------------------
    // Substrings
    //--------------------------------------------------------------------------

    /**
     * Creates a substring using UTF-8 code-point indices.
     */
    [[nodiscard]] String substr(
        size_type code_point_index,
        size_type code_point_count = npos) const;

    //--------------------------------------------------------------------------
    // Searching
    //--------------------------------------------------------------------------

    /**
     * Finds `needle` and returns its UTF-8 code-point index.
     *
     * Returns `npos` if it isn't found.
     */
    [[nodiscard]] size_type find(
        std::string_view needle,
        size_type code_point_index = 0) const;

    [[nodiscard]] bool contains(
        std::string_view needle) const;

    [[nodiscard]] bool startsWith(
        std::string_view prefix) const;

    [[nodiscard]] bool endsWith(
        std::string_view suffix) const;

    //--------------------------------------------------------------------------
    // Whitespace
    //--------------------------------------------------------------------------

    [[nodiscard]] size_type leadingWhitespace() const;

    [[nodiscard]] size_type trailingWhitespace() const;

    void trim();

    void trimLeft();

    void trimRight();

    //--------------------------------------------------------------------------
    // Comparison
    //--------------------------------------------------------------------------

    [[nodiscard]] int compare(const String& other) const;

    [[nodiscard]] int compare(std::string_view other) const;

    [[nodiscard]] int compare(const char* other) const;

    [[nodiscard]] bool equalsIgnoreCase(
        std::string_view other) const;

    //--------------------------------------------------------------------------
    // Operators
    //--------------------------------------------------------------------------

    friend bool operator==(
        const String& lhs,
        const String& rhs);

    friend bool operator==(
        const String& lhs,
        const char* rhs);

    friend bool operator==(
        const char* lhs,
        const String& rhs);

    friend bool operator!=(
        const String& lhs,
        const String& rhs);

    friend bool operator!=(
        const String& lhs,
        const char* rhs);

    friend bool operator!=(
        const char* lhs,
        const String& rhs);

    friend bool operator==(
        const String& lhs,
        std::string_view rhs);

    friend bool operator==(
        std::string_view lhs,
        const String& rhs);

    friend bool operator!=(
        const String& lhs,
        std::string_view rhs);

    friend bool operator!=(
        std::string_view lhs,
        const String& rhs);

    friend String operator+(
        const String& lhs,
        const String& rhs);

    friend String operator+(
        const String& lhs,
        std::string_view rhs);

    friend String operator+(
        std::string_view lhs,
        const String& rhs);

    friend String operator+(
        const String& lhs,
        const char* rhs);

    friend String operator+(
        const char* lhs,
        const String& rhs);

    friend std::ostream& operator<<(
        std::ostream& stream,
        const String& string);

private:
    std::string data_;

    [[nodiscard]] size_type codePointIndexFromByteIndex(
        size_type byte_index) const;

    [[nodiscard]] static bool isAsciiWhitespace(
        unsigned char byte);

    void trimRange(
        size_type begin,
        size_type end);
};

} // namespace Grain