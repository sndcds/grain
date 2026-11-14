#include <cstdint>
#include <cstdio>

#include "../../include/Grain.hpp"


using namespace Grain;


struct MovieContext {
    int32_t width = 1200;
    int32_t height = 800;
    Font *font = nullptr;
};

#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Graphics/GraphicCompoundPath.hpp>
#include <grain/Graphics/GraphicPath.hpp>
#include <grain/Graphics/Gradient.hpp>

#include <cmath>
#include <numbers>


//--------------------------------------------------------------------------
//  Render text as a compound path
//--------------------------------------------------------------------------

void renderCompoundPathFromText(
    GraphicContext& gc,
    const Font& font,
    double x,
    double y,
    double time
) {
    //--------------------------------------------------------------------------
    //  Create compound path from text
    //--------------------------------------------------------------------------

    GraphicCompoundPath compound;

    const Rectd bounds = compound.buildFromText(
        font,
        "GRAIN"
    );

    //--------------------------------------------------------------------------
    //  Create animated gradient
    //--------------------------------------------------------------------------

    Gradient gradient;

    const double phase = time * 0.025;

    gradient.addStop(0.0, Color(1.0, 0.1 + 0.1 * std::sin(phase), 0.2, 1.0));
    gradient.addStop(0.35, {1.0, 0.65, 0.05, 0.2});
    gradient.addStop(0.7, {0.05, 0.55, 1.0, 1.0});
    gradient.addStop(1.0, {0.55, 0.1, 0.9, 0.2});

    //--------------------------------------------------------------------------
    //  Draw compound path
    //--------------------------------------------------------------------------

    gc.save();

    gc.translate(x, y);
    gc.scale(4, 4);
    gc.rotateDegrees(10);

    gc.beginPath();
    gc.addPath(compound);
    gc.clipPath();

    gc.fillGradient(
        &gradient,
        {bounds.x, bounds.y},
        {
            bounds.x + bounds.width,
            bounds.y + bounds.height
        },
        true,
        true
    );

    gc.restore();

    //--------------------------------------------------------------------------
    //  Draw outline
    //--------------------------------------------------------------------------
/*
    gc.save();

    gc.translate(x, y);

    gc.beginPath();
    gc.addPath(compound);

    gc.setStrokeColor({1.0, 1.0, 1.0, 0.9});
    gc.setStrokeWidth(2.0);
    gc.strokePath();

    gc.restore();
*/
}


//--------------------------------------------------------------------------
//  Render animated compound path
//--------------------------------------------------------------------------

