
#pragma once

#include <grain/Core/Result.hpp>
#include <grain/DSP/FFT/FFT.hpp>

#include <cstddef>
#include <memory>
#include <span>

namespace Grain {

/**
 * Configuration for streaming convolution.
 */
struct ConvolutionStreamConfiguration {
    /// Number of samples processed per FFT partition.
    std::size_t partitionSize = 1024;

    /// FFT backend used for convolution.
    FFTBackendType backend = FFTBackendType::Automatic;
};


/**
 * Stateful, single-channel, real-time-capable convolution processor.
 *
 * The impulse response is divided into uniform partitions and processed
 * using frequency-domain delay-line convolution.
 *
 * Input and output blocks may have arbitrary sizes. The processor maintains
 * its own partition and output buffering.
 *
 * Memory:
 * - Memory is allocated during create() and setImpulseResponse().
 * - process(), finish() and reset() do not allocate memory.
 *
 * Processing:
 * - process() consumes input and produces available output.
 * - Input may be consumed before corresponding output becomes available.
 * - finish() flushes the final input partition and convolution tail.
 * - finish() must be called repeatedly until finished is true.
 *
 * Thread safety:
 * An instance must not be used concurrently from multiple threads.
 */
class ConvolutionStream {
public:
    /**
     * Creates a streaming convolution processor.
     *
     * @param configuration Partition size and FFT backend.
     * @param output Receives the created processor.
     */
    [[nodiscard]]
    static auto create(
        const ConvolutionStreamConfiguration& configuration,
        std::unique_ptr<ConvolutionStream>& output
    ) noexcept -> Result;

    ~ConvolutionStream();

    ConvolutionStream(const ConvolutionStream&) = delete;
    auto operator=(const ConvolutionStream&) -> ConvolutionStream& = delete;

    ConvolutionStream(ConvolutionStream&&) noexcept;
    auto operator=(ConvolutionStream&&) noexcept -> ConvolutionStream&;

    /**
     * Sets the impulse response.
     *
     * This prepares its frequency-domain partitions and resets the stream.
     * Calling this method replaces the previous impulse response.
     *
     * The impulse response must contain at least one sample.
     *
     * This operation may allocate memory and must not be called from an
     * audio processing callback.
     */
    auto setImpulseResponse(
        std::span<const float> impulseResponse
    ) noexcept -> Result;

    /**
     * Processes an input block.
     *
     * The method consumes as much input as possible and writes as much
     * available output as fits into the output span.
     *
     * inputConsumed reports the number of input samples consumed.
     * outputProduced reports the number of output samples written.
     *
     * Both output counts are set to zero before processing.
     *
     * The impulse response must have been configured first.
     * Processing is not permitted after finish() has been called.
     *
     * No memory allocation is performed.
     */
    auto process(
        std::span<const float> input,
        std::span<float> output,
        std::size_t& inputConsumed,
        std::size_t& outputProduced
    ) noexcept -> Result;

    /**
     * Signals the end of input and flushes the convolution tail.
     *
     * May be called repeatedly. Each call writes as much pending output
     * as fits into the supplied output span.
     *
     * finished becomes true when the complete convolution result has
     * been produced.
     *
     * No memory allocation is performed.
     */
    auto finish(
        std::span<float> output,
        std::size_t& outputProduced,
        bool& finished
    ) noexcept -> Result;

    /**
     * Clears all processing history and pending output.
     *
     * The configured impulse response is retained.
     */
    void reset() noexcept;

    /// Returns the configured FFT partition size.
    [[nodiscard]]
    auto partitionSize() const noexcept -> std::size_t;

    /// Returns the length of the configured impulse response.
    [[nodiscard]]
    auto impulseResponseSize() const noexcept -> std::size_t;

    /// Returns the FFT backend selected during creation.
    [[nodiscard]]
    auto backend() const noexcept -> FFTBackendType;

private:
    class Impl;

    explicit ConvolutionStream(std::unique_ptr<Impl> impl) noexcept;

    std::unique_ptr<Impl> impl_;
};

} // namespace Grain
