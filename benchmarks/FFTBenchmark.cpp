
#include <grain/DSP/FFT/FFT.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numbers>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

constexpr std::array<std::size_t, 5> kTransformSizes = {
    64, 256, 1024, 4096, 16384
};

constexpr std::size_t kWorkBudget = 2'000'000;
constexpr std::size_t kSampleCount = 3;


auto getIterationCount(std::size_t size) -> std::size_t {
    const std::size_t bin_count = size / 2 + 1;
    const std::size_t work = size * bin_count;

    return std::max<std::size_t>(1, kWorkBudget / work);
}


template <typename Function>
auto measureMedian(
    std::size_t iterations,
    Function&& function
) -> double {
    std::array<double, kSampleCount> samples{};

    for (std::size_t sample = 0; sample < kSampleCount; ++sample) {
        const auto start = Clock::now();

        for (std::size_t iteration = 0; iteration < iterations; ++iteration) {
            function();
        }

        const auto end = Clock::now();

        const double elapsed =
            std::chrono::duration<double, std::micro>(end - start).count();

        samples[sample] = elapsed / static_cast<double>(iterations);
    }

    std::sort(samples.begin(), samples.end());

    return samples[kSampleCount / 2];
}


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


auto runBenchmark(
    Grain::FFTBackendType backend,
    std::size_t size
) -> bool {
    using namespace Grain;

    std::unique_ptr<FFT> fft;

    const Result create_result = FFT::create(
        FFTConfiguration{size, backend},
        fft
    );

    if (create_result != Result::Success) {
        std::cerr
            << "Could not create "
            << backendName(backend)
            << " FFT for size "
            << size
            << ". Result: "
            << static_cast<int>(create_result)
            << '\n';

        return false;
    }

    std::vector<float> input(size);
    std::vector<float> output(size);

    std::vector<std::complex<float>> spectrum(fft->binCount());

    for (std::size_t i = 0; i < size; ++i) {
        const double phase =
            2.0 * std::numbers::pi_v<double>
            * 8.0
            * static_cast<double>(i)
            / static_cast<double>(size);

        input[i] = static_cast<float>(
            std::sin(phase)
            + 0.25 * std::sin(phase * 3.0)
            + static_cast<double>(i % 7) * 0.01
        );
    }

    // Prepare the spectrum used by the inverse benchmark.
    if (fft->forward(input, spectrum) != Result::Success) {
        std::cerr << "Initial forward transform failed.\n";
        return false;
    }

    const std::size_t iterations = getIterationCount(size);

    const double forward_time = measureMedian(
        iterations,
        [&]() {
            fft->forward(input, spectrum);
        }
    );

    const double inverse_time = measureMedian(
        iterations,
        [&]() {
            fft->inverse(spectrum, output);
        }
    );

    std::cout
        << std::left
        << std::setw(13) << backendName(backend)
        << std::right
        << std::setw(10) << size
        << std::setw(12) << iterations
        << std::setw(18) << std::fixed << std::setprecision(2)
        << forward_time
        << std::setw(18) << inverse_time
        << '\n';

    // Consume output values so the results are observable.
    volatile float checksum = output[size / 2];

    (void)checksum;

    return true;
}


} // namespace


auto main() -> int {
    using namespace Grain;

    std::cout
        << "\nGrain FFT Benchmarks\n"
        << "====================\n\n";

    std::cout
        << std::left
        << std::setw(13) << "Backend"
        << std::right
        << std::setw(10) << "Size"
        << std::setw(12) << "Iterations"
        << std::setw(18) << "Forward (us)"
        << std::setw(18) << "Inverse (us)"
        << '\n';

    std::cout << std::string(71, '-') << '\n';

    bool success = true;

    for (const std::size_t size : kTransformSizes) {
        success = runBenchmark(FFTBackendType::Portable, size) && success;

        // Accelerate is only available on macOS.
        const Result accelerate_result = []() {
            std::unique_ptr<FFT> fft;

            return FFT::create(
                FFTConfiguration{64, FFTBackendType::Accelerate},
                fft
            );
        }();

        if (accelerate_result == Result::Success) {
            success = runBenchmark(FFTBackendType::Accelerate, size) && success;
        }
    }

    std::cout << '\n';

    return success ? 0 : 1;
}