void renderAnimatedCompoundPath(
    GraphicContext& gc,
    double centerX,
    double centerY,
    double time
) {

    constexpr double pi = std::numbers::pi;


    //----------------------------------------------------------------------
    //  Animation parameters
    //----------------------------------------------------------------------

    const double rotation = time * 0.012;
    const double pulse = 1.0 + 0.08 * std::sin(time * 0.045);
    const double wave = std::sin(time * 0.035);


    //----------------------------------------------------------------------
    //  Create compound path
    //----------------------------------------------------------------------

    GraphicCompoundPath compound;


    //----------------------------------------------------------------------
    //  1. Outer organic contour
    //----------------------------------------------------------------------

    compound.addEmptyPath(16);

    GraphicPath* outer = compound.lastPathPtr();

    if (outer == nullptr) {
        return;
    }

    constexpr int32_t outerPointCount = 12;

    const double outerRadius = 210.0 * pulse;

    for (int32_t i = 0; i < outerPointCount; ++i) {

        const double angle =
            rotation + static_cast<double>(i) * 2.0 * pi /
            static_cast<double>(outerPointCount);

        const double deformation =
            1.0 +
            0.12 * std::sin(angle * 3.0 + time * 0.035) +
            0.06 * std::cos(angle * 5.0 - time * 0.025);

        const double radius = outerRadius * deformation;

        const double x = centerX + std::cos(angle) * radius;
        const double y = centerY + std::sin(angle) * radius;

        const double handleLength = radius * 0.32;

        outer->addPointByAngleDegrees(
            {x, y},
            angle * 180.0 / pi + 90.0,
            handleLength,
            handleLength
        );
    }

    outer->setClosed(true);


    //----------------------------------------------------------------------
    //  2. First inner contour
    //----------------------------------------------------------------------

    compound.addEmptyPath(12);

    GraphicPath* inner = compound.lastPathPtr();

    if (inner == nullptr) {
        return;
    }

    constexpr int32_t innerPointCount = 9;

    const double innerRadius = 105.0 *
        (1.0 + 0.12 * std::sin(time * 0.06));

    const double innerRotation = -rotation * 1.8;

    for (int32_t i = 0; i < innerPointCount; ++i) {

        const double angle =
            innerRotation + static_cast<double>(i) * 2.0 * pi /
            static_cast<double>(innerPointCount);

        const double deformation =
            1.0 + 0.22 * std::sin(angle * 3.0 + time * 0.07);

        const double radius = innerRadius * deformation;

        const double x = centerX + std::cos(angle) * radius;
        const double y = centerY + std::sin(angle) * radius;

        inner->addPointByAngleDegrees(
            {x, y},
            angle * 180.0 / pi + 90.0,
            radius * 0.30,
            radius * 0.30
        );
    }

    inner->setClosed(true);


    //----------------------------------------------------------------------
    //  3. Second inner contour
    //----------------------------------------------------------------------

    compound.addEmptyPath(12);

    GraphicPath* center = compound.lastPathPtr();

    if (center == nullptr) {
        return;
    }

    constexpr int32_t centerPointCount = 8;

    const double centerRadius =
        38.0 + 12.0 * std::sin(time * 0.08);

    for (int32_t i = 0; i < centerPointCount; ++i) {

        const double angle =
            rotation * 2.5 +
            static_cast<double>(i) * 2.0 * pi /
            static_cast<double>(centerPointCount);

        const double radius = centerRadius *
            (1.0 + 0.18 * std::sin(angle * 2.0 + time * 0.06));

        const double x = centerX + std::cos(angle) * radius;
        const double y = centerY + std::sin(angle) * radius;

        center->addPointByAngleDegrees(
            {x, y},
            angle * 180.0 / pi + 90.0,
            radius * 0.35,
            radius * 0.35
        );
    }

    center->setClosed(true);


    //----------------------------------------------------------------------
    //  Gradient
    //----------------------------------------------------------------------

    Gradient gradient;

    gradient.addStop(0.0, {1.0, 0.05, 0.25, 1.0});
    gradient.addStop(0.3, {1.0, 0.65, 0.05, 1.0});
    gradient.addStop(0.6, {0.05, 0.55, 1.0, 1.0});
    gradient.addStop(1.0, {0.55, 0.1, 0.9, 1.0});


    //----------------------------------------------------------------------
    //  4. Fill compound path using even-odd clipping
    //----------------------------------------------------------------------

    gc.save();

    gc.beginPath();
    gc.addPath(compound);

    gc.clipPathEvenOdd();

    gc.fillGradient(
        &gradient,
        {centerX - 250.0, centerY - 250.0},
        {centerX + 250.0, centerY + 250.0},
        true,
        true
    );

    gc.restore();


    //----------------------------------------------------------------------
    //  5. Draw compound path outlines
    //----------------------------------------------------------------------

    gc.save();

    gc.beginPath();
    gc.addPath(compound);

    gc.setStrokeColor({1.0, 1.0, 1.0, 0.85});
    gc.setStrokeWidth(2.0);

    gc.strokePath();

    gc.restore();


    //----------------------------------------------------------------------
    //  6. Animated orbiting accents
    //----------------------------------------------------------------------

    gc.save();

    constexpr int32_t orbitCount = 8;

    const double orbitRadius = 255.0 + 15.0 * wave;

    gc.setFillColor({1.0, 1.0, 1.0, 0.85});

    for (int32_t i = 0; i < orbitCount; ++i) {

        const double angle =
            -rotation * 2.0 +
            static_cast<double>(i) * 2.0 * pi /
            static_cast<double>(orbitCount);

        const double x = centerX + std::cos(angle) * orbitRadius;
        const double y = centerY + std::sin(angle) * orbitRadius;

        const double radius =
            3.0 + 2.0 * (0.5 + 0.5 * std::sin(time * 0.08 + i));

        gc.fillCircle({x, y}, radius);
    }

    gc.restore();
}


