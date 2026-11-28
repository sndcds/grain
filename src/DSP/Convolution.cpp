
#include <grain/DSP/Convolution.hpp>

#include <algorithm>
#include <complex>
#include <limits>
#include <new>
#include <vector>
#include <stdexcept>

namespace Grain {

auto Convolution::process(
    std::span<const float> input,
    std::span<const float> impulseResponse,
    std::vector<float>& output,
    const ConvolutionConfiguration& configuration
) noexcept -> Result {

    output.clear();

    if (input.empty() || impulseResponse.empty() ||
        configuration.partitionSize == 0) {
        return Result::InvalidArgument;
    }

    const std::size_t partitionSize = configuration.partitionSize;

    if (partitionSize > std::numeric_limits<std::size_t>::max() / 2) {
        return Result::InvalidArgument;
    }

    if (input.size() >
        std::numeric_limits<std::size_t>::max() - impulseResponse.size() + 1) {
        return Result::InvalidArgument;
    }

    const std::size_t outputSize =
        input.size() + impulseResponse.size() - 1;

    const std::size_t fftSize = partitionSize * 2;
    const std::size_t inputPartitionCount =
        1 + (input.size() - 1) / partitionSize;
    const std::size_t irPartitionCount =
        1 + (impulseResponse.size() - 1) / partitionSize;
    const std::size_t outputPartitionCount =
        1 + (outputSize - 1) / partitionSize;

    try {
        //----------------------------------------------------------------------
        //  Create FFT
        //----------------------------------------------------------------------

        FFTConfiguration fftConfiguration;
        fftConfiguration.size = fftSize;
        fftConfiguration.backend = configuration.backend;

        std::unique_ptr<FFT> fft;

        const Result fftResult = FFT::create(fftConfiguration, fft);

        if (fftResult != Result::Success) {
            return fftResult;
        }

        const std::size_t binCount = fft->binCount();


        //----------------------------------------------------------------------
        //  Validate allocation sizes
        //----------------------------------------------------------------------

        const std::vector<std::complex<float>> spectrumStorage;

        if (inputPartitionCount > spectrumStorage.max_size() / binCount ||
            irPartitionCount > spectrumStorage.max_size() / binCount) {
            return Result::InvalidArgument;
            }

        if (outputSize > output.max_size()) {
            return Result::InvalidArgument;
        }


        //----------------------------------------------------------------------
        //  Allocate working memory
        //----------------------------------------------------------------------

        std::vector<std::complex<float>> inputSpectra(
            inputPartitionCount * binCount
        );

        std::vector<std::complex<float>> irSpectra(
            irPartitionCount * binCount
        );

        std::vector<float> timeBuffer(fftSize, 0.0f);
        std::vector<float> inverseBuffer(fftSize, 0.0f);

        std::vector<std::complex<float>> spectrumBuffer(binCount);

        output.assign(outputSize, 0.0f);


        //----------------------------------------------------------------------
        //  Transform input partitions
        //----------------------------------------------------------------------

        for (std::size_t partition = 0;
             partition < inputPartitionCount;
             ++partition) {

            std::fill(timeBuffer.begin(), timeBuffer.end(), 0.0f);

            const std::size_t offset = partition * partitionSize;
            const std::size_t count = std::min(
                partitionSize,
                input.size() - offset
            );

            std::copy_n(
                input.begin() + static_cast<std::ptrdiff_t>(offset),
                count,
                timeBuffer.begin()
            );

            const std::span<std::complex<float>> spectrum(
                inputSpectra.data() + partition * binCount,
                binCount
            );

            const Result result = fft->forward(timeBuffer, spectrum);

            if (result != Result::Success) {
                output.clear();
                return result;
            }
        }

        //----------------------------------------------------------------------
        //  Transform impulse-response partitions
        //----------------------------------------------------------------------

        for (std::size_t partition = 0;
             partition < irPartitionCount;
             ++partition) {

            std::fill(timeBuffer.begin(), timeBuffer.end(), 0.0f);

            const std::size_t offset = partition * partitionSize;
            const std::size_t count = std::min(
                partitionSize,
                impulseResponse.size() - offset
            );

            std::copy_n(
                impulseResponse.begin() + static_cast<std::ptrdiff_t>(offset),
                count,
                timeBuffer.begin()
            );

            const std::span<std::complex<float>> spectrum(
                irSpectra.data() + partition * binCount,
                binCount
            );

            const Result result = fft->forward(timeBuffer, spectrum);

            if (result != Result::Success) {
                output.clear();
                return result;
            }
        }

        //----------------------------------------------------------------------
        //  Partitioned convolution
        //----------------------------------------------------------------------

        for (std::size_t outputPartition = 0;
             outputPartition < outputPartitionCount;
             ++outputPartition) {

            std::fill(
                spectrumBuffer.begin(),
                spectrumBuffer.end(),
                std::complex<float>{0.0f, 0.0f}
            );

            const std::size_t firstIRPartition =
                outputPartition >= inputPartitionCount
                    ? outputPartition - inputPartitionCount + 1
                    : 0;

            const std::size_t lastIRPartition =
                std::min(outputPartition, irPartitionCount - 1);

            //------------------------------------------------------------------
            //  Accumulate frequency-domain products
            //------------------------------------------------------------------

            for (std::size_t irPartition = firstIRPartition;
                 irPartition <= lastIRPartition;
                 ++irPartition) {

                const std::size_t inputPartition =
                    outputPartition - irPartition;

                const std::complex<float>* inputSpectrum =
                    inputSpectra.data() + inputPartition * binCount;

                const std::complex<float>* irSpectrum =
                    irSpectra.data() + irPartition * binCount;

                for (std::size_t bin = 0; bin < binCount; ++bin) {
                    spectrumBuffer[bin] +=
                        inputSpectrum[bin] * irSpectrum[bin];
                }
            }

            //------------------------------------------------------------------
            //  Inverse transform
            //------------------------------------------------------------------

            const Result result = fft->inverse(
                spectrumBuffer,
                inverseBuffer
            );

            if (result != Result::Success) {
                output.clear();
                return result;
            }

            //------------------------------------------------------------------
            //  Overlap-add
            //----------------------------------------------------------------------

            const std::size_t outputOffset =
                outputPartition * partitionSize;

            const std::size_t count = std::min(
                fftSize,
                outputSize - outputOffset
            );

            for (std::size_t i = 0; i < count; ++i) {
                output[outputOffset + i] += inverseBuffer[i];
            }
        }

        return Result::Success;
    }
    catch (const std::bad_alloc&) {
        output.clear();
        return Result::OutOfMemory;
    }
    catch (const std::length_error&) {
        output.clear();
        return Result::InvalidArgument;
    }
    catch (...) {
        output.clear();
        return Result::Error;
    }
}

} // namespace Grain
