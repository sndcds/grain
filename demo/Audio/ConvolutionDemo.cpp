
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <vector>

#include <Grain.hpp>


int main(int argc, char* argv[]) {

    if (argc != 4) {
        std::fprintf(
            stderr,
            "Usage: convolution_demo <input-audio> <impulse-response> <output.wav>\n"
        );
        return 1;
    }

    const std::filesystem::path input_path = argv[1];
    const std::filesystem::path impulse_path = argv[2];
    const std::filesystem::path output_path = argv[3];


    // Load input audio.

    Grain::Signal input;

    const Grain::AudioFileError input_error =
        Grain::AudioFile::load(input, input_path);

    if (input_error != Grain::AudioFileError::None) {
        std::fprintf(stderr, "Failed to load input audio: %d\n",
                     static_cast<int>(input_error));
        return 1;
    }


    // Load the impulse response as mono audio at the input sample rate.

    Grain::Signal impulse;

    Grain::AudioFileOptions impulse_options;
    impulse_options.channel_count = 1;
    impulse_options.sample_rate = input.sampleRate();

    const Grain::AudioFileError impulse_error =
        Grain::AudioFile::load(
            impulse,
            impulse_path,
            impulse_options
        );

    if (impulse_error != Grain::AudioFileError::None) {
        std::fprintf(stderr, "Failed to load impulse response: %d\n",
                     static_cast<int>(impulse_error));
        return 1;
    }


    // Calculate the full convolution output length.

    const int64_t input_count = input.sampleCount();
    const int64_t impulse_count = impulse.sampleCount();

    if (input_count <= 0 || impulse_count <= 0 ||
        impulse_count - 1 >
            std::numeric_limits<int64_t>::max() - input_count) {
        std::fprintf(stderr, "Invalid or excessively large audio data.\n");
        return 1;
    }

    const int64_t output_count =
        input_count + impulse_count - 1;


    // Configure the output signal.

    Grain::Signal output;

    const int32_t configure_error = output.configure(
        input.channelCount(),
        input.sampleRate(),
        output_count,
        Grain::SignalSampleType::Float32
    );

    if (configure_error != 0) {
        std::fprintf(stderr, "Failed to configure output signal: %d\n",
                     configure_error);
        return 1;
    }


    // Extract the impulse response into a contiguous mono buffer.

    std::vector<float> impulse_samples(
        static_cast<std::size_t>(impulse_count)
    );

    for (int64_t i = 0; i < impulse_count; ++i) {
        impulse_samples[static_cast<std::size_t>(i)] =
            impulse.readFloat(0, i);
    }


    // Convolve each input channel independently.

    std::vector<float> input_samples(
        static_cast<std::size_t>(input_count)
    );

    std::vector<float> convolution_result;

    Grain::ConvolutionConfiguration configuration;
    configuration.partitionSize = 1024;

    for (int32_t channel = 0;
         channel < input.channelCount();
         ++channel) {

        for (int64_t i = 0; i < input_count; ++i) {
            input_samples[static_cast<std::size_t>(i)] =
                input.readFloat(channel, i);
        }

        const Grain::Result result = Grain::Convolution::process(
            input_samples,
            impulse_samples,
            convolution_result,
            configuration
        );

        if (result != Grain::Result::Success) {
            std::fprintf(stderr, "Convolution failed: %d\n",
                         static_cast<int>(result));
            return 1;
        }

        for (int64_t i = 0; i < output_count; ++i) {
            output.writeFloat(
                channel,
                i,
                convolution_result[static_cast<std::size_t>(i)]
            );
        }
    }


    // Prevent clipping when saving as 16-bit PCM WAV.

    float peak = 0.0f;

    for (int64_t i = 0; i < output_count; ++i) {
        for (int32_t channel = 0;
             channel < output.channelCount();
             ++channel) {

            peak = std::max(
                peak,
                std::abs(output.readFloat(channel, i))
            );
        }
    }

    if (peak > 0.99f) {
        const float gain = 0.99f / peak;

        for (int64_t i = 0; i < output_count; ++i) {
            for (int32_t channel = 0;
                 channel < output.channelCount();
                 ++channel) {

                output.writeFloat(
                    channel,
                    i,
                    output.readFloat(channel, i) * gain
                );
            }
        }

        std::printf("Output attenuated to prevent clipping.\n");
    }


    // Save the result.

    Grain::AudioFileOptions output_options;
    output_options.format = Grain::AudioFileFormat::WAV;
    output_options.overwrite = true;

    const Grain::AudioFileError save_error =
        Grain::AudioFile::save(
            output,
            output_path,
            output_options
        );

    if (save_error != Grain::AudioFileError::None) {
        std::fprintf(stderr, "Failed to save output: %d\n",
                     static_cast<int>(save_error));
        return 1;
    }

    std::printf(
        "Convolution completed:\n"
        "  Sample rate: %d Hz\n"
        "  Channels:    %d\n"
        "  Input:       %lld samples\n"
        "  IR:          %lld samples\n"
        "  Output:      %lld samples\n",
        output.sampleRate(),
        output.channelCount(),
        static_cast<long long>(input_count),
        static_cast<long long>(impulse_count),
        static_cast<long long>(output_count)
    );

    return 0;
}
