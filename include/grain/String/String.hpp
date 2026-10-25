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
    //  Construction.
    //--------------------------------------------------------------------------

    String() = default;

    String(const char* utf8);

    String(std::string_view utf8);

    String(std::string utf8);

    String(const String&) = default;

    String(String&&) = default;

    auto operator = (const String&) -> String& = default;

    auto operator = (String&&) -> String& = default;

    auto operator = (const char* utf8) -> String&;

    auto operator = (std::string_view utf8) -> String&;

    ~String() = default;


    //--------------------------------------------------------------------------
    //  Basic properties.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto empty() const -> bool;

    [[nodiscard]] auto isEmpty() const -> bool {
        return empty();
    }

    /**
     * Number of UTF-8 code points.
     *
     * This is intentionally called `length()` rather than `size()` because
     * `size()` traditionally means the number of stored bytes for std::string.
     */
    [[nodiscard]] auto length() const -> size_type;

    /**
     * Number of bytes used by the UTF-8 representation.
     */
    [[nodiscard]] auto byteLength() const -> size_type;

    /**
     * Returns true if every byte belongs to the ASCII range.
     */
    [[nodiscard]] auto isAscii() const -> bool;

    /**
     * Checks whether the stored byte sequence is valid UTF-8.
     */
    [[nodiscard]] auto isValidUtf8(
        size_type* invalid_byte_index = nullptr
    ) const -> bool;

    [[nodiscard]]auto isAsciiAtIndex(
        size_type code_point_index,
        char& out_char
    ) const -> bool;


    //--------------------------------------------------------------------------
    //  Access.
    //--------------------------------------------------------------------------

    /**
     * Null-terminated UTF-8 string.
     *
     * The returned pointer remains valid until the String is modified.
     */
    [[nodiscard]] auto c_str() const -> const char*;

    /**
     * Alias used by the existing Grain API.
     */
    [[nodiscard]] auto utf8() const -> const char* {
        return c_str();
    }

    /**
     * Read-only view of the underlying UTF-8 bytes.
     */
    [[nodiscard]] auto view() const -> std::string_view;

    /**
     * Direct read-only byte access.
     *
     * This is intentionally byte-based, not Unicode-character-based.
     */
    [[nodiscard]] auto byteAt(size_type index) const -> char;

    /**
     * Returns the Unicode code point at `index`.
     *
     * Returns U+FFFD if the index is invalid or the sequence is malformed.
     */
    [[nodiscard]] auto codePointAt(size_type index) const -> char32_t;

    /**
     * Returns the byte offset corresponding to a code-point index.
     */
    [[nodiscard]] auto byteOffset(
        size_type code_point_index) const -> size_type;


    //--------------------------------------------------------------------------
    //  Modification.
    //--------------------------------------------------------------------------

    void clear();

    void assign(std::string_view utf8);

    void assign(const char* utf8);

    void append(std::string_view utf8);

    void append(const String& other);

    void append(const char* utf8);

    auto operator += (std::string_view utf8) -> String&;

    auto operator += (const String& other) -> String&;

    auto operator += (const char* utf8) -> String&;

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
    //  Substrings.
    //--------------------------------------------------------------------------

    /**
     * Creates a substring using UTF-8 code-point indices.
     */
    [[nodiscard]] auto substr(
        size_type code_point_index,
        size_type code_point_count = npos) const -> String;


    //--------------------------------------------------------------------------
    //  Searching.
    //--------------------------------------------------------------------------

    /**
     * Finds `needle` and returns its UTF-8 code-point index.
     *
     * Returns `npos` if it isn't found.
     */
    [[nodiscard]] auto find(
        std::string_view needle,
        size_type code_point_index = 0
    ) const -> size_type;

    [[nodiscard]] auto contains(std::string_view needle) const -> bool;

    [[nodiscard]] auto startsWith(std::string_view prefix) const -> bool;

    [[nodiscard]] auto endsWith(std::string_view suffix) const -> bool;


    //--------------------------------------------------------------------------
    //  Whitespace.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto leadingWhitespace() const -> size_type;

    [[nodiscard]] auto trailingWhitespace() const -> size_type;

    void trim();

    void trimLeft();

    void trimRight();


    //--------------------------------------------------------------------------
    //  Comparison.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto compare(const String& other) const -> int;

    [[nodiscard]] auto compare(std::string_view other) const -> int;

    [[nodiscard]] auto compare(const char* other) const -> int;

    [[nodiscard]] auto equalsIgnoreCase(std::string_view other) const -> bool;


    //--------------------------------------------------------------------------
    //  Operators.
    //--------------------------------------------------------------------------

    friend auto operator == (const String& lhs, const String& rhs) -> bool;
    friend auto operator == (const String& lhs, const char* rhs) -> bool;
    friend auto operator == (const char* lhs, const String& rhs) -> bool;
    friend auto operator != (const String& lhs, const String& rhs) -> bool;
    friend auto operator != (const String& lhs, const char* rhs) -> bool;
    friend auto operator != (const char* lhs, const String& rhs) -> bool;
    friend auto operator == (const String& lhs, std::string_view rhs) -> bool;
    friend auto operator == (std::string_view lhs, const String& rhs) -> bool;
    friend auto operator != (const String& lhs, std::string_view rhs) -> bool;
    friend auto operator != (std::string_view lhs, const String& rhs) -> bool;
    friend auto operator + (const String& lhs, const String& rhs) -> String;
    friend auto operator + (const String& lhs, std::string_view rhs) -> String;
    friend auto operator + (std::string_view lhs, const String& rhs) -> String;
    friend auto operator + (const String& lhs, const char* rhs) -> String;
    friend auto operator + (const char* lhs, const String& rhs) -> String;
    friend auto operator << (std::ostream& stream, const String& string) -> std::ostream&;

private:
    std::string data_;

    [[nodiscard]] auto codePointIndexFromByteIndex(size_type byte_index) const -> size_type;

    [[nodiscard]] static auto isAsciiWhitespace(unsigned char byte) -> bool;

    void trimRange(size_type begin,size_type end);
};

} // namespace Grain