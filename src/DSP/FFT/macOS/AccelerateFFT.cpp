
#include "AccelerateFFT.hpp"

#include <Accelerate/Accelerate.h>

#include <limits>
#include <new>
#include <vector>

namespace Grain {

class AccelerateFFT final : public FFTBackend {
public:
    explicit AccelerateFFT(std::size_t size)
        : size_(size),
          bin_count_(size / 2 + 1),
          real_input_(size),
          imaginary_input_(size),
          real_output_(size),
          imaginary_output_(size) {

        forward_setup_ = vDSP_DFT_zrop_CreateSetup(
            nullptr,
            static_cast<vDSP_Length>(size_),
            vDSP_DFT_FORWARD
        );

        inverse_setup_ = vDSP_DFT_zrop_CreateSetup(
            nullptr,
            static_cast<vDSP_Length>(size_),
            vDSP_DFT_INVERSE
        );
    }

    ~AccelerateFFT() override {
        if (forward_setup_ != nullptr) {
            vDSP_DFT_DestroySetup(forward_setup_);
        }

        if (inverse_setup_ != nullptr) {
            vDSP_DFT_DestroySetup(inverse_setup_);
        }
    }

    [[nodiscard]]
    auto valid() const noexcept -> bool {
        return forward_setup_ != nullptr && inverse_setup_ != nullptr;
    }

    [[nodiscard]]
    auto size() const noexcept -> std::size_t override {
        return size_;
    }

    [[nodiscard]]
    auto binCount() const noexcept -> std::size_t override {
        return bin_count_;
    }

    [[nodiscard]]
    auto type() const noexcept -> FFTBackendType override {
        return FFTBackendType::Accelerate;
    }

    auto forward(
        std::span<const float> input,
        std::span<std::complex<float>> output
    ) noexcept -> Result override {

        if (input.size() != size_ || output.size() != bin_count_) {
            return Result::InvalidArgument;
        }

        const std::size_t half_size = size_ / 2;

        // vDSP expects even and odd samples in separate arrays.
        for (std::size_t i = 0; i < half_size; ++i) {
            real_input_[i] = input[i * 2];
            imaginary_input_[i] = input[i * 2 + 1];
        }

        vDSP_DFT_Execute(
            forward_setup_,
            real_input_.data(),
            imaginary_input_.data(),
            real_output_.data(),
            imaginary_output_.data()
        );

        // vDSP's real forward transform has a scale factor of 2.
        constexpr float scale = 0.5f;

        output[0] = {
            real_output_[0] * scale,
            0.0f
        };

        // DC and Nyquist are stored separately by vDSP.
        output[half_size] = {
            imaginary_output_[0] * scale,
            0.0f
        };

        for (std::size_t k = 1; k < half_size; ++k) {
            output[k] = {
                real_output_[k] * scale,
                imaginary_output_[k] * scale
            };
        }

        return Result::Success;
    }

    auto inverse(
        std::span<const std::complex<float>> input,
        std::span<float> output
    ) noexcept -> Result override {

        if (input.size() != bin_count_ || output.size() != size_) {
            return Result::InvalidArgument;
        }

        const std::size_t half_size = size_ / 2;

        // Convert Grain's spectrum into vDSP's packed representation.
        real_input_[0] = input[0].real();
        imaginary_input_[0] = input[half_size].real();

        for (std::size_t k = 1; k < half_size; ++k) {
            real_input_[k] = input[k].real();
            imaginary_input_[k] = input[k].imag();
        }

        vDSP_DFT_Execute(
            inverse_setup_,
            real_input_.data(),
            imaginary_input_.data(),
            real_output_.data(),
            imaginary_output_.data()
        );

        // vDSP's inverse transform is unnormalized.
        const float scale = 1.0f / static_cast<float>(size_);

        for (std::size_t i = 0; i < half_size; ++i) {
            output[i * 2] = real_output_[i] * scale;
            output[i * 2 + 1] = imaginary_output_[i] * scale;
        }

        return Result::Success;
    }

private:
    std::size_t size_ = 0;
    std::size_t bin_count_ = 0;

    vDSP_DFT_Setup forward_setup_ = nullptr;
    vDSP_DFT_Setup inverse_setup_ = nullptr;

    std::vector<float> real_input_;
    std::vector<float> imaginary_input_;
    std::vector<float> real_output_;
    std::vector<float> imaginary_output_;
};


auto createAccelerateFFT(
    std::size_t size,
    std::unique_ptr<FFTBackend>& output
) noexcept -> Result {

    output.reset();

    if (size < 2 || size % 2 != 0 ||
        size > std::numeric_limits<vDSP_Length>::max()) {
        return Result::Unsupported;
    }

    try {
        std::unique_ptr<AccelerateFFT> backend =
            std::make_unique<AccelerateFFT>(size);

        if (!backend->valid()) {
            return Result::Unsupported;
        }

        output = std::move(backend);

        return Result::Success;
    }
    catch (const std::bad_alloc&) {
        return Result::OutOfMemory;
    }
    catch (...) {
        return Result::Error;
    }
}

} // namespace Grain
