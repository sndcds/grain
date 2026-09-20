#include <catch2/catch_test_macros.hpp>

#include <grain/Math/Quantization.hpp>

#include <cstdint>
#include <limits>

namespace Grain::Quantization::Tests {

TEST_CASE("Quantization::quantize", "[Math][Quantization]") {
    SECTION("target bit depth equal to source bit depth returns value unchanged") {
        REQUIRE(quantize(0u, 8, 8) == 0u);
        REQUIRE(quantize(1u, 8, 8) == 1u);
        REQUIRE(quantize(127u, 8, 8) == 127u);
        REQUIRE(quantize(255u, 8, 8) == 255u);
    }

    SECTION("target bit depth greater than source bit depth returns value unchanged") {
        REQUIRE(quantize(0u, 8, 16) == 0u);
        REQUIRE(quantize(1u, 8, 16) == 1u);
        REQUIRE(quantize(127u, 8, 16) == 127u);
        REQUIRE(quantize(255u, 8, 16) == 255u);
    }

    SECTION("targetBits zero returns zero") {
        REQUIRE(quantize(0u, 8, 0) == 0u);
        REQUIRE(quantize(1u, 8, 0) == 0u);
        REQUIRE(quantize(127u, 8, 0) == 0u);
        REQUIRE(quantize(255u, 8, 0) == 0u);
    }

    SECTION("sourceBits zero returns zero") {
        REQUIRE(quantize(0u, 0, 0) == 0u);
        REQUIRE(quantize(1u, 0, 0) == 1u);
        REQUIRE(quantize(255u, 0, 0) == 255u);
    }

    SECTION("8 bit to 4 bit conversion") {
        REQUIRE(quantize(0u, 8, 4) == 0u);
        REQUIRE(quantize(255u, 8, 4) == 15u);

        REQUIRE(quantize(128u, 8, 4) == 8u);
        REQUIRE(quantize(127u, 8, 4) == 7u);

        REQUIRE(quantize(16u, 8, 4) == 1u);
        REQUIRE(quantize(17u, 8, 4) == 1u);

        REQUIRE(quantize(32u, 8, 4) == 2u);
        REQUIRE(quantize(64u, 8, 4) == 4u);
        REQUIRE(quantize(192u, 8, 4) == 11u);
    }

    SECTION("8 bit to 1 bit conversion") {
        REQUIRE(quantize(0u, 8, 1) == 0u);
        REQUIRE(quantize(1u, 8, 1) == 0u);
        REQUIRE(quantize(127u, 8, 1) == 0u);
        REQUIRE(quantize(128u, 8, 1) == 1u);
        REQUIRE(quantize(129u, 8, 1) == 1u);
        REQUIRE(quantize(255u, 8, 1) == 1u);
    }

    SECTION("16 bit to 8 bit conversion") {
        REQUIRE(quantize(0u, 16, 8) == 0u);
        REQUIRE(quantize(65535u, 16, 8) == 255u);

        REQUIRE(quantize(32768u, 16, 8) == 128u);
        REQUIRE(quantize(32767u, 16, 8) == 127u);

        REQUIRE(quantize(257u, 16, 8) == 1u);
        REQUIRE(quantize(258u, 16, 8) == 1u);
    }

    SECTION("32 bit source uses 64 bit arithmetic") {
        constexpr std::uint32_t max =
            std::numeric_limits<std::uint32_t>::max();

        REQUIRE(
            quantize(max, 32, 16)
            == 65535u
        );

        REQUIRE(
            quantize(0u, 32, 16)
            == 0u
        );

        REQUIRE(
            quantize(
                0x80000000u,
                32,
                16
            )
            == 32768u
        );
    }

    SECTION("32 bit source to 8 bit target") {
        constexpr std::uint32_t max =
            std::numeric_limits<std::uint32_t>::max();

        REQUIRE(quantize(0u, 32, 8) == 0u);
        REQUIRE(quantize(max, 32, 8) == 255u);

        REQUIRE(
            quantize(
                0x80000000u,
                32,
                8
            )
            == 128u
        );
    }

    SECTION("full source range maps to full target range") {
        REQUIRE(quantize(0u, 10, 5) == 0u);
        REQUIRE(quantize(1023u, 10, 5) == 31u);

        REQUIRE(quantize(0u, 16, 4) == 0u);
        REQUIRE(quantize(65535u, 16, 4) == 15u);

        REQUIRE(quantize(0u, 24, 8) == 0u);
        REQUIRE(quantize(0xFFFFFFu, 24, 8) == 255u);
    }

    SECTION("zero maps to zero for every reduction") {
        REQUIRE(quantize(0u, 2, 1) == 0u);
        REQUIRE(quantize(0u, 4, 2) == 0u);
        REQUIRE(quantize(0u, 8, 4) == 0u);
        REQUIRE(quantize(0u, 16, 8) == 0u);
        REQUIRE(quantize(0u, 32, 16) == 0u);
    }

    SECTION("maximum value maps to maximum target value") {
        REQUIRE(quantize(3u, 2, 1) == 1u);
        REQUIRE(quantize(15u, 4, 2) == 3u);
        REQUIRE(quantize(255u, 8, 4) == 15u);
        REQUIRE(quantize(65535u, 16, 8) == 255u);
        REQUIRE(quantize(0xFFFFFFFFu, 32, 16) == 65535u);
    }

    SECTION("conversion is monotonic") {
        constexpr unsigned sourceBits = 8;
        constexpr unsigned targetBits = 4;

        std::uint32_t previous = 0;

        for (std::uint32_t value = 0; value <= 255; ++value) {
            const std::uint32_t result =
                quantize(value, sourceBits, targetBits);

            REQUIRE(result >= previous);
            previous = result;
        }
    }

    SECTION("all 8 bit values stay within target range") {
        for (std::uint32_t value = 0; value <= 255; ++value) {
            const std::uint32_t result =
                quantize(value, 8, 4);

            REQUIRE(result <= 15u);
        }
    }

    SECTION("4 bit to 2 bit conversion") {
        REQUIRE(quantize(0u,  4, 2) == 0u);
        REQUIRE(quantize(2u,  4, 2) == 0u);
        REQUIRE(quantize(3u,  4, 2) == 1u);
        REQUIRE(quantize(7u,  4, 2) == 1u);
        REQUIRE(quantize(8u,  4, 2) == 2u);
        REQUIRE(quantize(12u, 4, 2) == 2u);
        REQUIRE(quantize(13u, 4, 2) == 3u);
        REQUIRE(quantize(15u, 4, 2) == 3u);
    }
}

} // namespace Grain::Quantization::Tests