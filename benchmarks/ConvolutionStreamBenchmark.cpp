
#include <grain/DSP/ConvolutionStream.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numbers>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

constexpr std::size_t kSampleRate = 48000;
constexpr std::size_t kHostBlockSize = 128;
constexpr std::size_t kInputSampleCount = kSampleRate * 2;
constexpr std::size_t kWarmupBlockCount = 10;

constexpr std::array<std::size_t, 6> kPartitionSizes = {
    64, 128, 256, 512, 1024, 2048
};

constexpr std::array<std::size_t, 3> kImpulseResponseSizes = {
    4800, 24000, 48000
};

auto backendName(Grain::FFTBackendType backend) -> const char* {
    switch (backend) {
        case Grain::FFTBackendType::Portable:
            return "Portable";

        case Grain::FFTBackendType::Accelerate:
            return "Accelerate";

        default:
            return "Unknown";
    }
}


auto processSamples(
    Grain::ConvolutionStream& stream,
    std::size_t sampleCount,
    std::vector<float>& outputBuffer,
    std::vector<double>* measurements,
    std::size_t& outputSampleCount
) -> bool {

    std::array<float, kHostBlockSize> inputBlock{};

    for (std::size_t i = 0; i < inputBlock.size(); ++i) {
        const double phase =
            2.0 * std::numbers::pi_v<double>
            * 440.0
            * static_cast<double>(i)
            / static_cast<double>(kSampleRate);

        inputBlock[i] = static_cast<float>(std::sin(phase));
    }

    for (std::size_t block = 0; block < sampleCount / kHostBlockSize; ++block) {
        std::size_t inputOffset = 0;

        while (inputOffset < inputBlock.size()) {
            std::size_t inputConsumed = 0;
            std::size_t outputProduced = 0;

            const auto input = std::span<const float>(inputBlock).subspan(inputOffset);
            const auto output = std::span<float>(outputBuffer);

            const auto start = Clock::now();

            const Grain::Result result = stream.process(
                input,
                output,
                inputConsumed,
                outputProduced
            );

            const auto end = Clock::now();

            if (result != Grain::Result::Success) {
                return false;
            }

            if (inputConsumed > input.size()) {
                return false;
            }

            if (inputConsumed == 0 && outputProduced == 0) {
                return false;
            }

            inputOffset += inputConsumed;
            outputSampleCount += outputProduced;

            if (measurements != nullptr) {
                measurements->push_back(
                    std::chrono::duration<double, std::micro>(end - start).count()
                );
            }
        }
    }

    return true;
}


auto finishStream(
    Grain::ConvolutionStream& stream,
    std::vector<float>& outputBuffer,
    std::size_t& outputSampleCount
) -> bool {

    bool finished = false;

    while (!finished) {
        std::size_t outputProduced = 0;

        const Grain::Result result = stream.finish(
            outputBuffer,
            outputProduced,
            finished
        );

        if (result != Grain::Result::Success) {
            return false;
        }

        outputSampleCount += outputProduced;

        if (!finished && outputProduced == 0) {
            return false;
        }
    }

    return true;
}


auto percentile(const std::vector<double>& sortedValues, double value) -> double {
    if (sortedValues.empty()) {
        return 0.0;
    }

    const std::size_t index = static_cast<std::size_t>(
        value * static_cast<double>(sortedValues.size() - 1)
    );

    return sortedValues[index];
}


