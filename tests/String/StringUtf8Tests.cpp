#include <catch2/catch_test_macros.hpp>

#include <grain/String/StringUtf8.hpp>

using Grain::StringUtf8;

TEST_CASE("StringUtf8 ASCII detection", "[StringUtf8]") {
    CHECK(StringUtf8::isAscii(""));
    CHECK(StringUtf8::isAscii("hello"));
    CHECK(StringUtf8::isAscii("Hello 123"));

    CHECK_FALSE(StringUtf8::isAscii("Ä"));
    CHECK_FALSE(StringUtf8::isAscii("€"));
    CHECK_FALSE(StringUtf8::isAscii("😀"));
}

TEST_CASE("StringUtf8 validation", "[StringUtf8]") {
    CHECK(StringUtf8::isValid(""));
    CHECK(StringUtf8::isValid("hello"));
    CHECK(StringUtf8::isValid("Ä"));
    CHECK(StringUtf8::isValid("€"));
    CHECK(StringUtf8::isValid("😀"));
    CHECK(StringUtf8::isValid("Ä€😀"));
}

TEST_CASE("StringUtf8 reports invalid byte", "[StringUtf8]") {
    constexpr std::string_view value{"\xc3\x28"};

    StringUtf8::size_type invalid_byte_index =
        StringUtf8::npos;

    CHECK_FALSE(StringUtf8::isValid(
        value,
        &invalid_byte_index));

    CHECK(invalid_byte_index == 0);
}

TEST_CASE("StringUtf8 length", "[StringUtf8]") {
    CHECK(StringUtf8::length("") == 0);
    CHECK(StringUtf8::length("abc") == 3);

    CHECK(StringUtf8::length("Ä") == 1);
    CHECK(StringUtf8::length("€") == 1);
    CHECK(StringUtf8::length("😀") == 1);

    CHECK(StringUtf8::length("Äbc") == 3);
    CHECK(StringUtf8::length("Ä€😀") == 3);
}

TEST_CASE("StringUtf8 decode ASCII", "[StringUtf8]") {
    char32_t code_point{};
    StringUtf8::size_type sequence_length{};

    REQUIRE(StringUtf8::decode(
        "A",
        0,
        code_point,
        sequence_length));

    CHECK(code_point == U'A');
    CHECK(sequence_length == 1);
}

TEST_CASE("StringUtf8 decode multi-byte sequences", "[StringUtf8]") {
    char32_t code_point{};
    StringUtf8::size_type sequence_length{};

    SECTION("two bytes") {
        REQUIRE(StringUtf8::decode(
            "Ä",
            0,
            code_point,
            sequence_length));

        CHECK(code_point == U'Ä');
        CHECK(sequence_length == 2);
    }

    SECTION("three bytes") {
        REQUIRE(StringUtf8::decode(
            "€",
            0,
            code_point,
            sequence_length));

        CHECK(code_point == U'€');
        CHECK(sequence_length == 3);
    }

    SECTION("four bytes") {
        REQUIRE(StringUtf8::decode(
            "😀",
            0,
            code_point,
            sequence_length));

        CHECK(code_point == U'😀');
        CHECK(sequence_length == 4);
    }
}

TEST_CASE("StringUtf8 codePointAt", "[StringUtf8]") {
    constexpr std::string_view value = "Ä€😀";

    CHECK(StringUtf8::codePointAt(value, 0) == U'Ä');
    CHECK(StringUtf8::codePointAt(value, 1) == U'€');
    CHECK(StringUtf8::codePointAt(value, 2) == U'😀');

    CHECK(
        StringUtf8::codePointAt(value, 3) ==
        StringUtf8::replacementCharacter);
}

TEST_CASE("StringUtf8 byteOffset", "[StringUtf8]") {
    constexpr std::string_view value = "Ä€😀";

    CHECK(StringUtf8::byteOffset(value, 0) == 0);
    CHECK(StringUtf8::byteOffset(value, 1) == 2);
    CHECK(StringUtf8::byteOffset(value, 2) == 5);
    CHECK(StringUtf8::byteOffset(value, 3) == 9);

    CHECK(
        StringUtf8::byteOffset(value, 4) ==
        StringUtf8::npos);
}

