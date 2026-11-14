
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>

#include "../../include/Grain.hpp"


int main() {

    constexpr int32_t channel_count = 2;
    constexpr int32_t sample_rate = 48000;
    constexpr int64_t duration_seconds = 5;

    constexpr int64_t sample_count =
        static_cast<int64_t>(sample_rate) * duration_seconds;

    constexpr float frequency = 440.0f;
    constexpr float amplitude = 0.5f;


    //--------------------------------------------------------------------------
    //  Create and configure the source signal
    //--------------------------------------------------------------------------

    Grain::Signal signal;

    const int32_t configure_error = signal.configure(
        channel_count,
        sample_rate,
        sample_count,
        Grain::SignalSampleType::Float32
    );

    if (configure_error != 0) {

        std::fprintf(
            stderr,
            "Failed to configure signal (error code: %d).\n",
            configure_error
        );

        return 1;
    }


    //--------------------------------------------------------------------------
    //  Generate a sine wave
    //--------------------------------------------------------------------------

    float* samples = static_cast<float*>(signal.mutDataPtr());

    if (samples == nullptr) {
        std::fprintf(stderr, "Signal has no data buffer.\n");
        return 1;
    }

    for (int64_t i = 0; i < sample_count; ++i) {

        const float time =
            static_cast<float>(i) / static_cast<float>(sample_rate);

        const float phase =
            2.0f * static_cast<float>(M_PI) * frequency * time;

        const float value = amplitude * std::sin(phase);

        // Interleaved stereo: left, right, left, right, ...

        samples[i * channel_count + 0] = value;
        samples[i * channel_count + 1] = value;
    }


    //--------------------------------------------------------------------------
    //  Save the signal as WAV
    //--------------------------------------------------------------------------

    Grain::AudioFileOptions wav_options;

    wav_options.format = Grain::AudioFileFormat::WAV;
    wav_options.overwrite = true;

    const Grain::AudioFileError wav_error =
        Grain::AudioFile::save(
            signal,
            "demo.wav",
            wav_options
        );

    if (wav_error != Grain::AudioFileError::None) {

        std::fprintf(
            stderr,
            "Failed to save WAV file (error code: %d).\n",
            static_cast<int>(wav_error)
        );

        return 1;
    }

    std::printf("Saved demo.wav.\n");


    //--------------------------------------------------------------------------
    //  Load the WAV file into another signal
    //--------------------------------------------------------------------------

    Grain::Signal decoded_signal;

    const Grain::AudioFileError load_error =
        Grain::AudioFile::load(
            decoded_signal,
            "demo.wav"
        );

    if (load_error != Grain::AudioFileError::None) {

        std::fprintf(
            stderr,
            "Failed to load WAV file (error code: %d).\n",
            static_cast<int>(load_error)
        );

        return 1;
    }

    std::printf(
        "Loaded demo.wav (%d channels, %d Hz, %lld samples).\n",
        decoded_signal.channelCount(),
        decoded_signal.sampleRate(),
        static_cast<long long>(decoded_signal.sampleCount())
    );


    //--------------------------------------------------------------------------
    //  Save the decoded signal as MP3
    //--------------------------------------------------------------------------

    Grain::AudioFileOptions mp3_options;

    mp3_options.format = Grain::AudioFileFormat::MP3;
    mp3_options.bit_rate = 192000;
    mp3_options.overwrite = true;

    const Grain::AudioFileError mp3_error =
        Grain::AudioFile::save(
            decoded_signal,
            "demo.mp3",
            mp3_options
        );

    if (mp3_error != Grain::AudioFileError::None) {

        std::fprintf(
            stderr,
            "Failed to save MP3 file (error code: %d).\n",
            static_cast<int>(mp3_error)
        );

        return 1;
    }

    std::printf("Saved demo.mp3.\n");

    return 0;
}
