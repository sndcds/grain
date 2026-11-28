
#include <grain/DSP/ConvolutionStream.hpp>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <new>

namespace {

std::atomic<bool> allocation_tracking_enabled = false;
std::atomic<std::size_t> allocation_count = 0;

void startAllocationTracking() noexcept {
    allocation_count.store(0, std::memory_order_relaxed);
    allocation_tracking_enabled.store(true, std::memory_order_relaxed);
}

auto stopAllocationTracking() noexcept -> std::size_t {
    allocation_tracking_enabled.store(false, std::memory_order_relaxed);
    return allocation_count.load(std::memory_order_relaxed);
}

} // namespace


//-------------------------------------------------------------------------------
//  Global allocation instrumentation.
//-------------------------------------------------------------------------------

auto operator new(std::size_t size) -> void* {
    if (allocation_tracking_enabled.load(std::memory_order_relaxed)) {
        allocation_count.fetch_add(1, std::memory_order_relaxed);
    }

    if (void* memory = std::malloc(size == 0 ? 1 : size)) {
        return memory;
    }

    throw std::bad_alloc();
}

auto operator new[](std::size_t size) -> void* {
    return ::operator new(size);
}

void operator delete(void* memory) noexcept {
    std::free(memory);
}

void operator delete[](void* memory) noexcept {
    std::free(memory);
}

void operator delete(void* memory, std::size_t) noexcept {
    std::free(memory);
}

void operator delete[](void* memory, std::size_t) noexcept {
    std::free(memory);
}


//-------------------------------------------------------------------------------
//  Test.
//-------------------------------------------------------------------------------

auto main() -> int {
    using namespace Grain;

    constexpr std::size_t partitionSize = 64;

    ConvolutionStreamConfiguration configuration;
    configuration.partitionSize = partitionSize;
    configuration.backend = FFTBackendType::Portable;

    std::unique_ptr<ConvolutionStream> stream;

    if (ConvolutionStream::create(configuration, stream) != Result::Success) {
        std::fprintf(stderr, "Failed to create ConvolutionStream.\n");
        return 1;
    }

    std::array<float, 256> input {};
    std::array<float, 129> impulseResponse {};
    std::array<float, 512> output {};

    // Prepare all test data before enabling allocation tracking.
    for (std::size_t i = 0; i < input.size(); ++i) {
        input[i] = static_cast<float>(i % 17) / 17.0f;
    }

    for (std::size_t i = 0; i < impulseResponse.size(); ++i) {
        impulseResponse[i] = static_cast<float>(i % 7) / 7.0f;
    }

    if (stream->setImpulseResponse(impulseResponse) != Result::Success) {
        std::fprintf(stderr, "Failed to configure impulse response.\n");
        return 1;
    }

    bool success = true;

    startAllocationTracking();

    //--------------------------------------------------------------------------
    //  First processing run.
    //--------------------------------------------------------------------------

    std::size_t inputConsumed = 0;
    std::size_t outputProduced = 0;

    if (stream->process(
        input,
        output,
        inputConsumed,
        outputProduced
    ) != Result::Success) {
        success = false;
    }

    if (inputConsumed != input.size()) {
        success = false;
    }

    bool finished = false;

    while (success && !finished) {
        if (stream->finish(
            output,
            outputProduced,
            finished
        ) != Result::Success) {
            success = false;
        }
    }

    //--------------------------------------------------------------------------
    //  Reset and process again.
    //--------------------------------------------------------------------------

    if (success) {
        stream->reset();

        inputConsumed = 0;
        outputProduced = 0;

        if (stream->process(
            input,
            output,
            inputConsumed,
            outputProduced
        ) != Result::Success) {
            success = false;
        }

        if (inputConsumed != input.size()) {
            success = false;
        }

        finished = false;

        while (success && !finished) {
            if (stream->finish(
                output,
                outputProduced,
                finished
            ) != Result::Success) {
                success = false;
            }
        }
    }

    const std::size_t allocations = stopAllocationTracking();

    //--------------------------------------------------------------------------
    //  Report results outside the tracked region.
    //--------------------------------------------------------------------------

    if (!success) {
        std::fprintf(stderr, "ConvolutionStream processing failed.\n");
        return 1;
    }

    if (allocations != 0) {
        std::fprintf(
            stderr,
            "Unexpected heap allocations during processing: %zu\n",
            allocations
        );
        return 1;
    }

    std::printf(
        "ConvolutionStream real-time allocation test passed.\n"
        "Heap allocations during processing: %zu\n",
        allocations
    );

    return 0;
}