TEST_CASE("StringUtf8 codePointIndex", "[StringUtf8]") {
    constexpr std::string_view value = "Ä€😀";

    SECTION("valid UTF-8 boundaries") {
        CHECK(StringUtf8::codePointIndex(value, 0) == 0);
        CHECK(StringUtf8::codePointIndex(value, 2) == 1);
        CHECK(StringUtf8::codePointIndex(value, 5) == 2);
        CHECK(StringUtf8::codePointIndex(value, 9) == 3);
    }

    SECTION("inside UTF-8 sequence") {
        CHECK(
            StringUtf8::codePointIndex(value, 1) ==
            StringUtf8::npos);

        CHECK(
            StringUtf8::codePointIndex(value, 3) ==
            StringUtf8::npos);

        CHECK(
            StringUtf8::codePointIndex(value, 4) ==
            StringUtf8::npos);

        CHECK(
            StringUtf8::codePointIndex(value, 6) ==
            StringUtf8::npos);

        CHECK(
            StringUtf8::codePointIndex(value, 7) ==
            StringUtf8::npos);

        CHECK(
            StringUtf8::codePointIndex(value, 8) ==
            StringUtf8::npos);
    }

    SECTION("outside string") {
        CHECK(
            StringUtf8::codePointIndex(value, 10) ==
            StringUtf8::npos);
    }
}

TEST_CASE("StringUtf8 sequenceLength", "[StringUtf8]") {
    CHECK(StringUtf8::sequenceLength(0x00) == 1);
    CHECK(StringUtf8::sequenceLength(0x7f) == 1);

    CHECK(StringUtf8::sequenceLength(0xc2) == 2);
    CHECK(StringUtf8::sequenceLength(0xdf) == 2);

    CHECK(StringUtf8::sequenceLength(0xe0) == 3);
    CHECK(StringUtf8::sequenceLength(0xef) == 3);

    CHECK(StringUtf8::sequenceLength(0xf0) == 4);
    CHECK(StringUtf8::sequenceLength(0xf4) == 4);

    CHECK(StringUtf8::sequenceLength(0x80) == 0);
    CHECK(StringUtf8::sequenceLength(0xff) == 0);
}

TEST_CASE("StringUtf8 continuation bytes", "[StringUtf8]") {
    CHECK(StringUtf8::isContinuationByte(0x80));
    CHECK(StringUtf8::isContinuationByte(0x8f));
    CHECK(StringUtf8::isContinuationByte(0xbf));

    CHECK_FALSE(StringUtf8::isContinuationByte(0x00));
    CHECK_FALSE(StringUtf8::isContinuationByte(0x7f));
    CHECK_FALSE(StringUtf8::isContinuationByte(0xc0));
}

TEST_CASE("StringUtf8 rejects invalid encodings", "[StringUtf8]") {
    constexpr std::string_view invalid_sequences[] = {
        "\xc0\x80",           // overlong NUL
        "\xe0\x80\x80",       // overlong
        "\xf0\x80\x80\x80",   // overlong
        "\xed\xa0\x80",       // UTF-16 surrogate
        "\xf4\x90\x80\x80",   // > U+10FFFF
        "\xc2",               // truncated
        "\xe2\x82",           // truncated
        "\xf0\x9f\x98",       // truncated
        "\xc2\x41",           // invalid continuation
        "\xe2\x28\xa1",       // invalid continuation
    };

    for (const auto value : invalid_sequences) {
        CHECK_FALSE(StringUtf8::isValid(value));
    }
}

TEST_CASE("StringUtf8 decode rejects invalid sequences", "[StringUtf8]") {
    char32_t code_point{};
    StringUtf8::size_type sequence_length{};

    CHECK_FALSE(StringUtf8::decode(
        "\xc0\x80",
        0,
        code_point,
        sequence_length));

    CHECK_FALSE(StringUtf8::decode(
        "\xe2\x28\xa1",
        0,
        code_point,
        sequence_length));

    CHECK_FALSE(StringUtf8::decode(
        "\xf0\x90\x80",
        0,
        code_point,
        sequence_length));
}