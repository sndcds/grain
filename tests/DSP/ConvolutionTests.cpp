
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <grain/DSP/Convolution.hpp>

#include <array>
#include <cstddef>
#include <vector>

namespace Grain::DSP::Tests {

namespace {

auto directConvolution(
    const std::vector<float>& input,
    const std::vector<float>& impulseResponse
) -> std::vector<float> {

    std::vector<float> output(
        input.size() + impulseResponse.size() - 1,
        0.0f
    );

    for (std::size_t i = 0; i < input.size(); ++i) {
        for (std::size_t j = 0; j < impulseResponse.size(); ++j) {
            output[i + j] += input[i] * impulseResponse[j];
        }
    }

    return output;
}

auto verifyConvolution(
    const std::vector<float>& input,
    const std::vector<float>& impulseResponse,
    std::size_t partitionSize
) -> void {

    ConvolutionConfiguration configuration;
    configuration.partitionSize = partitionSize;
    configuration.backend = FFTBackendType::Portable;

    std::vector<float> actual;

    REQUIRE(
        Convolution::process(
            input,
            impulseResponse,
            actual,
            configuration
        ) == Result::Success
    );

    const std::vector<float> expected =
        directConvolution(input, impulseResponse);

    REQUIRE(actual.size() == expected.size());

    for (std::size_t i = 0; i < expected.size(); ++i) {
        INFO("Sample index: " << i);
        REQUIRE(actual[i] == Catch::Approx(expected[i]).margin(1.0e-4f));
    }
}

} // namespace


TEST_CASE("Convolution basic functionality", "[DSP][Convolution]")
{
    SECTION("convolves two short signals")
    {
        const std::vector<float> input = {1.0f, 2.0f, 3.0f};
        const std::vector<float> impulseResponse = {1.0f, 1.0f, 1.0f};

        verifyConvolution(input, impulseResponse, 2);
    }

    SECTION("supports an impulse response with one sample")
    {
        const std::vector<float> input = {1.0f, 2.0f, 3.0f};
        const std::vector<float> impulseResponse = {2.0f};

        verifyConvolution(input, impulseResponse, 2);
    }

    SECTION("supports a single-sample input")
    {
        const std::vector<float> input = {2.0f};
        const std::vector<float> impulseResponse = {1.0f, 2.0f, 3.0f};

        verifyConvolution(input, impulseResponse, 2);
    }

    SECTION("supports negative samples")
    {
        const std::vector<float> input = {1.0f, -2.0f, 3.0f};
        const std::vector<float> impulseResponse = {-1.0f, 0.5f};

        verifyConvolution(input, impulseResponse, 2);
    }
}


TEST_CASE("Convolution partition sizes", "[DSP][Convolution]")
{
    const std::vector<float> input = {
        0.25f, -0.5f, 0.75f, 1.0f,
        -0.25f, 0.5f, -0.75f, 0.125f,
        0.625f, -0.375f, 0.875f
    };

    const std::vector<float> impulseResponse = {
        0.5f, 0.25f, -0.125f, 0.75f,
        -0.5f, 0.125f, 0.25f
    };

    constexpr std::array<std::size_t, 5> partitionSizes = {
        1, 2, 3, 4, 8
    };

    for (const std::size_t partitionSize : partitionSizes) {
        DYNAMIC_SECTION("Partition size: " << partitionSize) {
            verifyConvolution(input, impulseResponse, partitionSize);
        }
    }
}


TEST_CASE("Convolution validates arguments", "[DSP][Convolution]")
{
    const std::vector<float> input = {1.0f, 2.0f};
    const std::vector<float> impulseResponse = {1.0f, 1.0f};

    ConvolutionConfiguration configuration;
    configuration.partitionSize = 2;
    configuration.backend = FFTBackendType::Portable;

    std::vector<float> output = {99.0f};

    SECTION("rejects empty input")
    {
        REQUIRE(
            Convolution::process(
                {},
                impulseResponse,
                output,
                configuration
            ) == Result::InvalidArgument
        );

        REQUIRE(output.empty());
    }

    SECTION("rejects empty impulse response")
    {
        REQUIRE(
            Convolution::process(
                input,
                {},
                output,
                configuration
            ) == Result::InvalidArgument
        );

        REQUIRE(output.empty());
    }

    SECTION("rejects a zero partition size")
    {
        configuration.partitionSize = 0;

        REQUIRE(
            Convolution::process(
                input,
                impulseResponse,
                output,
                configuration
            ) == Result::InvalidArgument
        );

        REQUIRE(output.empty());
    }
}

} // namespace Grain::DSP::Tests
