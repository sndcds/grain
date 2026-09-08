#include <catch2/catch_test_macros.hpp>
#include <grain/String/String.hpp>

#include <sstream>
#include <string>

using Grain::String;

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

TEST_CASE("String can be constructed", "[String]")
{
    String string("Hello");

    REQUIRE_FALSE(string.empty());
    REQUIRE(string.length() == 5);
    REQUIRE(string.byteLength() == 5);
}

TEST_CASE("String can be default constructed", "[String]")
{
    String string;

    REQUIRE(string.empty());
    REQUIRE(string.length() == 0);
    REQUIRE(string.byteLength() == 0);
}

TEST_CASE("String can be constructed from std::string_view", "[String]")
{
    String string(std::string_view{"Hello"});

    REQUIRE(string == "Hello");
}

TEST_CASE("String can be constructed from std::string", "[String]")
{
    const std::string value = "Hello";
    String string(value);

    REQUIRE(string == "Hello");
}

TEST_CASE("String can be copied", "[String]")
{
    String original("Hello");
    String copy(original);

    REQUIRE(copy == original);
}

TEST_CASE("String can be moved", "[String]")
{
    String original("Hello");
    String moved(std::move(original));

    REQUIRE(moved == "Hello");
}

TEST_CASE("String can be assigned from another String", "[String]")
{
    String string("Hello");
    String other("World");

    string = other;

    REQUIRE(string == "World");
}

TEST_CASE("String can be assigned from const char pointer", "[String]")
{
    String string;

    string = "Hello";

    REQUIRE(string == "Hello");
}

TEST_CASE("String can be assigned from std::string_view", "[String]")
{
    String string;

    string = std::string_view{"Hello"};

    REQUIRE(string == "Hello");
}

// -----------------------------------------------------------------------------
// Basic properties
// -----------------------------------------------------------------------------

TEST_CASE("String reports empty correctly", "[String]")
{
    String string;

    REQUIRE(string.empty());

    string = "Hello";

    REQUIRE_FALSE(string.empty());
}

TEST_CASE("String isEmpty is an alias for empty", "[String]")
{
    String empty;
    String nonEmpty("Hello");

    REQUIRE(empty.isEmpty());
    REQUIRE_FALSE(nonEmpty.isEmpty());
}

TEST_CASE("String length counts UTF-8 code points", "[String]")
{
    String ascii("Hello");
    String utf8("Äbc");
    String mixed("Hällö Welt");

    REQUIRE(ascii.length() == 5);
    REQUIRE(utf8.length() == 3);
    REQUIRE(mixed.length() == 10);
}

TEST_CASE("String byteLength counts UTF-8 bytes", "[String]")
{
    String ascii("Hello");
    String utf8("Äbc");
    String mixed("Hällö");

    REQUIRE(ascii.byteLength() == 5);
    REQUIRE(utf8.byteLength() == 4);
    REQUIRE(mixed.byteLength() == 7);
}

TEST_CASE("String length and byteLength differ for multibyte UTF-8", "[String]")
{
    String string("ÄÖÜ");

    REQUIRE(string.length() == 3);
    REQUIRE(string.byteLength() == 6);
}

// -----------------------------------------------------------------------------
// ASCII and UTF-8
// -----------------------------------------------------------------------------

TEST_CASE("String identifies ASCII correctly", "[String]")
{
    String ascii("Hello, World!");

    REQUIRE(ascii.isAscii());
}

TEST_CASE("String identifies non-ASCII UTF-8 correctly", "[String]")
{
    String utf8("Hällo");

    REQUIRE_FALSE(utf8.isAscii());
}

TEST_CASE("String validates ASCII UTF-8", "[String]")
{
    String string("Hello, World!");

    REQUIRE(string.isValidUtf8());
}

TEST_CASE("String validates UTF-8", "[String]")
{
    String string("Hällö 世界");

    REQUIRE(string.isValidUtf8());
}

TEST_CASE("String detects invalid UTF-8", "[String]")
{
    const std::string invalid("\xC3\x28", 2);
    String string(invalid);

    REQUIRE_FALSE(string.isValidUtf8());
}

TEST_CASE("String reports invalid UTF-8 byte index", "[String]")
{
    const std::string invalid("Hello\xC3\x28", 7);
    String string(invalid);

    String::size_type invalidIndex = String::npos;

    REQUIRE_FALSE(string.isValidUtf8(&invalidIndex));
    REQUIRE(invalidIndex == 5);
}

// -----------------------------------------------------------------------------
// Access
// -----------------------------------------------------------------------------

TEST_CASE("String returns a null-terminated UTF-8 string", "[String]")
{
    String string("Hello");

    REQUIRE(string.c_str() != nullptr);
    REQUIRE(std::string(string.c_str()) == "Hello");
}

