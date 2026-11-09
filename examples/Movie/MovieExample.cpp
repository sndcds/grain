#include <cstdint>
#include <cstdio>

#include <grain/String/String.hpp>

#include <grain/Movie/Movie.hpp>
#include <grain/Movie/MovieWriter.hpp>

#include <grain/Image/Image.hpp>
#include <grain/Image/ImageAccess.hpp>


struct MovieContext {
    std::int32_t width = 1200;
    std::int32_t height = 800;
};


bool renderFrame(
    Grain::MovieWriter* writer,
    std::int64_t frame_index,
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

    for (std::int32_t y = 0; y < context->height; ++y) {

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

    constexpr std::int32_t width = 1200;
    constexpr std::int32_t height = 800;

    constexpr std::int32_t frame_rate = 25;
    constexpr std::int64_t duration_seconds = 10;

    constexpr std::int64_t frame_count =
        frame_rate * duration_seconds;


    //----------------------------------------------------------------------
    // Configure the movie.
    //----------------------------------------------------------------------

    Grain::MovieWriterConfig config;

    config.width = width;
    config.height = height;

    config.frame_rate_num = frame_rate;
    config.frame_rate_den = 1;

    config.video_codec = Grain::MovieVideoCodec::H264;
    config.video_quality = 23;

    config.has_audio = false;


    //----------------------------------------------------------------------
    // Set up the frame callback context.
    //----------------------------------------------------------------------

    MovieContext context;

    context.width = width;
    context.height = height;


    //----------------------------------------------------------------------
    // Create the movie writer.
    //----------------------------------------------------------------------

    Grain::MovieWriter writer;

    const Grain::MovieWriterError error =
        writer.writeVideoWithAudio(
            Grain::String("gradient.mp4"),
            frame_count,
            config,
            renderFrame,
            nullptr,
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