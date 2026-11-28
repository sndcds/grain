
#pragma once

#include <grain/Core/Result.hpp>

#include <complex>
#include <cstddef>
#include <memory>
#include <span>

namespace Grain {

/**
 * FFT backend implementation.
 *
 * Automatic:
 *   Selects the best available backend for the requested transform size.
 *   Currently prefers Accelerate when available and supported, otherwise
 *   falls back to Portable.
 *
 * Portable:
 *   Platform-independent C++ implementation.
 *   Supports all positive transform sizes.
 *
 * Accelerate:
 *   Apple Accelerate/vDSP implementation.
 *   Available on macOS for even transform sizes >= 2.
 *
 * FFTW:
 *   Reserved for a future FFTW implementation.
 *   Currently unavailable.
 */
enum class FFTBackendType {
    Automatic,
    Portable,
    Accelerate,
    FFTW
};


/**
 * Configuration used when creating an FFT instance.
 */
struct FFTConfiguration {
    /// Number of real samples in the transform.
    std::size_t size = 1024;

    /// Backend implementation to use.
    FFTBackendType backend = FFTBackendType::Automatic;
};


/**
 * Real-valued Fast Fourier Transform.
 *
 * Transform conventions:
 * - Forward transforms are unnormalized.
 * - Inverse transforms are normalized by 1 / size().
 * - Only the non-redundant half of the real-valued spectrum is exposed.
 *
 * Backend selection:
 * - Automatic selects the best available backend.
 * - Portable is always available for positive transform sizes.
 * - Accelerate is available on macOS for supported sizes.
 * - FFTW is currently unsupported.
 *
 * Memory:
 * Backend setup and working memory are allocated during create().
 * The current implementations do not allocate temporary memory during
 * forward() or inverse().
 *
 * Thread safety:
 * An FFT instance must not be used for concurrent forward() or inverse()
 * calls. Separate FFT instances can be used independently on different
 * threads.
 */
class FFT {
public:
    /**
     * Creates an FFT instance.
     *
     * The output pointer is reset before creation. On success, it contains
     * the newly created FFT instance.
     *
     * Automatic backend selection falls back to Portable when an optimized
     * backend is unavailable or does not support the requested size.
     * Resource allocation errors are returned instead of triggering fallback.
     *
     * @param configuration Transform size and backend configuration.
     * @param output Receives the created FFT instance.
     *
     * @return
     * - Success: FFT instance created.
     * - InvalidArgument: Invalid configuration or zero transform size.
     * - Unsupported: Requested backend unavailable or incompatible.
     * - OutOfMemory: Memory allocation failed.
     * - Error: Other initialization error.
     */
    [[nodiscard]]
    static auto create(
        const FFTConfiguration& configuration,
        std::unique_ptr<FFT>& output
    ) noexcept -> Result;

    ~FFT();

    FFT(const FFT&) = delete;
    auto operator = (const FFT&) -> FFT& = delete;

    FFT(FFT&&) noexcept;
    auto operator = (FFT&&) noexcept -> FFT&;

    /// Returns the number of real samples in the transform.
    [[nodiscard]]
    auto size() const noexcept -> std::size_t;

    /**
     * Returns the number of frequency bins.
     *
     * For a real-valued transform this is size() / 2 + 1,
     * including the DC and Nyquist bins where applicable.
     */
    [[nodiscard]]
    auto binCount() const noexcept -> std::size_t;

    /**
     * Returns the backend implementation selected during creation.
     *
     * This may differ from the requested backend when Automatic is used.
     */
    [[nodiscard]]
    auto backend() const noexcept -> FFTBackendType;

    /**
     * Computes the forward real-to-complex transform.
     *
     * The transform is unnormalized.
     *
     * The output contains size() / 2 + 1 bins, representing frequencies
     * from DC through Nyquist (for even transform sizes).
     *
     * Both spans must have exactly the expected sizes:
     * - input: size() real samples
     * - output: binCount() complex frequency bins
     *
     * No temporary memory allocation is performed.
     *
     * @return Success, InvalidArgument or NotInitialized.
     */
    auto forward(
        std::span<const float> input,
        std::span<std::complex<float>> output
    ) noexcept -> Result;

    /**
     * Computes the inverse complex-to-real transform.
     *
     * The inverse transform is normalized by 1 / size().
     *
     * The input must contain the non-redundant frequency bins of a
     * real-valued signal:
     * - input: binCount() complex frequency bins
     * - output: size() real samples
     *
     * The imaginary components of the DC and Nyquist bins are ignored.
     *
     * Both spans must have exactly the expected sizes.
     *
     * No temporary memory allocation is performed.
     *
     * @return Success, InvalidArgument or NotInitialized.
     */
    auto inverse(
        std::span<const std::complex<float>> input,
        std::span<float> output
    ) noexcept -> Result;

private:
    class Impl;

    explicit FFT(std::unique_ptr<Impl> impl) noexcept;

    std::unique_ptr<Impl> impl_;
};

} // namespace Grain