auto runBenchmark(
    std::size_t partitionSize,
    std::size_t impulseResponseSize
) -> bool {

    using namespace Grain;

    ConvolutionStreamConfiguration configuration;
    configuration.partitionSize = partitionSize;
    configuration.backend = FFTBackendType::Automatic;

    std::unique_ptr<ConvolutionStream> stream;

    if (ConvolutionStream::create(configuration, stream) != Result::Success) {
        std::cerr << "Could not create ConvolutionStream.\n";
        return false;
    }

    std::vector<float> impulseResponse(impulseResponseSize);

    impulseResponse[0] = 1.0f;

    for (std::size_t i = 1; i < impulseResponse.size(); ++i) {
        const double position =
            static_cast<double>(i) / static_cast<double>(impulseResponse.size());

        const double phase =
            2.0 * std::numbers::pi_v<double>
            * 1800.0
            * static_cast<double>(i)
            / static_cast<double>(kSampleRate);

        impulseResponse[i] = static_cast<float>(
            0.5 * std::exp(-6.0 * position) * std::sin(phase)
        );
    }

    if (stream->setImpulseResponse(impulseResponse) != Result::Success) {
        std::cerr << "Could not configure impulse response.\n";
        return false;
    }

    std::vector<float> outputBuffer(partitionSize * 2);

    // Warm up the processing path.
    std::size_t warmupOutputCount = 0;

    if (!processSamples(
        *stream,
        kHostBlockSize * kWarmupBlockCount,
        outputBuffer,
        nullptr,
        warmupOutputCount
    )) {
        std::cerr << "Warm-up processing failed.\n";
        return false;
    }

    stream->reset();

    // Measure the processing path.
    std::vector<double> measurements;
    measurements.reserve(kInputSampleCount / kHostBlockSize);

    std::size_t outputSampleCount = 0;

    if (!processSamples(
        *stream,
        kInputSampleCount,
        outputBuffer,
        &measurements,
        outputSampleCount
    )) {
        std::cerr << "Measured processing failed.\n";
        return false;
    }

    if (!finishStream(*stream, outputBuffer, outputSampleCount)) {
        std::cerr << "Finishing the stream failed.\n";
        return false;
    }

    const std::size_t expectedOutputCount =
        kInputSampleCount + impulseResponseSize - 1;

    if (outputSampleCount != expectedOutputCount) {
        std::cerr
            << "Unexpected output length: "
            << outputSampleCount
            << ", expected: "
            << expectedOutputCount
            << '\n';

        return false;
    }

    std::sort(measurements.begin(), measurements.end());

    double totalMicroseconds = 0.0;

    for (const double measurement : measurements) {
        totalMicroseconds += measurement;
    }

    const double averageMicroseconds =
        totalMicroseconds / static_cast<double>(measurements.size());

    const double p95 = percentile(measurements, 0.95);
    const double p99 = percentile(measurements, 0.99);
    const double maximum = measurements.back();

    const double callbackBudgetMicroseconds =
        1'000'000.0
        * static_cast<double>(kHostBlockSize)
        / static_cast<double>(kSampleRate);

    const double impulseResponseMilliseconds =
        1000.0
        * static_cast<double>(impulseResponseSize)
        / static_cast<double>(kSampleRate);

    std::cout
        << std::left
        << std::setw(13) << backendName(stream->backend())
        << std::right
        << std::setw(8) << partitionSize
        << std::setw(10) << std::fixed << std::setprecision(0)
        << impulseResponseMilliseconds
        << std::setw(13) << std::setprecision(2)
        << averageMicroseconds
        << std::setw(12) << p95
        << std::setw(12) << p99
        << std::setw(12) << maximum
        << '\n';

    return true;
}

} // namespace


auto main() -> int {
    std::cout
        << "\nGrain ConvolutionStream Benchmarks\n"
        << "==================================\n\n"
        << "Sample rate:       " << kSampleRate << " Hz\n"
        << "Host block size:   " << kHostBlockSize << " samples\n"
        << "Input duration:    "
        << (1000 * kInputSampleCount / kSampleRate) << " ms\n"
        << "Callback budget:   "
        << std::fixed << std::setprecision(2)
        << (1'000'000.0 * kHostBlockSize / kSampleRate) << " us\n\n";

    std::cout
        << std::left
        << std::setw(13) << "Backend"
        << std::right
        << std::setw(8) << "Part."
        << std::setw(10) << "IR (ms)"
        << std::setw(13) << "Avg (us)"
        << std::setw(12) << "P95 (us)"
        << std::setw(12) << "P99 (us)"
        << std::setw(12) << "Max (us)"
        << '\n';

    std::cout << std::string(80, '-') << '\n';

    bool success = true;

    for (const std::size_t partitionSize : kPartitionSizes) {
        for (const std::size_t impulseResponseSize : kImpulseResponseSizes) {
            success = runBenchmark(partitionSize, impulseResponseSize) && success;
        }
    }

    std::cout << '\n';

    return success ? 0 : 1;
}