TEST_CASE("String utf8 is an alias for c_str", "[String]")
{
    String string("Hello");

    REQUIRE(std::string(string.utf8()) == "Hello");
}

TEST_CASE("String provides a string_view", "[String]")
{
    String string("Hello");

    const std::string_view view = string.view();

    REQUIRE(view == "Hello");
    REQUIRE(view.size() == 5);
}

TEST_CASE("String provides byte access", "[String]")
{
    String string("Hello");

    REQUIRE(string.byteAt(0) == 'H');
    REQUIRE(string.byteAt(1) == 'e');
    REQUIRE(string.byteAt(4) == 'o');
}

TEST_CASE("String byte access is byte-based", "[String]")
{
    String string("Äbc");

    REQUIRE(string.length() == 3);
    REQUIRE(string.byteLength() == 4);

    REQUIRE(
        static_cast<unsigned char>(string.byteAt(0)) == 0xC3
    );

    REQUIRE(
        static_cast<unsigned char>(string.byteAt(1)) == 0x84
    );

    REQUIRE(string.byteAt(2) == 'b');
    REQUIRE(string.byteAt(3) == 'c');
}

TEST_CASE("String returns replacement character for invalid code point index",
          "[String]")
{
    String string("Hello");

    REQUIRE(string.codePointAt(100) == 0xFFFD);
}

TEST_CASE("String returns UTF-8 code points", "[String]")
{
    String string("AÄ€😀");

    REQUIRE(string.length() == 4);

    REQUIRE(string.codePointAt(0) == U'A');
    REQUIRE(string.codePointAt(1) == U'Ä');
    REQUIRE(string.codePointAt(2) == U'€');
    REQUIRE(string.codePointAt(3) == U'😀');
}

TEST_CASE("String byteOffset returns UTF-8 byte positions", "[String]")
{
    String string("Äbc");

    REQUIRE(string.byteOffset(0) == 0);
    REQUIRE(string.byteOffset(1) == 2);
    REQUIRE(string.byteOffset(2) == 3);
}

TEST_CASE("String byteOffset returns byteLength at end", "[String]")
{
    String string("Äbc");

    REQUIRE(string.byteOffset(3) == string.byteLength());
}

TEST_CASE("String byteOffset returns npos for invalid index", "[String]")
{
    String string("Hello");

    REQUIRE(string.byteOffset(6) == String::npos);
}

// -----------------------------------------------------------------------------
// Modification
// -----------------------------------------------------------------------------

TEST_CASE("String can be cleared", "[String]")
{
    String string("Hello");

    string.clear();

    REQUIRE(string.empty());
    REQUIRE(string.length() == 0);
    REQUIRE(string.byteLength() == 0);
}

TEST_CASE("String can be assigned", "[String]")
{
    String string("Hello");

    string.assign("World");

    REQUIRE(string == "World");
}

TEST_CASE("String can append UTF-8 text", "[String]")
{
    String string("Hello");

    string.append(" World");

    REQUIRE(string == "Hello World");
}

TEST_CASE("String can append another String", "[String]")
{
    String string("Hello");
    String other(" World");

    string.append(other);

    REQUIRE(string == "Hello World");
}

TEST_CASE("String supports operator plus equals", "[String]")
{
    String string("Hello");

    string += " ";
    string += String("World");

    REQUIRE(string == "Hello World");
}

TEST_CASE("String can insert ASCII text", "[String]")
{
    String string("Hllo");

    string.insert(1, "e");

    REQUIRE(string == "Hello");
}

TEST_CASE("String can insert UTF-8 text", "[String]")
{
    String string("Hllo");

    string.insert(1, "ä");

    REQUIRE(string == "Hällo");
}

TEST_CASE("String can erase code points", "[String]")
{
    String string("Hello");

    string.erase(1, 2);

    REQUIRE(string == "Hlo");
}

TEST_CASE("String can erase UTF-8 code points", "[String]")
{
    String string("Hällo");

    string.erase(1, 1);

    REQUIRE(string == "Hllo");
}

TEST_CASE("String can replace code points", "[String]")
{
    String string("Hello");

    string.replace(1, 3, "i");

    REQUIRE(string == "Hio");
}

TEST_CASE("String can replace UTF-8 text", "[String]")
{
    String string("Hällo");

    string.replace(1, 1, "e");

    REQUIRE(string == "Hello");
}

// -----------------------------------------------------------------------------
// Substrings
// -----------------------------------------------------------------------------

TEST_CASE("String can create substrings", "[String]")
{
    String string("Hello World");

    REQUIRE(string.substr(0, 5) == "Hello");
    REQUIRE(string.substr(6, 5) == "World");
}

TEST_CASE("String substr uses code-point indices", "[String]")
{
    String string("Äbcdef");

    REQUIRE(string.substr(0, 1) == "Ä");
    REQUIRE(string.substr(1, 3) == "bcd");
    REQUIRE(string.substr(2, 2) == "cd");
}

