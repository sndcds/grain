#include <grain/Signal/FFmpeg/FFmpegReader.hpp>

#include <algorithm>
#include <cstring>
#include <limits>
#include <new>
#include <string>
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libavutil/mem.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}


namespace Grain::Internal {

namespace {

struct FFmpegReaderContext {

    AVFormatContext* format = nullptr;
    AVCodecContext* codec = nullptr;
    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;
    SwrContext* resampler = nullptr;

    ~FFmpegReaderContext() {

        swr_free(&resampler);
        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codec);
        avformat_close_input(&format);
    }
};


[[nodiscard]]
auto mapOpenError(const int error) noexcept -> AudioFileError {

    if (error == AVERROR(ENOENT)) {
        return AudioFileError::FileNotFound;
    }

    return AudioFileError::FileOpenFailed;
}


[[nodiscard]]
auto appendConvertedSamples(
    SwrContext* resampler,
    AVFrame* frame,
    std::vector<float>& samples,
    int32_t channel_count
) noexcept -> AudioFileError {

    const auto output_capacity = swr_get_out_samples(
        resampler,
        frame ? frame->nb_samples : 0
    );

    if (output_capacity < 0) {
        return AudioFileError::DecodingFailed;
    }

    if (output_capacity == 0) {
        return AudioFileError::None;
    }

    const auto channels = static_cast<std::size_t>(channel_count);

    if (static_cast<std::size_t>(output_capacity) >
        (std::numeric_limits<std::size_t>::max() - samples.size()) / channels) {
        return AudioFileError::AllocationFailed;
    }

    const auto old_size = samples.size();

    try {
        samples.resize(
            old_size +
            static_cast<std::size_t>(output_capacity) * channels
        );
    }
    catch (const std::bad_alloc&) {
        return AudioFileError::AllocationFailed;
    }

    std::uint8_t* output[] = {
        reinterpret_cast<std::uint8_t*>(samples.data() + old_size)
    };

    const auto converted = swr_convert(
        resampler,
        output,
        output_capacity,
        frame ? const_cast<const std::uint8_t**>(frame->extended_data) : nullptr,
        frame ? frame->nb_samples : 0
    );

    if (converted < 0) {
        samples.resize(old_size);
        return AudioFileError::DecodingFailed;
    }

    samples.resize(
        old_size +
        static_cast<std::size_t>(converted) * channels
    );

    return AudioFileError::None;
}


[[nodiscard]]
auto receiveFrames(
    AVCodecContext* codec,
    AVFrame* frame,
    SwrContext* resampler,
    std::vector<float>& samples,
    int32_t channel_count
) noexcept -> AudioFileError {

    while (true) {

        const auto result = avcodec_receive_frame(codec, frame);

        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            return AudioFileError::None;
        }

        if (result < 0) {
            return AudioFileError::DecodingFailed;
        }

        const auto error = appendConvertedSamples(
            resampler,
            frame,
            samples,
            channel_count
        );

        av_frame_unref(frame);

        if (error != AudioFileError::None) {
            return error;
        }
    }
}

} // namespace


