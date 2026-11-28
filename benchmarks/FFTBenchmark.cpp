
#include <grain/DSP/FFT/FFT.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numbers>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

constexpr std::array<std::size_t, 5> kTransformSizes = {
    64, 256, 1024, 4096, 16384
};

constexpr std::size_t kWarmupCount = 5;
constexpr std::size_t kSampleCount = 5;
constexpr auto kTargetSampleDuration = std::chrono::milliseconds(50);


struct Measurement {
    std::size_t iterations = 0;
    double microseconds = 0.0;
};


template <typename Function>
auto measureMedian(Function&& function, Measurement& measurement) -> bool {

    // Warm up the transform.
    for (std::size_t i = 0; i < kWarmupCount; ++i) {
        if (!function()) {
            return false;
        }
    }

    // Find an iteration count that gives us a sufficiently long sample.
    std::size_t iterations = 1;

    while (true) {
        const auto start = Clock::now();

        for (std::size_t i = 0; i < iterations; ++i) {
            if (!function()) {
                return false;
            }
        }

        const auto end = Clock::now();

        if (end - start >= kTargetSampleDuration) {
            break;
        }

        if (iterations > std::numeric_limits<std::size_t>::max() / 2) {
            return false;
        }

        iterations *= 2;
    }

    // Measure multiple samples using the calibrated iteration count.
    std::array<double, kSampleCount> samples{};

    for (std::size_t sample = 0; sample < kSampleCount; ++sample) {
        const auto start = Clock::now();

        for (std::size_t i = 0; i < iterations; ++i) {
            if (!function()) {
                return false;
            }
        }

        const auto end = Clock::now();

        const double elapsed =
            std::chrono::duration<double, std::micro>(end - start).count();

        samples[sample] = elapsed / static_cast<double>(iterations);
    }

    std::sort(samples.begin(), samples.end());

    measurement.iterations = iterations;
    measurement.microseconds = samples[kSampleCount / 2];

    return true;
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


auto isBackendAvailable(Grain::FFTBackendType backend) -> bool {
    std::unique_ptr<Grain::FFT> fft;

    return Grain::FFT::create(
        Grain::FFTConfiguration{64, backend},
        fft
    ) == Grain::Result::Success;
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

    Measurement forward_measurement;
    Measurement inverse_measurement;

    const bool forward_success = measureMedian(
        [&]() {
            return fft->forward(input, spectrum) == Result::Success;
        },
        forward_measurement
    );

    if (!forward_success) {
        std::cerr << "Forward benchmark failed.\n";
        return false;
    }

    const bool inverse_success = measureMedian(
        [&]() {
            return fft->inverse(spectrum, output) == Result::Success;
        },
        inverse_measurement
    );

    if (!inverse_success) {
        std::cerr << "Inverse benchmark failed.\n";
        return false;
    }

    std::cout
        << std::left
        << std::setw(13) << backendName(backend)
        << std::right
        << std::setw(10) << size
        << std::setw(12) << forward_measurement.iterations
        << std::setw(12) << inverse_measurement.iterations
        << std::setw(16) << std::fixed << std::setprecision(2)
        << forward_measurement.microseconds
        << std::setw(16) << inverse_measurement.microseconds
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
        << "Target sample duration: "
        << kTargetSampleDuration.count()
        << " ms\n"
        << "Samples per measurement: "
        << kSampleCount
        << "\n\n";

    std::cout
        << std::left
        << std::setw(13) << "Backend"
        << std::right
        << std::setw(10) << "Size"
        << std::setw(12) << "Fwd iters"
        << std::setw(12) << "Inv iters"
        << std::setw(16) << "Forward (us)"
        << std::setw(16) << "Inverse (us)"
        << '\n';

    std::cout << std::string(85, '-') << '\n';

    // Detect Accelerate availability only once.
    const bool accelerate_available =
        isBackendAvailable(FFTBackendType::Accelerate);

    bool success = true;

    for (const std::size_t size : kTransformSizes) {
        success = runBenchmark(FFTBackendType::Portable, size) && success;

        if (accelerate_available) {
            success = runBenchmark(FFTBackendType::Accelerate, size) && success;
        }
    }

    std::cout << '\n';

    return success ? 0 : 1;
}