TEST_CASE("String substr without count returns remainder", "[String]")
{
    String string("Hello World");

    REQUIRE(string.substr(6) == "World");
}

// -----------------------------------------------------------------------------
// Searching
// -----------------------------------------------------------------------------

TEST_CASE("String can find ASCII text", "[String]")
{
    String string("Hello World");

    REQUIRE(string.find("Hello") == 0);
    REQUIRE(string.find("World") == 6);
    REQUIRE(string.find("lo") == 3);
}

TEST_CASE("String find returns npos when not found", "[String]")
{
    String string("Hello World");

    REQUIRE(string.find("xyz") == String::npos);
}

TEST_CASE("String find supports a starting code-point index", "[String]")
{
    String string("one two one");

    REQUIRE(string.find("one") == 0);
    REQUIRE(string.find("one", 1) == 8);
    REQUIRE(string.find("one", 9) == String::npos);
}

TEST_CASE("String find returns code-point index for UTF-8", "[String]")
{
    String string("Äbc Äde");

    REQUIRE(string.find("Ä") == 0);
    REQUIRE(string.find("bc") == 1);
    REQUIRE(string.find("Äde") == 4);
}

TEST_CASE("String can check whether it contains text", "[String]")
{
    String string("Hello World");

    REQUIRE(string.contains("Hello"));
    REQUIRE(string.contains("World"));
    REQUIRE(string.contains("lo Wo"));
    REQUIRE_FALSE(string.contains("xyz"));
}

TEST_CASE("String can check prefixes", "[String]")
{
    String string("Hello World");

    REQUIRE(string.startsWith("Hello"));
    REQUIRE(string.startsWith(""));
    REQUIRE_FALSE(string.startsWith("World"));
}

TEST_CASE("String can check suffixes", "[String]")
{
    String string("Hello World");

    REQUIRE(string.endsWith("World"));
    REQUIRE(string.endsWith(""));
    REQUIRE_FALSE(string.endsWith("Hello"));
}

// -----------------------------------------------------------------------------
// Whitespace
// -----------------------------------------------------------------------------

TEST_CASE("String counts leading ASCII whitespace", "[String]")
{
    String string("  Hello");

    REQUIRE(string.leadingWhitespace() == 2);
}

TEST_CASE("String counts trailing ASCII whitespace", "[String]")
{
    String string("Hello  ");

    REQUIRE(string.trailingWhitespace() == 2);
}

TEST_CASE("String trims both sides", "[String]")
{
    String string("  Hello World  ");

    string.trim();

    REQUIRE(string == "Hello World");
}

TEST_CASE("String trims left whitespace", "[String]")
{
    String string("  Hello World  ");

    string.trimLeft();

    REQUIRE(string == "Hello World  ");
}

TEST_CASE("String trims right whitespace", "[String]")
{
    String string("  Hello World  ");

    string.trimRight();

    REQUIRE(string == "  Hello World");
}

TEST_CASE("String handles an all-whitespace string", "[String]")
{
    String string("   ");

    string.trim();

    REQUIRE(string.empty());
}

// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------

TEST_CASE("String compares equal strings", "[String]")
{
    String lhs("Hello");
    String rhs("Hello");

    REQUIRE(lhs.compare(rhs) == 0);
    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
}

TEST_CASE("String compares different strings", "[String]")
{
    String lhs("Hello");
    String rhs("World");

    REQUIRE(lhs.compare(rhs) < 0);
    REQUIRE(rhs.compare(lhs) > 0);
}

TEST_CASE("String can compare with std::string_view", "[String]")
{
    String string("Hello");

    REQUIRE(string.compare("Hello") == 0);
    REQUIRE(string == "Hello");
    REQUIRE("Hello" == string);

    REQUIRE(string != "World");
    REQUIRE("World" != string);
}

TEST_CASE("String supports case-insensitive comparison", "[String]")
{
    String string("Hello World");

    REQUIRE(string.equalsIgnoreCase("hello world"));
    REQUIRE(string.equalsIgnoreCase("HELLO WORLD"));
    REQUIRE_FALSE(string.equalsIgnoreCase("Hello World!"));
}

// -----------------------------------------------------------------------------
// Operators
// -----------------------------------------------------------------------------

TEST_CASE("String supports operator plus", "[String]")
{
    String hello("Hello");
    String world("World");

    REQUIRE(hello + world == "HelloWorld");
    REQUIRE(hello + " World" == "Hello World");
    REQUIRE("Hello " + world == "Hello World");
}

TEST_CASE("String supports stream output", "[String]")
{
    String string("Hello World");
    std::ostringstream stream;

    stream << string;

    REQUIRE(stream.str() == "Hello World");
}