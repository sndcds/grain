
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
        output_samples_generated_ = 0;
        output_samples_total_ = 0;
        finish_target_samples_ = 0;

        finish_started_ = false;
        finished_ = false;
    }

    auto processPartition(std::size_t validOutputSamples) noexcept -> Result {
        if (impulse_partition_count_ == 0 ||
            validOutputSamples > partition_size_) {
            return Result::InvalidArgument;
        }

        // Transform the current input partition.
        const std::size_t writeOffset =
            input_spectrum_write_index_ * bin_count_;

        std::span<std::complex<float>> inputSpectrum(
            input_spectra_.data() + writeOffset,
            bin_count_
        );

        Result result = fft_->forward(time_buffer_, inputSpectrum);

        if (result != Result::Success) {
            return result;
        }

        // Accumulate the frequency-domain products.
        std::fill(spectrum_buffer_.begin(), spectrum_buffer_.end(),
                  std::complex<float>{});

        for (std::size_t partitionIndex = 0;
             partitionIndex < impulse_partition_count_;
             ++partitionIndex) {

            const std::size_t historyIndex =
                (input_spectrum_write_index_ +
                 impulse_partition_count_ -
                 partitionIndex) % impulse_partition_count_;

            const std::size_t inputOffset = historyIndex * bin_count_;
            const std::size_t impulseOffset = partitionIndex * bin_count_;

            for (std::size_t binIndex = 0; binIndex < bin_count_; ++binIndex) {
                spectrum_buffer_[binIndex] +=
                    input_spectra_[inputOffset + binIndex] *
                    impulse_spectra_[impulseOffset + binIndex];
            }
        }

        // Transform the accumulated spectrum back to the time domain.
        result = fft_->inverse(spectrum_buffer_, inverse_buffer_);

        if (result != Result::Success) {
            return result;
        }

        // Overlap-add.
        for (std::size_t sampleIndex = 0;
             sampleIndex < partition_size_;
             ++sampleIndex) {

            output_partition_[sampleIndex] =
                inverse_buffer_[sampleIndex] + overlap_[sampleIndex];

            overlap_[sampleIndex] =
                inverse_buffer_[partition_size_ + sampleIndex];
        }

        output_read_index_ = 0;
        output_available_ = validOutputSamples;

        output_samples_generated_ += validOutputSamples;

        // Advance the circular input-spectrum history.
        input_spectrum_write_index_ =
            (input_spectrum_write_index_ + 1) % impulse_partition_count_;

        std::fill(input_partition_.begin(), input_partition_.end(), 0.0f);
        input_partition_fill_ = 0;

        return Result::Success;
    }

    void copyPendingOutput(
        std::span<float> output,
        std::size_t& outputOffset
    ) noexcept {
        const std::size_t copyCount = std::min(
            output_available_,
            output.size() - outputOffset
        );

        if (copyCount == 0) {
            return;
        }

        std::copy_n(
            output_partition_.data() + output_read_index_,
            copyCount,
            output.data() + outputOffset
        );

        output_read_index_ += copyCount;
        output_available_ -= copyCount;
        outputOffset += copyCount;
        output_samples_total_ += copyCount;

        if (output_available_ == 0) {
            output_read_index_ = 0;
        }
    }

    std::unique_ptr<FFT> fft_;

    std::size_t partition_size_ = 0;
    std::size_t bin_count_ = 0;

    std::size_t impulse_response_size_ = 0;
    std::size_t impulse_partition_count_ = 0;
    bool impulse_response_set_ = false;

    std::vector<std::complex<float>> impulse_spectra_;
    std::vector<std::complex<float>> input_spectra_;

    std::vector<float> input_partition_;
    std::vector<float> output_partition_;
    std::vector<float> overlap_;

    std::vector<std::complex<float>> spectrum_buffer_;
    std::vector<float> time_buffer_;
    std::vector<float> inverse_buffer_;

    std::size_t input_partition_fill_ = 0;
    std::size_t input_spectrum_write_index_ = 0;

    std::size_t output_read_index_ = 0;
    std::size_t output_available_ = 0;

    std::size_t input_samples_total_ = 0;
    std::size_t output_samples_generated_ = 0;
    std::size_t output_samples_total_ = 0;
    std::size_t finish_target_samples_ = 0;

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
        FFTConfiguration fftConfiguration;
        fftConfiguration.size = configuration.partitionSize * 2;
        fftConfiguration.backend = configuration.backend;

        std::unique_ptr<FFT> fft;

        const Result fftResult = FFT::create(fftConfiguration, fft);

        if (fftResult != Result::Success) {
            return fftResult;
        }

        const std::size_t binCount = fft->binCount();

        std::unique_ptr<Impl> impl = std::make_unique<Impl>(
            std::move(fft),
            configuration.partitionSize,
            binCount
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

            std::span<std::complex<float>> spectrum(
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


auto ConvolutionStream::process(
    std::span<const float> input,
    std::span<float> output,
    std::size_t& inputConsumed,
    std::size_t& outputProduced
) noexcept -> Result {

    inputConsumed = 0;
    outputProduced = 0;

    if (!impl_) {
        return Result::NotInitialized;
    }

    if (!impl_->impulse_response_set_) {
        return Result::NotInitialized;
    }

    if (impl_->finish_started_) {
        return Result::InvalidArgument;
    }

    if (input.size() >
        std::numeric_limits<std::size_t>::max() - impl_->input_samples_total_) {
        return Result::InvalidArgument;
    }

    std::size_t outputOffset = 0;

    while (true) {
        // First deliver any output left from the previous call.
        impl_->copyPendingOutput(output, outputOffset);

        if (outputOffset == output.size()) {
            break;
        }

        if (impl_->output_available_ > 0) {
            break;
        }

        // No more input to consume.
        if (inputConsumed == input.size()) {
            break;
        }

        // Collect samples until one complete partition is available.
        const std::size_t copyCount = std::min(
            impl_->partition_size_ - impl_->input_partition_fill_,
            input.size() - inputConsumed
        );

        std::copy_n(
            input.data() + inputConsumed,
            copyCount,
            impl_->input_partition_.data() + impl_->input_partition_fill_
        );

        inputConsumed += copyCount;
        impl_->input_partition_fill_ += copyCount;
        impl_->input_samples_total_ += copyCount;

        if (impl_->input_partition_fill_ < impl_->partition_size_) {
            break;
        }

        // The input partition is complete. Prepare the FFT input.
        std::copy(
            impl_->input_partition_.begin(),
            impl_->input_partition_.end(),
            impl_->time_buffer_.begin()
        );

        std::fill(
            impl_->time_buffer_.begin() +
                static_cast<std::ptrdiff_t>(impl_->partition_size_),
            impl_->time_buffer_.end(),
            0.0f
        );

        const Result result = impl_->processPartition(impl_->partition_size_);

        if (result != Result::Success) {
            return result;
        }
    }

    outputProduced = outputOffset;

    return Result::Success;
}


auto ConvolutionStream::finish(
    std::span<float> output,
    std::size_t& outputProduced,
    bool& finished
) noexcept -> Result {

    outputProduced = 0;
    finished = false;

    if (!impl_) {
        return Result::NotInitialized;
    }

    if (!impl_->impulse_response_set_) {
        return Result::NotInitialized;
    }

    if (impl_->finished_) {
        finished = true;
        return Result::Success;
    }

    if (!impl_->finish_started_) {
        impl_->finish_started_ = true;

        if (impl_->input_samples_total_ == 0) {
            impl_->finished_ = true;
            finished = true;
            return Result::Success;
        }

        if (impl_->impulse_response_size_ - 1 >
            std::numeric_limits<std::size_t>::max() - impl_->input_samples_total_) {
            return Result::InvalidArgument;
        }

        impl_->finish_target_samples_ =
            impl_->input_samples_total_ + impl_->impulse_response_size_ - 1;
    }

    std::size_t outputOffset = 0;

    while (outputOffset < output.size()) {
        // Deliver any output already waiting.
        impl_->copyPendingOutput(output, outputOffset);

        if (outputOffset == output.size()) {
            break;
        }

        if (impl_->output_available_ > 0) {
            break;
        }

        if (impl_->output_samples_generated_ >= impl_->finish_target_samples_) {
            break;
        }

        // Prepare the next partition. This is either the final partial
        // input partition or a zero-input partition used to flush the tail.
        if (impl_->input_partition_fill_ < impl_->partition_size_) {
            std::fill(
                impl_->input_partition_.begin() +
                    static_cast<std::ptrdiff_t>(impl_->input_partition_fill_),
                impl_->input_partition_.end(),
                0.0f
            );
        }

        std::copy(
            impl_->input_partition_.begin(),
            impl_->input_partition_.end(),
            impl_->time_buffer_.begin()
        );

        std::fill(
            impl_->time_buffer_.begin() +
                static_cast<std::ptrdiff_t>(impl_->partition_size_),
            impl_->time_buffer_.end(),
            0.0f
        );

        const std::size_t remainingSamples =
            impl_->finish_target_samples_ - impl_->output_samples_generated_;

        const std::size_t validOutputSamples = std::min(
            impl_->partition_size_,
            remainingSamples
        );

        const Result result = impl_->processPartition(validOutputSamples);

        if (result != Result::Success) {
            return result;
        }
    }

    outputProduced = outputOffset;

    impl_->finished_ =
        impl_->output_samples_total_ == impl_->finish_target_samples_ &&
        impl_->output_available_ == 0;

    finished = impl_->finished_;

    return Result::Success;
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
