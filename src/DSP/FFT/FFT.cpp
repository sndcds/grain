#include <grain/DSP/FFT/FFT.hpp>

#include "FFTBackend.hpp"

#include "Portable/PortableFFT.hpp"

#if defined(GRAIN_HAS_ACCELERATE)
#include "macOS/AccelerateFFT.hpp"
#endif

#include <new>
#include <utility>

namespace Grain {

class FFT::Impl {
public:
    explicit Impl(std::unique_ptr<FFTBackend> backend) noexcept
        : backend_(std::move(backend)) {
    }

    std::unique_ptr<FFTBackend> backend_;
};


FFT::FFT(std::unique_ptr<Impl> impl) noexcept
    : impl_(std::move(impl)) {
}


FFT::~FFT() = default;

FFT::FFT(FFT&&) noexcept = default;

auto FFT::operator = (FFT&&) noexcept -> FFT& = default;


auto FFT::create(
    const FFTConfiguration& configuration,
    std::unique_ptr<FFT>& output
) noexcept -> Result {

    output.reset();

    if (configuration.size == 0) {
        return Result::InvalidArgument;
    }

    if (
        configuration.backend != FFTBackendType::Automatic &&
        configuration.backend != FFTBackendType::Portable &&
        configuration.backend != FFTBackendType::Accelerate &&
        configuration.backend != FFTBackendType::FFTW
    ) {
        return Result::InvalidArgument;
    }

    //--------------------------------------------------------------------------
    //  Backend Selection
    //--------------------------------------------------------------------------

    try {
        std::unique_ptr<FFTBackend> backend;

        if (configuration.backend == FFTBackendType::Automatic) {

#if defined(GRAIN_HAS_ACCELERATE)
            const Result accelerate_result =
                createAccelerateFFT(configuration.size, backend);

            if (accelerate_result == Result::OutOfMemory ||
                accelerate_result == Result::Error) {
                return accelerate_result;
                }
#endif

            // Use the portable backend if Accelerate is unavailable
            // or does not support this transform size.
            if (!backend) {
                backend = std::make_unique<PortableFFT>(
                    configuration.size
                );
            }
        }
        else if (configuration.backend == FFTBackendType::Accelerate) {

#if defined(GRAIN_HAS_ACCELERATE)
            const Result result =
                createAccelerateFFT(configuration.size, backend);

            if (result != Result::Success) {
                return result;
            }
#else
            return Result::Unsupported;
#endif

        }
        else if (configuration.backend == FFTBackendType::Portable) {
            backend = std::make_unique<PortableFFT>(
                configuration.size
            );
        }
        else {
            // FFTW is implemented in a later milestone.
            return Result::Unsupported;
        }

        std::unique_ptr<Impl> impl =
            std::make_unique<Impl>(std::move(backend));

        output.reset(new FFT(std::move(impl)));

        return Result::Success;
    }
    catch (const std::bad_alloc&) {
        return Result::OutOfMemory;
    }
    catch (...) {
        return Result::Error;
    }
}


auto FFT::size() const noexcept -> std::size_t {
    return impl_ ? impl_->backend_->size() : 0;
}


auto FFT::binCount() const noexcept -> std::size_t {
    return impl_ ? impl_->backend_->binCount() : 0;
}


auto FFT::backend() const noexcept -> FFTBackendType {
    return impl_
        ? impl_->backend_->type()
        : FFTBackendType::Automatic;
}


auto FFT::forward(
    std::span<const float> input,
    std::span<std::complex<float>> output
) noexcept -> Result {
    if (!impl_) {
        return Result::NotInitialized;
    }

    return impl_->backend_->forward(input, output);
}


auto FFT::inverse(
    std::span<const std::complex<float>> input,
    std::span<float> output
) noexcept -> Result {
    if (!impl_) {
        return Result::NotInitialized;
    }

    return impl_->backend_->inverse(input, output);
}

} // namespace Grain