auto FFmpegReader::load(
    Signal& signal,
    const std::filesystem::path& path,
    const AudioFileOptions& options
) noexcept -> AudioFileError {

    if (path.empty()) {
        return AudioFileError::InvalidConfiguration;
    }

    if (options.sample_rate < 0 || options.channel_count < 0) {
        return AudioFileError::InvalidConfiguration;
    }

    try {

        FFmpegReaderContext context;

        const auto filename = path.string();

        auto result = avformat_open_input(
            &context.format,
            filename.c_str(),
            nullptr,
            nullptr
        );

        if (result < 0) {
            return mapOpenError(result);
        }

        result = avformat_find_stream_info(context.format, nullptr);

        if (result < 0) {
            return AudioFileError::InvalidAudioData;
        }

        const auto stream_index = av_find_best_stream(
            context.format,
            AVMEDIA_TYPE_AUDIO,
            -1,
            -1,
            nullptr,
            0
        );

        if (stream_index < 0) {
            return AudioFileError::UnsupportedFormat;
        }

        const auto* stream = context.format->streams[stream_index];

        const auto* decoder = avcodec_find_decoder(
            stream->codecpar->codec_id
        );

        if (!decoder) {
            return AudioFileError::UnsupportedCodec;
        }

        context.codec = avcodec_alloc_context3(decoder);

        if (!context.codec) {
            return AudioFileError::AllocationFailed;
        }

        result = avcodec_parameters_to_context(
            context.codec,
            stream->codecpar
        );

        if (result < 0) {
            return AudioFileError::InvalidAudioData;
        }

        result = avcodec_open2(context.codec, decoder, nullptr);

        if (result < 0) {
            return AudioFileError::UnsupportedCodec;
        }

        auto input_channels = context.codec->ch_layout.nb_channels;

        if (input_channels <= 0) {
            return AudioFileError::InvalidAudioData;
        }

        auto output_channels = options.channel_count > 0
            ? options.channel_count
            : input_channels;

        auto output_rate = options.sample_rate > 0
            ? options.sample_rate
            : context.codec->sample_rate;

        if (output_rate <= 0 || output_channels <= 0) {
            return AudioFileError::InvalidAudioData;
        }

        AVChannelLayout input_layout{};

        if (context.codec->ch_layout.nb_channels > 0) {
            result = av_channel_layout_copy(
                &input_layout,
                &context.codec->ch_layout
            );
        }
        else {
            av_channel_layout_default(
                &input_layout,
                input_channels
            );
            result = 0;
        }

        if (result < 0) {
            return AudioFileError::InvalidAudioData;
        }

        AVChannelLayout output_layout{};

        av_channel_layout_default(
            &output_layout,
            output_channels
        );

        result = swr_alloc_set_opts2(
            &context.resampler,
            &output_layout,
            AV_SAMPLE_FMT_FLT,
            output_rate,
            &input_layout,
            context.codec->sample_fmt,
            context.codec->sample_rate,
            0,
            nullptr
        );

        av_channel_layout_uninit(&input_layout);
        av_channel_layout_uninit(&output_layout);

        if (result < 0 || !context.resampler) {
            return AudioFileError::AllocationFailed;
        }

        result = swr_init(context.resampler);

        if (result < 0) {
            return AudioFileError::DecodingFailed;
        }

        context.packet = av_packet_alloc();
        context.frame = av_frame_alloc();

        if (!context.packet || !context.frame) {
            return AudioFileError::AllocationFailed;
        }

        std::vector<float> samples;

        while (av_read_frame(context.format, context.packet) >= 0) {

            if (context.packet->stream_index == stream_index) {

                result = avcodec_send_packet(
                    context.codec,
                    context.packet
                );

                av_packet_unref(context.packet);

                if (result < 0 && result != AVERROR(EAGAIN)) {
                    return AudioFileError::DecodingFailed;
                }

                const auto error = receiveFrames(
                    context.codec,
                    context.frame,
                    context.resampler,
                    samples,
                    output_channels
                );

                if (error != AudioFileError::None) {
                    return error;
                }
            }
            else {
                av_packet_unref(context.packet);
            }
        }

        // Flush the decoder.
        result = avcodec_send_packet(context.codec, nullptr);

        if (result < 0 && result != AVERROR_EOF) {
            return AudioFileError::DecodingFailed;
        }

        auto error = receiveFrames(
            context.codec,
            context.frame,
            context.resampler,
            samples,
            output_channels
        );

        if (error != AudioFileError::None) {
            return error;
        }

        // Flush samples buffered by the resampler.
        error = appendConvertedSamples(
            context.resampler,
            nullptr,
            samples,
            output_channels
        );

        if (error != AudioFileError::None) {
            return error;
        }

        const auto channels = static_cast<std::size_t>(output_channels);

        if (samples.empty() || samples.size() % channels != 0) {
            return AudioFileError::InvalidAudioData;
        }

        const auto sample_count = samples.size() / channels;

        if (sample_count >
            static_cast<std::size_t>(std::numeric_limits<int64_t>::max())) {
            return AudioFileError::AllocationFailed;
        }

        Signal temporary;

        const auto configure_result = temporary.configure(
            output_channels,
            output_rate,
            static_cast<int64_t>(sample_count),
            SignalSampleType::Float32
        );

        if (configure_result != SignalData::kErrNone) {
            return configure_result == SignalData::kErrAllocationFailed
                ? AudioFileError::AllocationFailed
                : AudioFileError::InvalidConfiguration;
        }

        std::memcpy(
            temporary.mutDataPtr(),
            samples.data(),
            samples.size() * sizeof(float)
        );

        signal = std::move(temporary);

        return AudioFileError::None;
    }
    catch (const std::bad_alloc&) {
        return AudioFileError::AllocationFailed;
    }
    catch (...) {
        return AudioFileError::DecodingFailed;
    }
}

} // namespace Grain::Internal
