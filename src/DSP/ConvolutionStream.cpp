
#include <grain/DSP/ConvolutionStream.hpp>

#include <algorithm>
#include <complex>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>
#include <vector>

namespace Grain {

class ConvolutionStream::Impl {
public:
    Impl(
        std::unique_ptr<FFT> fft,
        std::size_t partitionSize,
        std::size_t binCount
    )
        : fft_(std::move(fft)),
          partition_size_(partitionSize),
          bin_count_(binCount),
          input_partition_(partitionSize, 0.0f),
          output_partition_(partitionSize, 0.0f),
          overlap_(partitionSize, 0.0f),
          spectrum_buffer_(binCount),
          time_buffer_(partitionSize * 2, 0.0f),
          inverse_buffer_(partitionSize * 2, 0.0f) {
    }

    void reset() noexcept {
        std::fill(input_partition_.begin(), input_partition_.end(), 0.0f);
        std::fill(output_partition_.begin(), output_partition_.end(), 0.0f);
        std::fill(overlap_.begin(), overlap_.end(), 0.0f);

        std::fill(input_spectra_.begin(), input_spectra_.end(), std::complex<float>{});

        input_partition_fill_ = 0;
        input_spectrum_write_index_ = 0;

        output_read_index_ = 0;
        output_available_ = 0;

        input_samples_total_ = 0;
        output_samples_total_ = 0;

        tail_blocks_remaining_ = 0;
        finish_started_ = false;
        finished_ = false;
    }

    std::unique_ptr<FFT> fft_;

    std::size_t partition_size_ = 0;
    std::size_t bin_count_ = 0;

    std::size_t impulse_response_size_ = 0;
    std::size_t impulse_partition_count_ = 0;
    bool impulse_response_set_ = false;

    // Precomputed impulse-response spectra.
    std::vector<std::complex<float>> impulse_spectra_;

    // Circular history of input spectra.
    std::vector<std::complex<float>> input_spectra_;

    // Time-domain and frequency-domain working buffers.
    std::vector<float> input_partition_;
    std::vector<float> output_partition_;
    std::vector<float> overlap_;

    std::vector<std::complex<float>> spectrum_buffer_;
    std::vector<float> time_buffer_;
    std::vector<float> inverse_buffer_;

    // Streaming state.
    std::size_t input_partition_fill_ = 0;
    std::size_t input_spectrum_write_index_ = 0;

    std::size_t output_read_index_ = 0;
    std::size_t output_available_ = 0;

    std::size_t input_samples_total_ = 0;
    std::size_t output_samples_total_ = 0;

    std::size_t tail_blocks_remaining_ = 0;

    bool finish_started_ = false;
    bool finished_ = false;
};


ConvolutionStream::ConvolutionStream(std::unique_ptr<Impl> impl) noexcept
    : impl_(std::move(impl)) {
}


ConvolutionStream::~ConvolutionStream() = default;

ConvolutionStream::ConvolutionStream(ConvolutionStream&&) noexcept = default;

auto ConvolutionStream::operator=(ConvolutionStream&&) noexcept
    -> ConvolutionStream& = default;


auto ConvolutionStream::create(
    const ConvolutionStreamConfiguration& configuration,
    std::unique_ptr<ConvolutionStream>& output
) noexcept -> Result {

    output.reset();

    if (configuration.partitionSize == 0 ||
        configuration.partitionSize > std::numeric_limits<std::size_t>::max() / 2) {
        return Result::InvalidArgument;
    }

    try {
        FFTConfiguration fft_configuration;
        fft_configuration.size = configuration.partitionSize * 2;
        fft_configuration.backend = configuration.backend;

        std::unique_ptr<FFT> fft;

        const Result fft_result = FFT::create(fft_configuration, fft);

        if (fft_result != Result::Success) {
            return fft_result;
        }

        const std::size_t bin_count = fft->binCount();

        std::unique_ptr<Impl> impl = std::make_unique<Impl>(
            std::move(fft),
            configuration.partitionSize,
            bin_count
        );

        output.reset(new ConvolutionStream(std::move(impl)));

        return Result::Success;
    }
    catch (const std::bad_alloc&) {
        return Result::OutOfMemory;
    }
    catch (const std::length_error&) {
        return Result::InvalidArgument;
    }
    catch (...) {
        return Result::Error;
    }
}


auto ConvolutionStream::setImpulseResponse(
    std::span<const float> impulseResponse
) noexcept -> Result {

    if (!impl_) {
        return Result::NotInitialized;
    }

    if (impulseResponse.empty()) {
        return Result::InvalidArgument;
    }

    try {
        const std::size_t partitionCount =
            1 + (impulseResponse.size() - 1) / impl_->partition_size_;

        const std::size_t binCount = impl_->bin_count_;

        const std::vector<std::complex<float>> storage;

        if (partitionCount > storage.max_size() / binCount) {
            return Result::InvalidArgument;
        }

        const std::size_t spectrumSize = partitionCount * binCount;

        std::vector<std::complex<float>> impulseSpectra(spectrumSize);
        std::vector<std::complex<float>> inputSpectra(spectrumSize);

        for (std::size_t partitionIndex = 0;
             partitionIndex < partitionCount;
             ++partitionIndex) {

            const std::size_t inputOffset =
                partitionIndex * impl_->partition_size_;

            const std::size_t sampleCount = std::min(
                impl_->partition_size_,
                impulseResponse.size() - inputOffset
            );

            std::fill(
                impl_->time_buffer_.begin(),
                impl_->time_buffer_.end(),
                0.0f
            );

            std::copy_n(
                impulseResponse.data() + inputOffset,
                sampleCount,
                impl_->time_buffer_.data()
            );

            const std::span<std::complex<float>> spectrum(
                impulseSpectra.data() + partitionIndex * binCount,
                binCount
            );

            const Result result = impl_->fft_->forward(
                impl_->time_buffer_,
                spectrum
            );

            if (result != Result::Success) {
                return result;
            }
        }

        // Commit the new impulse response only after successful preparation.
        impl_->impulse_spectra_.swap(impulseSpectra);
        impl_->input_spectra_.swap(inputSpectra);

        impl_->impulse_response_size_ = impulseResponse.size();
        impl_->impulse_partition_count_ = partitionCount;
        impl_->impulse_response_set_ = true;

        impl_->reset();

        return Result::Success;
    }
    catch (const std::bad_alloc&) {
        return Result::OutOfMemory;
    }
    catch (const std::length_error&) {
        return Result::InvalidArgument;
    }
    catch (...) {
        return Result::Error;
    }
}


void ConvolutionStream::reset() noexcept {
    if (impl_) {
        impl_->reset();
    }
}


auto ConvolutionStream::partitionSize() const noexcept -> std::size_t {
    return impl_ ? impl_->partition_size_ : 0;
}


auto ConvolutionStream::impulseResponseSize() const noexcept -> std::size_t {
    return impl_ ? impl_->impulse_response_size_ : 0;
}


auto ConvolutionStream::backend() const noexcept -> FFTBackendType {
    return impl_ ? impl_->fft_->backend() : FFTBackendType::Automatic;
}

} // namespace Grain