bool renderFrame(
    MovieWriter* writer,
    int64_t frame_index,
    void* ref
) noexcept {

    if (writer == nullptr || ref == nullptr) {
        return false;
    }

    auto* mc = static_cast<MovieContext*>(ref);

    Image* image = writer->videoFrameBufferPtr();

    if (image == nullptr) {
        return false;
    }

    ImageAccess access;

    if (!access.setup(*image)) {
        return false;
    }

    const float phase =
        static_cast<float>(frame_index) * 0.04f;

    for (int32_t y = 0; y < mc->height; ++y) {

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
            values[0] = x / static_cast<float>(mc->width);

            values[1] = y_pos / static_cast<float>(mc->height);

            values[2] = 0.5f + 0.5f * __builtin_sinf(
                phase + x * 0.005f
            );

            values[3] = 1.0f;

            if (!access.write()) {
                return false;
            }

        } while (access.nextX());
    }


    //--------------------------------------------------------------------------
    //  Draw text
    //--------------------------------------------------------------------------

    auto gc = GraphicContext::createGraphicContext(*image);
    if (gc != nullptr) {
        gc->save();

        gc->setFillColor({1.0f, 1.0f, 0.0f, 1.0f});
        double x = 200 + frame_index * 4;
        gc->fillCircle({x, 200}, 100);

        gc->save();
        for (int i = 0; i < 10; ++i) {
            gc->setFillColor({0.0f, 0.0f, 1.0f, 0.1f});
            gc->fillRect({10, 10, 900, 600});
            gc->translate(20 + frame_index * 2, 0);
            gc->rotateDegrees(0.2 * frame_index);
        }

        if (mc->font == nullptr) {
            mc->font = new Font("SFPro-Regular", 80.0f);
        }

        gc->rotateDegrees(0.3 * frame_index);
        gc->drawText("Raytracing", {200, 200}, mc->font, {1.0f, 0.0f, 0.0f, 1.0f});

		Gradient gradient;
        gradient.addStop(0, {1, 0, 0, .2});
        gradient.addStop(.2, {1, 1, 0, .3});
        gradient.addStop(.5, {0, .5, 1, .6});
        gradient.addStop(1, {0, 1, .5, 1});

		Vec2d center(mc->width / 2, mc->height / 2);
        gc->addRingPath(center, mc->width / 4, mc->height / 4);
        // gc.setFillColor({1, 0, 0, 1});
        // gc.fillPathEvenOdd();
        gc->clipPathEvenOdd();
        gc->fillGradient(&gradient, {.0, .0}, {1000, 1000}, true, true);

        gc->restore();

        Rectd gradient_rect = {100, 100, 600, 600};
        gc->clip(gradient_rect);
        gc->fillRadialGradient(
            &gradient,
            gradient_rect.center(),
            10.0,
            gradient_rect.center(),
            300.0,
            true,
            true);
    }

    {
        //----------------------------------------------------------------------
        //  Shared animation values
        //----------------------------------------------------------------------

        const double time = static_cast<double>(frame_index);
        const double centerX = static_cast<double>(mc->width) * 0.5;
        const double centerY = static_cast<double>(mc->height) * 0.5;
        const double pulse = 1.0 + 0.08 * std::sin(time * 0.25);


        //----------------------------------------------------------------------
        //  Gradient
        //----------------------------------------------------------------------

        Gradient gradient;

        gradient.addStop(0.0, {1.0, 0.1, 0.2, 1.0});
        gradient.addStop(0.3, {1.0, 0.7, 0.1, 1.0});
        gradient.addStop(0.6, {0.1, 0.6, 1.0, 1.0});
        gradient.addStop(1.0, {0.5, 0.1, 0.9, 1.0});


        //----------------------------------------------------------------------
        //  1. Closed Bézier shape
        //----------------------------------------------------------------------

        GraphicPath blob(16);

        const double blobRadius = 115.0 * pulse;
        const double rotation = time * 0.012;

        for (int32_t i = 0; i < 6; ++i) {

            const double angle =
                rotation + static_cast<double>(i) * 2.0 * M_PI / 6.0;

            const double x = centerX + std::cos(angle) * blobRadius;
            const double y = centerY + std::sin(angle) * blobRadius;

            blob.addPointByAngleDegrees(
                {x, y},
                angle * 180.0 / M_PI + 90.0,
                55.0,
                55.0
            );
        }

        blob.setClosed(true);

        gc->save();

        gc->addPath(blob);
        gc->clipPath();

        gc->fillGradient(
            &gradient,
            {centerX - 150.0, centerY - 150.0},
            {centerX + 150.0, centerY + 150.0},
            true,
            true
        );

        gc->restore();
    }

    //--------------------------------------------------------------------------
    //  Animated GraphicCompoundPath
    //--------------------------------------------------------------------------

    if (gc != nullptr) {

        const double centerX = static_cast<double>(mc->width) * 0.5;
        const double centerY = static_cast<double>(mc->height) * 0.5;

        renderAnimatedCompoundPath(
            *gc,
            centerX,
            centerY,
            static_cast<double>(frame_index)
        );
    }


    //--------------------------------------------------------------------------
    //  Render text as compound path
    //--------------------------------------------------------------------------

    renderCompoundPathFromText(
        *gc,
        *mc->font,
        250.0,
        350.0,
        static_cast<double>(frame_index)
    );

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

    Signal audio_signal;
    int32_t audio_channel_count = 2;
    int32_t audio_sample_rate = 44100;
    int32_t audio_sample_count = 10 * audio_sample_rate;
    float audio_sine_frequency = 440.0f;
    float audio_amplitude = 1.0f;

    const int32_t configure_error = audio_signal.configure(
        audio_channel_count,
        audio_sample_rate,
        audio_sample_count,
        SignalSampleType::Float32
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

    MovieWriterConfig config;

    config.width = width;
    config.height = height;

    config.frame_rate_num = frame_rate;
    config.frame_rate_den = 1;

    config.video_codec = MovieVideoCodec::H264;
    config.video_quality = 23;

    config.has_audio = true;
    config.audio_codec = MovieAudioCodec::AAC;
    config.audio_bitrate = 192000;


    //--------------------------------------------------------------------------
    //  Set up the frame callback MovieContext
    //--------------------------------------------------------------------------

    MovieContext mc;

    mc.width = width;
    mc.height = height;


    //--------------------------------------------------------------------------
    //  Create the movie writer
    //--------------------------------------------------------------------------

    MovieWriter writer;

    const MovieWriterError error =
        writer.writeVideoWithAudio(
            String("gradient.mp4"),
            frame_count,
            config,
            renderFrame,
            &audio_signal,
            &mc
        );

    if (error != MovieWriterError::None) {

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