#include <grain/DSP/FFT/FFT.hpp>

#include <grain/DSP/FFT/FFTBackend.hpp>
#include "Portable/PortableFFT.hpp"

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

auto FFT::operator=(FFT&&) noexcept -> FFT& = default;


auto FFT::create(
    const FFTConfiguration& configuration,
    std::unique_ptr<FFT>& output
) noexcept -> Result {
    output.reset();

    if (configuration.size == 0) {
        return Result::InvalidArgument;
    }

    if (configuration.backend == FFTBackendType::Accelerate ||
        configuration.backend == FFTBackendType::FFTW) {
        return Result::Unsupported;
    }

    if (configuration.backend != FFTBackendType::Automatic &&
        configuration.backend != FFTBackendType::Portable) {
        return Result::InvalidArgument;
    }

    try {
        std::unique_ptr<FFTBackend> backend =
            std::make_unique<PortableFFT>(configuration.size);

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