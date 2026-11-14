#include <cstdint>
#include <cstdio>

#include "../../include/Grain.hpp"


struct MovieContext {
    int32_t width = 1200;
    int32_t height = 800;
};


bool renderFrame(
    Grain::MovieWriter* writer,
    int64_t frame_index,
    void* ref
) noexcept {

    if (writer == nullptr || ref == nullptr) {
        return false;
    }

    auto* context = static_cast<MovieContext*>(ref);

    Grain::Image* image = writer->videoFrameBufferPtr();

    if (image == nullptr) {
        return false;
    }

    Grain::ImageAccess access;

    if (!access.setup(*image)) {
        return false;
    }

    const float phase =
        static_cast<float>(frame_index) * 0.04f;

    for (int32_t y = 0; y < context->height; ++y) {

        if (!access.beginRow(y)) {
            return false;
        }

        do {
            if (!access.read()) {
                return false;
            }

            float* values = access.transferPtr();

            const float x = static_cast<float>(access.x());
            const float y_pos = static_cast<float>(y);

            // Animated colour gradient.
            values[0] = x / static_cast<float>(context->width);

            values[1] = y_pos / static_cast<float>(context->height);

            values[2] = 0.5f + 0.5f * __builtin_sinf(
                phase + x * 0.005f
            );

            values[3] = 1.0f;

            if (!access.write()) {
                return false;
            }

        } while (access.nextX());
    }

    return true;
}


int main() {

    constexpr int32_t width = 1200;
    constexpr int32_t height = 800;

    constexpr int32_t frame_rate = 25;
    constexpr int64_t duration_seconds = 10;

    constexpr int64_t frame_count =frame_rate * duration_seconds;

    //--------------------------------------------------------------------------
    //  Create the audio signal
    //--------------------------------------------------------------------------

    Grain::Signal audio_signal;
    int32_t audio_channel_count = 2;
    int32_t audio_sample_rate = 44100;
    int32_t audio_sample_count = 10 * audio_sample_rate;
    float audio_sine_frequency = 440.0f;
    float audio_amplitude = 1.0f;

    const int32_t configure_error = audio_signal.configure(
        audio_channel_count,
        audio_sample_rate,
        audio_sample_count,
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

    float* samples = static_cast<float*>(audio_signal.mutDataPtr());

    if (samples == nullptr) {
        std::fprintf(stderr, "Signal has no data buffer.\n");
        return 1;
    }

    for (int64_t i = 0; i < audio_sample_count; ++i) {
        const float time = static_cast<float>(i) / static_cast<float>(audio_sample_rate);
        const float phase = 2.0f * static_cast<float>(M_PI) * audio_sine_frequency * time;
        const float value = audio_amplitude * std::sin(phase);

        // Interleaved stereo: left, right, left, right, ...
        samples[i * audio_channel_count + 0] = value;
        samples[i * audio_channel_count + 1] = value;
    }


    //--------------------------------------------------------------------------
    //  Configure the movie
    //--------------------------------------------------------------------------

    Grain::MovieWriterConfig config;

    config.width = width;
    config.height = height;

    config.frame_rate_num = frame_rate;
    config.frame_rate_den = 1;

    config.video_codec = Grain::MovieVideoCodec::H264;
    config.video_quality = 23;

    config.has_audio = true;
    config.audio_codec = Grain::MovieAudioCodec::AAC;
    config.audio_bitrate = 192000;


    //--------------------------------------------------------------------------
    //  Set up the frame callback context
    //--------------------------------------------------------------------------

    MovieContext context;

    context.width = width;
    context.height = height;


    //--------------------------------------------------------------------------
    //  Create the movie writer
    //--------------------------------------------------------------------------

    Grain::MovieWriter writer;

    const Grain::MovieWriterError error =
        writer.writeVideoWithAudio(
            Grain::String("gradient.mp4"),
            frame_count,
            config,
            renderFrame,
            &audio_signal,
            &context
        );

    if (error != Grain::MovieWriterError::None) {

        std::fprintf(
            stderr,
            "Failed to write movie (error code: %d).\n",
            static_cast<int>(error)
        );

        return 1;
    }

    std::printf(
        "Saved gradient.mp4 (%d x %d, %d fps, %lld seconds).\n",
        width,
        height,
        frame_rate,
        static_cast<long long>(duration_seconds)
    );

    return 0;
}