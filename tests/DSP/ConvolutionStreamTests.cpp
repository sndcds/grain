
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <grain/DSP/Convolution.hpp>
#include <grain/DSP/ConvolutionStream.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <random>
#include <vector>

namespace Grain::DSP::Tests {

namespace {

auto runStreamingConvolution(
    const std::vector<float>& input,
    const std::vector<float>& impulseResponse,
    std::size_t partitionSize
) -> std::vector<float> {

    ConvolutionStreamConfiguration configuration;
    configuration.partitionSize = partitionSize;
    configuration.backend = FFTBackendType::Portable;

    std::unique_ptr<ConvolutionStream> stream;

    REQUIRE(
        ConvolutionStream::create(configuration, stream) == Result::Success
    );

    REQUIRE(stream != nullptr);

    REQUIRE(
        stream->setImpulseResponse(impulseResponse) == Result::Success
    );

    constexpr std::array<std::size_t, 4> inputBlockSizes = {
        1, 13, 4, 29
    };

    std::array<float, 7> outputBuffer {};
    std::vector<float> result;

    std::size_t inputOffset = 0;
    std::size_t blockIndex = 0;

    // Feed the input in blocks of varying sizes.
    while (inputOffset < input.size()) {
        const std::size_t blockSize = std::min(
            inputBlockSizes[blockIndex % inputBlockSizes.size()],
            input.size() - inputOffset
        );

        const std::size_t blockEnd = inputOffset + blockSize;

        while (inputOffset < blockEnd) {
            std::size_t inputConsumed = 0;
            std::size_t outputProduced = 0;

            const Result processResult = stream->process(
                std::span<const float>(
                    input.data() + inputOffset,
                    blockEnd - inputOffset
                ),
                outputBuffer,
                inputConsumed,
                outputProduced
            );

            REQUIRE(processResult == Result::Success);

            inputOffset += inputConsumed;

            result.insert(
                result.end(),
                outputBuffer.begin(),
                outputBuffer.begin() +
                    static_cast<std::ptrdiff_t>(outputProduced)
            );

            REQUIRE((inputConsumed > 0 || outputProduced > 0));
        }

        ++blockIndex;
    }

    // Drain output that may still be pending after the final input block.
    while (true) {
        std::size_t inputConsumed = 0;
        std::size_t outputProduced = 0;

        REQUIRE(
            stream->process(
                {},
                outputBuffer,
                inputConsumed,
                outputProduced
            ) == Result::Success
        );

        REQUIRE(inputConsumed == 0);

        result.insert(
            result.end(),
            outputBuffer.begin(),
            outputBuffer.begin() +
                static_cast<std::ptrdiff_t>(outputProduced)
        );

        if (outputProduced == 0) {
            break;
        }
    }

    // Flush the remaining convolution tail.
    bool finished = false;

    while (!finished) {
        std::size_t outputProduced = 0;

        REQUIRE(
            stream->finish(
                outputBuffer,
                outputProduced,
                finished
            ) == Result::Success
        );

        result.insert(
            result.end(),
            outputBuffer.begin(),
            outputBuffer.begin() +
                static_cast<std::ptrdiff_t>(outputProduced)
        );
    }

    return result;
}


auto verifyStreamingConvolution(
    const std::vector<float>& input,
    const std::vector<float>& impulseResponse,
    std::size_t partitionSize
) -> void {

    ConvolutionConfiguration configuration;
    configuration.partitionSize = partitionSize;
    configuration.backend = FFTBackendType::Portable;

    std::vector<float> expected;

    REQUIRE(
        Convolution::process(
            input,
            impulseResponse,
            expected,
            configuration
        ) == Result::Success
    );

    const std::vector<float> actual = runStreamingConvolution(
        input,
        impulseResponse,
        partitionSize
    );

    REQUIRE(actual.size() == expected.size());

    for (std::size_t i = 0; i < expected.size(); ++i) {
        INFO("Sample index: " << i);

        REQUIRE(
            actual[i] == Catch::Approx(expected[i]).margin(1.0e-4f)
        );
    }
}

} // namespace


TEST_CASE("ConvolutionStream basic functionality", "[DSP][ConvolutionStream]")
{
    SECTION("convolves short signals")
    {
        const std::vector<float> input = {
            1.0f, 2.0f, 3.0f, 4.0f
        };

        const std::vector<float> impulseResponse = {
            1.0f, 0.5f, 0.25f
        };

        verifyStreamingConvolution(input, impulseResponse, 2);
    }

    SECTION("supports a single-sample impulse response")
    {
        const std::vector<float> input = {
            0.25f, -0.5f, 0.75f, 1.0f, -0.25f
        };

        const std::vector<float> impulseResponse = {
            2.0f
        };

        verifyStreamingConvolution(input, impulseResponse, 4);
    }

    SECTION("supports an impulse response longer than the input")
    {
        const std::vector<float> input = {
            1.0f, 0.0f, -0.5f
        };

        const std::vector<float> impulseResponse = {
            0.5f, 0.25f, 0.125f, 0.0625f,
            0.03125f, 0.015625f, 0.0078125f
        };

        verifyStreamingConvolution(input, impulseResponse, 2);
    }
}


TEST_CASE("ConvolutionStream supports varying partition sizes",
          "[DSP][ConvolutionStream]")
{
    std::mt19937 generator(12345);
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    std::vector<float> input(2048);
    std::vector<float> impulseResponse(513);

    for (float& sample : input) {
        sample = distribution(generator);
    }

    for (float& sample : impulseResponse) {
        sample = distribution(generator);
    }

    constexpr std::array<std::size_t, 4> partitionSizes = {
        1, 2, 7, 128
    };

    for (const std::size_t partitionSize : partitionSizes) {
        DYNAMIC_SECTION("Partition size: " << partitionSize) {
            verifyStreamingConvolution(
                input,
                impulseResponse,
                partitionSize
            );
        }
    }
}


TEST_CASE("ConvolutionStream preserves delayed impulse alignment",
          "[DSP][ConvolutionStream]")
{
    const std::vector<float> input = {
        1.0f, 0.0f, 0.0f, 0.0f
    };

    const std::vector<float> impulseResponse = {
        0.0f, 0.0f, 0.5f, 0.0f, 0.25f
    };

    verifyStreamingConvolution(input, impulseResponse, 2);
}


TEST_CASE("ConvolutionStream handles an empty input",
          "[DSP][ConvolutionStream]")
{
    const std::vector<float> impulseResponse = {
        1.0f, 0.5f, 0.25f
    };

    ConvolutionStreamConfiguration configuration;
    configuration.partitionSize = 4;
    configuration.backend = FFTBackendType::Portable;

    std::unique_ptr<ConvolutionStream> stream;

    REQUIRE(
        ConvolutionStream::create(configuration, stream) == Result::Success
    );

    REQUIRE(
        stream->setImpulseResponse(impulseResponse) == Result::Success
    );

    std::array<float, 8> output {};
    std::size_t outputProduced = 0;
    bool finished = false;

    REQUIRE(
        stream->finish(output, outputProduced, finished) == Result::Success
    );

    REQUIRE(outputProduced == 0);
    REQUIRE(finished);
}


TEST_CASE("ConvolutionStream reset clears processing history", "[DSP][ConvolutionStream]") {
    const std::vector<float> input = {
        1.0F, 0.5F, -0.25F, 0.75F, 0.2F, -0.4F, 0.8F, 0.1F
    };
    const std::vector<float> impulseResponse = {
        0.5F, 0.25F, -0.1F
    };

    ConvolutionStreamConfiguration configuration;
    configuration.partitionSize = 2;
    configuration.backend = FFTBackendType::Portable;

    std::unique_ptr<ConvolutionStream> stream;
    REQUIRE(ConvolutionStream::create(configuration, stream) == Result::Success);
    REQUIRE(stream != nullptr);
    REQUIRE(stream->setImpulseResponse(impulseResponse) == Result::Success);

    const auto processInput = [&]() {
        std::vector<float> result;
        std::vector<float> outputBuffer(64);

        std::size_t inputConsumed = 0;
        std::size_t outputProduced = 0;

        REQUIRE(stream->process(
            input,
            outputBuffer,
            inputConsumed,
            outputProduced
        ) == Result::Success);

        REQUIRE(inputConsumed == input.size());

        result.insert(
            result.end(),
            outputBuffer.begin(),
            outputBuffer.begin() + static_cast<std::ptrdiff_t>(outputProduced)
        );

        bool finished = false;
        while (!finished) {
            REQUIRE(stream->finish(outputBuffer, outputProduced, finished) == Result::Success);

            result.insert(
                result.end(),
                outputBuffer.begin(),
                outputBuffer.begin() + static_cast<std::ptrdiff_t>(outputProduced)
            );
        }

        return result;
    };

    std::vector<float> expected;
    REQUIRE(Convolution::process(input, impulseResponse, expected) == Result::Success);

    const std::vector<float> firstResult = processInput();

    REQUIRE(firstResult.size() == expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        REQUIRE(firstResult[i] == Catch::Approx(expected[i]).margin(1.0e-4F));
    }

    stream->reset();

    const std::vector<float> secondResult = processInput();

    REQUIRE(secondResult.size() == expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        REQUIRE(secondResult[i] == Catch::Approx(expected[i]).margin(1.0e-4F));
    }
}

} // namespace Grain::DSP::Tests
