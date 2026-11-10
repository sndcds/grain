
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include <grain/Signal/FFmpeg/FFmpegWriter.hpp>


namespace Grain::Internal {

namespace {


//----------------------------------------------------------------------
// FFmpeg resource management.
//----------------------------------------------------------------------

struct FFmpegWriterContext {

    AVFormatContext* format = nullptr;
    AVCodecContext* codec = nullptr;
    AVStream* stream = nullptr;

    SwrContext* resampler = nullptr;
    AVAudioFifo* fifo = nullptr;

    AVPacket* packet = nullptr;

    int64_t next_pts = 0;

    ~FFmpegWriterContext() {

        if (fifo != nullptr) {
            av_audio_fifo_free(fifo);
        }

        if (resampler != nullptr) {
            swr_free(&resampler);
        }

        if (packet != nullptr) {
            av_packet_free(&packet);
        }

        if (codec != nullptr) {
            avcodec_free_context(&codec);
        }

        if (format != nullptr) {

            if (format->pb != nullptr &&
                !(format->oformat->flags & AVFMT_NOFILE)) {

                avio_closep(&format->pb);
            }

            avformat_free_context(format);
        }
    }
};


//----------------------------------------------------------------------
// Resolve the output container and codec.
//----------------------------------------------------------------------

struct AudioFormatInfo {

    const char* container = nullptr;
    AVCodecID codec_id = AV_CODEC_ID_NONE;
};


auto getAudioFormatInfo(
    AudioFileFormat format,
    const std::filesystem::path& path
) noexcept -> AudioFormatInfo {

    if (format == AudioFileFormat::Auto) {

        std::string extension = path.extension().string();

        std::transform(
            extension.begin(),
            extension.end(),
            extension.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            }
        );

        if (extension == ".wav") {
            format = AudioFileFormat::WAV;
        }
        else if (extension == ".aiff" || extension == ".aif") {
            format = AudioFileFormat::AIFF;
        }
        else if (extension == ".mp3") {
            format = AudioFileFormat::MP3;
        }
        else if (extension == ".aac") {
            format = AudioFileFormat::AAC;
        }
        else if (extension == ".m4a") {
            format = AudioFileFormat::M4A;
        }
        else if (extension == ".flac") {
            format = AudioFileFormat::FLAC;
        }
        else if (extension == ".ogg" || extension == ".oga") {
            format = AudioFileFormat::OGG;
        }
        else {
            return {};
        }
    }

    switch (format) {

        case AudioFileFormat::WAV:
            return {"wav", AV_CODEC_ID_PCM_S16LE};

        case AudioFileFormat::AIFF:
            return {"aiff", AV_CODEC_ID_PCM_S16BE};

        case AudioFileFormat::MP3:
            return {"mp3", AV_CODEC_ID_MP3};

        case AudioFileFormat::AAC:
            return {"adts", AV_CODEC_ID_AAC};

        case AudioFileFormat::M4A:
            return {"ipod", AV_CODEC_ID_AAC};

        case AudioFileFormat::FLAC:
            return {"flac", AV_CODEC_ID_FLAC};

        case AudioFileFormat::OGG:
            return {"ogg", AV_CODEC_ID_VORBIS};

        default:
            return {};
    }
}


//----------------------------------------------------------------------
// Select a sample format supported by the encoder.
//----------------------------------------------------------------------

auto selectSampleFormat(
    const AVCodec* encoder
) noexcept -> AVSampleFormat {

    if (encoder == nullptr) {
        return AV_SAMPLE_FMT_NONE;
    }

    const void* supported_formats = nullptr;

    const int result = avcodec_get_supported_config(
        nullptr,
        encoder,
        AV_CODEC_CONFIG_SAMPLE_FORMAT,
        0,
        &supported_formats,
        nullptr
    );

    if (result < 0) {
        return AV_SAMPLE_FMT_NONE;
    }

    // nullptr means that the encoder accepts any sample format.
    if (supported_formats == nullptr) {
        return AV_SAMPLE_FMT_FLTP;
    }

    const auto* formats =
        static_cast<const AVSampleFormat*>(supported_formats);

    constexpr AVSampleFormat preferred_formats[] = {
        AV_SAMPLE_FMT_FLTP,
        AV_SAMPLE_FMT_FLT,
        AV_SAMPLE_FMT_S16P,
        AV_SAMPLE_FMT_S16,
        AV_SAMPLE_FMT_S32P,
        AV_SAMPLE_FMT_S32
    };

    for (const AVSampleFormat preferred : preferred_formats) {

        for (
            const AVSampleFormat* current = formats;
            *current != AV_SAMPLE_FMT_NONE;
            ++current
        ) {
            if (*current == preferred) {
                return preferred;
            }
        }
    }

    // Fall back to the encoder's first supported format.
    return formats[0];
}

//----------------------------------------------------------------------
// Select a supported sample rate.
//----------------------------------------------------------------------

auto selectSampleRate(
    const AVCodec* encoder,
    int32_t requested_rate
) noexcept -> int32_t {

    if (encoder == nullptr || requested_rate <= 0) {
        return 0;
    }

    const void* supported_rates = nullptr;
    int rate_count = 0;

    const int result = avcodec_get_supported_config(
        nullptr,
        encoder,
        AV_CODEC_CONFIG_SAMPLE_RATE,
        0,
        &supported_rates,
        &rate_count
    );

    if (result < 0) {
        return 0;
    }

    // nullptr means all sample rates are supported.
    if (supported_rates == nullptr) {
        return requested_rate;
    }

    const auto* rates =
        static_cast<const int*>(supported_rates);

    if (rate_count <= 0) {
        return 0;
    }

    int32_t best_rate = rates[0];

    int64_t best_difference =
        std::numeric_limits<int64_t>::max();

    for (int i = 0; i < rate_count; ++i) {

        const int32_t rate = rates[i];

        const int64_t difference =
            static_cast<int64_t>(rate) -
            static_cast<int64_t>(requested_rate);

        const int64_t absolute_difference =
            difference < 0 ? -difference : difference;

        if (absolute_difference < best_difference) {
            best_difference = absolute_difference;
            best_rate = rate;
        }

        if (rate == requested_rate) {
            return requested_rate;
        }
    }

    return best_rate;
}


//----------------------------------------------------------------------
// Convert a Signal sample to normalized Float32.
//----------------------------------------------------------------------

auto readSignalSample(
    const Signal& signal,
    int64_t index
) noexcept -> float {

    switch (signal.sampleType()) {

        case SignalSampleType::Int8: {

            const auto* samples =
                static_cast<const int8_t*>(signal.dataPtr());

            return static_cast<float>(samples[index]) / 128.0f;
        }

        case SignalSampleType::Int16: {

            const auto* samples =
                static_cast<const int16_t*>(signal.dataPtr());

            return static_cast<float>(samples[index]) / 32768.0f;
        }

        case SignalSampleType::Int32: {

            const auto* samples =
                static_cast<const int32_t*>(signal.dataPtr());

            return static_cast<float>(
                static_cast<double>(samples[index]) / 2147483648.0
            );
        }

        case SignalSampleType::Float32: {

            const auto* samples =
                static_cast<const float*>(signal.dataPtr());

            return samples[index];
        }

        default:
            return 0.0f;
    }
}


//----------------------------------------------------------------------
// Write encoded packets to the output container.
//----------------------------------------------------------------------

auto writePackets(
    FFmpegWriterContext& context
) noexcept -> bool {

    while (true) {

        const int result = avcodec_receive_packet(
            context.codec,
            context.packet
        );

        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            return true;
        }

        if (result < 0) {
            return false;
        }

        av_packet_rescale_ts(
            context.packet,
            context.codec->time_base,
            context.stream->time_base
        );

        context.packet->stream_index = context.stream->index;

        const int write_result = av_interleaved_write_frame(
            context.format,
            context.packet
        );

        av_packet_unref(context.packet);

        if (write_result < 0) {
            return false;
        }
    }
}


//----------------------------------------------------------------------
// Encode one audio frame.
//----------------------------------------------------------------------

auto encodeFrame(
    FFmpegWriterContext& context,
    AVFrame* frame
) noexcept -> bool {

    if (frame != nullptr) {
        frame->pts = context.next_pts;
        context.next_pts += frame->nb_samples;
    }

    int result = avcodec_send_frame(
        context.codec,
        frame
    );

    if (result == AVERROR(EAGAIN)) {

        if (!writePackets(context)) {
            return false;
        }

        result = avcodec_send_frame(
            context.codec,
            frame
        );
    }

    if (result < 0) {
        return false;
    }

    return writePackets(context);
}


//----------------------------------------------------------------------
// Transfer converted samples into the encoder FIFO.
//----------------------------------------------------------------------

auto appendToFifo(
    FFmpegWriterContext& context,
    AVFrame* frame,
    int32_t sample_count
) noexcept -> bool {

    if (sample_count <= 0) {
        return true;
    }

    const int required_size =
        av_audio_fifo_size(context.fifo) + sample_count;

    if (av_audio_fifo_realloc(
        context.fifo,
        required_size
    ) < 0) {
        return false;
    }

    const int written = av_audio_fifo_write(
        context.fifo,
        reinterpret_cast<void**>(frame->extended_data),
        sample_count
    );

    return written == sample_count;
}


//----------------------------------------------------------------------
// Encode all complete frames currently in the FIFO.
//----------------------------------------------------------------------

auto drainFifo(
    FFmpegWriterContext& context,
    int32_t frame_size
) noexcept -> bool {

    while (av_audio_fifo_size(context.fifo) >= frame_size) {

        AVFrame* frame = av_frame_alloc();

        if (frame == nullptr) {
            return false;
        }

        frame->format = context.codec->sample_fmt;
        frame->sample_rate = context.codec->sample_rate;
        frame->nb_samples = frame_size;

        if (av_channel_layout_copy(
            &frame->ch_layout,
            &context.codec->ch_layout
        ) < 0) {

            av_frame_free(&frame);
            return false;
        }

        if (av_frame_get_buffer(frame, 0) < 0) {
            av_frame_free(&frame);
            return false;
        }

        if (av_frame_make_writable(frame) < 0) {
            av_frame_free(&frame);
            return false;
        }

        const int read_count = av_audio_fifo_read(
            context.fifo,
            reinterpret_cast<void**>(frame->extended_data),
            frame_size
        );

        if (read_count != frame_size) {
            av_frame_free(&frame);
            return false;
        }

        const bool success = encodeFrame(context, frame);

        av_frame_free(&frame);

        if (!success) {
            return false;
        }
    }

    return true;
}


//----------------------------------------------------------------------
// Convert Float32 input samples and append them to the FIFO.
//----------------------------------------------------------------------

auto convertSamples(
    FFmpegWriterContext& context,
    const float* input,
    int32_t input_sample_count,
    int32_t frame_size
) noexcept -> bool {

    const int output_capacity = swr_get_out_samples(
        context.resampler,
        input_sample_count
    );

    if (output_capacity < 0) {
        return false;
    }

    if (output_capacity == 0) {
        return true;
    }

    AVFrame* frame = av_frame_alloc();

    if (frame == nullptr) {
        return false;
    }

    frame->format = context.codec->sample_fmt;
    frame->sample_rate = context.codec->sample_rate;
    frame->nb_samples = output_capacity;

    if (av_channel_layout_copy(
        &frame->ch_layout,
        &context.codec->ch_layout
    ) < 0) {

        av_frame_free(&frame);
        return false;
    }

    if (av_frame_get_buffer(frame, 0) < 0) {
        av_frame_free(&frame);
        return false;
    }

    const std::uint8_t* input_data[] = {
        reinterpret_cast<const std::uint8_t*>(input)
    };

    const int converted_samples = swr_convert(
        context.resampler,
        frame->extended_data,
        output_capacity,
        input_data,
        input_sample_count
    );

    if (converted_samples < 0) {
        av_frame_free(&frame);
        return false;
    }

    const bool success =
        appendToFifo(context, frame, converted_samples) &&
        drainFifo(context, frame_size);

    av_frame_free(&frame);

    return success;
}


//----------------------------------------------------------------------
// Flush delayed samples from the resampler.
//----------------------------------------------------------------------

auto flushResampler(
    FFmpegWriterContext& context,
    int32_t frame_size
) noexcept -> bool {

    constexpr int32_t output_capacity = 4096;

    while (true) {

        AVFrame* frame = av_frame_alloc();

        if (frame == nullptr) {
            return false;
        }

        frame->format = context.codec->sample_fmt;
        frame->sample_rate = context.codec->sample_rate;
        frame->nb_samples = output_capacity;

        if (av_channel_layout_copy(
            &frame->ch_layout,
            &context.codec->ch_layout
        ) < 0) {

            av_frame_free(&frame);
            return false;
        }

        if (av_frame_get_buffer(frame, 0) < 0) {
            av_frame_free(&frame);
            return false;
        }

        const int converted_samples = swr_convert(
            context.resampler,
            frame->extended_data,
            output_capacity,
            nullptr,
            0
        );

        if (converted_samples < 0) {
            av_frame_free(&frame);
            return false;
        }

        const bool success =
            appendToFifo(context, frame, converted_samples);

        av_frame_free(&frame);

        if (!success) {
            return false;
        }

        if (converted_samples == 0) {
            break;
        }
    }

    return drainFifo(context, frame_size);
}


//----------------------------------------------------------------------
// Encode the remaining samples in the FIFO.
//----------------------------------------------------------------------

auto encodeRemainingSamples(
    FFmpegWriterContext& context,
    int32_t frame_size,
    bool fixed_frame_size
) noexcept -> bool {

    const int remaining = av_audio_fifo_size(context.fifo);

    if (remaining <= 0) {
        return true;
    }

    const int output_size =
        fixed_frame_size ? frame_size : remaining;

    AVFrame* frame = av_frame_alloc();

    if (frame == nullptr) {
        return false;
    }

    frame->format = context.codec->sample_fmt;
    frame->sample_rate = context.codec->sample_rate;
    frame->nb_samples = output_size;

    if (av_channel_layout_copy(
        &frame->ch_layout,
        &context.codec->ch_layout
    ) < 0) {

        av_frame_free(&frame);
        return false;
    }

    if (av_frame_get_buffer(frame, 0) < 0) {
        av_frame_free(&frame);
        return false;
    }

    if (av_frame_make_writable(frame) < 0) {
        av_frame_free(&frame);
        return false;
    }

    const int read_count = av_audio_fifo_read(
        context.fifo,
        reinterpret_cast<void**>(frame->extended_data),
        remaining
    );

    if (read_count != remaining) {
        av_frame_free(&frame);
        return false;
    }

    if (output_size > remaining) {

        if (av_samples_set_silence(
            frame->extended_data,
            remaining,
            output_size - remaining,
            context.codec->ch_layout.nb_channels,
            context.codec->sample_fmt
        ) < 0) {

            av_frame_free(&frame);
            return false;
        }
    }

    const bool success = encodeFrame(context, frame);

    av_frame_free(&frame);

    return success;
}


} // namespace


//----------------------------------------------------------------------
// Save a Signal to an audio file.
//----------------------------------------------------------------------

auto FFmpegWriter::save(
    const Signal& signal,
    const std::filesystem::path& path,
    const AudioFileOptions& options
) noexcept -> AudioFileError {

    try {

        //----------------------------------------------------------------------
        // Validate the input signal.
        //----------------------------------------------------------------------

        if (!signal.isConfigured() || !signal.hasData()) {
            return AudioFileError::InvalidAudioData;
        }

        if (path.empty()) {
            return AudioFileError::InvalidConfiguration;
        }

        const int32_t input_channels = signal.channelCount();
        const int32_t input_rate = signal.sampleRate();
        const int64_t sample_count = signal.sampleCount();

        if (input_channels <= 0 ||
            input_rate <= 0 ||
            sample_count <= 0 ||
            signal.dataPtr() == nullptr) {

            return AudioFileError::InvalidAudioData;
        }

        if (signal.sampleType() == SignalSampleType::Undefined) {
            return AudioFileError::UnsupportedSampleType;
        }

        if (input_channels >
            std::numeric_limits<int32_t>::max() / 2) {

            return AudioFileError::InvalidConfiguration;
        }

        //----------------------------------------------------------------------
        // Check whether the destination already exists.
        //----------------------------------------------------------------------

        std::error_code filesystem_error;

        const bool file_exists = std::filesystem::exists(
            path,
            filesystem_error
        );

        if (filesystem_error) {
            return AudioFileError::FileOpenFailed;
        }

        if (file_exists && !options.overwrite) {
            return AudioFileError::FileOpenFailed;
        }

        //----------------------------------------------------------------------
        // Resolve the output format.
        //----------------------------------------------------------------------

        const AudioFormatInfo format_info =
            getAudioFormatInfo(options.format, path);

        if (format_info.container == nullptr ||
            format_info.codec_id == AV_CODEC_ID_NONE) {

            return AudioFileError::UnsupportedFormat;
        }

        //----------------------------------------------------------------------
        // Allocate FFmpeg resources.
        //----------------------------------------------------------------------

        FFmpegWriterContext context;

        const std::string filename = path.string();

        if (avformat_alloc_output_context2(
            &context.format,
            nullptr,
            format_info.container,
            filename.c_str()
        ) < 0 || context.format == nullptr) {

            return AudioFileError::UnsupportedFormat;
        }

        //----------------------------------------------------------------------
        // Find the encoder.
        //----------------------------------------------------------------------

        const AVCodec* encoder = avcodec_find_encoder(
            format_info.codec_id
        );

        if (encoder == nullptr) {
            return AudioFileError::UnsupportedCodec;
        }

        context.stream = avformat_new_stream(
            context.format,
            nullptr
        );

        if (context.stream == nullptr) {
            return AudioFileError::AllocationFailed;
        }

        context.codec = avcodec_alloc_context3(encoder);

        if (context.codec == nullptr) {
            return AudioFileError::AllocationFailed;
        }

        //----------------------------------------------------------------------
        // Configure the encoder.
        //----------------------------------------------------------------------

        const int32_t requested_channels =
            options.channel_count > 0
                ? options.channel_count
                : input_channels;

        const int32_t requested_rate =
            options.sample_rate > 0
                ? options.sample_rate
                : input_rate;

        if (requested_channels <= 0 ||
            requested_channels > 8 ||
            requested_rate <= 0) {

            return AudioFileError::InvalidConfiguration;
        }

        context.codec->sample_rate = selectSampleRate(
            encoder,
            requested_rate
        );

        context.codec->sample_fmt = selectSampleFormat(encoder);

        context.codec->time_base = AVRational{
            1,
            context.codec->sample_rate
        };

        context.codec->bit_rate =
            options.bit_rate > 0
                ? options.bit_rate
                : 192000;

        av_channel_layout_default(
            &context.codec->ch_layout,
            requested_channels
        );

        if (context.format->oformat->flags & AVFMT_GLOBALHEADER) {
            context.codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (avcodec_open2(
            context.codec,
            encoder,
            nullptr
        ) < 0) {

            return AudioFileError::EncodingFailed;
        }

        if (avcodec_parameters_from_context(
            context.stream->codecpar,
            context.codec
        ) < 0) {

            return AudioFileError::EncodingFailed;
        }

        context.stream->time_base = context.codec->time_base;

        //----------------------------------------------------------------------
        // Open the output file.
        //----------------------------------------------------------------------

        if (!(context.format->oformat->flags & AVFMT_NOFILE)) {

            if (avio_open(
                &context.format->pb,
                filename.c_str(),
                AVIO_FLAG_WRITE
            ) < 0) {

                return AudioFileError::FileOpenFailed;
            }
        }

        if (avformat_write_header(
            context.format,
            nullptr
        ) < 0) {

            return AudioFileError::WriteFailed;
        }

        //----------------------------------------------------------------------
        // Configure the sample converter.
        //----------------------------------------------------------------------

        AVChannelLayout input_layout{};

        av_channel_layout_default(
            &input_layout,
            input_channels
        );

        const int resampler_result = swr_alloc_set_opts2(
            &context.resampler,
            &context.codec->ch_layout,
            context.codec->sample_fmt,
            context.codec->sample_rate,
            &input_layout,
            AV_SAMPLE_FMT_FLT,
            input_rate,
            0,
            nullptr
        );

        av_channel_layout_uninit(&input_layout);

        if (resampler_result < 0 ||
            context.resampler == nullptr ||
            swr_init(context.resampler) < 0) {

            return AudioFileError::InvalidConfiguration;
        }

        //----------------------------------------------------------------------
        // Create the encoder FIFO.
        //----------------------------------------------------------------------

        context.fifo = av_audio_fifo_alloc(
            context.codec->sample_fmt,
            context.codec->ch_layout.nb_channels,
            1
        );

        if (context.fifo == nullptr) {
            return AudioFileError::AllocationFailed;
        }

        context.packet = av_packet_alloc();

        if (context.packet == nullptr) {
            return AudioFileError::AllocationFailed;
        }

        const int32_t frame_size =
            context.codec->frame_size > 0
                ? context.codec->frame_size
                : 1024;

        const bool fixed_frame_size =
            (encoder->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE) == 0;

        //----------------------------------------------------------------------
        // Convert and encode the input signal.
        //----------------------------------------------------------------------

        constexpr int32_t chunk_size = 4096;

        std::vector<float> input_buffer(
            static_cast<std::size_t>(chunk_size) *
            static_cast<std::size_t>(input_channels)
        );

        int64_t position = 0;

        while (position < sample_count) {

            const int32_t current_count =
                static_cast<int32_t>(
                    std::min<int64_t>(
                        chunk_size,
                        sample_count - position
                    )
                );

            for (int32_t i = 0; i < current_count; ++i) {

                const int64_t source_position =
                    (position + i) * input_channels;

                for (int32_t channel = 0;
                     channel < input_channels;
                     ++channel) {

                    const int64_t source_index =
                        source_position + channel;

                    input_buffer[
                        static_cast<std::size_t>(i) *
                        static_cast<std::size_t>(input_channels) +
                        static_cast<std::size_t>(channel)
                    ] = readSignalSample(
                        signal,
                        source_index
                    );
                }
            }

            if (!convertSamples(
                context,
                input_buffer.data(),
                current_count,
                frame_size
            )) {

                return AudioFileError::EncodingFailed;
            }

            position += current_count;
        }

        //----------------------------------------------------------------------
        // Flush the resampler.
        //----------------------------------------------------------------------

        if (!flushResampler(context, frame_size)) {
            return AudioFileError::EncodingFailed;
        }

        //----------------------------------------------------------------------
        // Encode the remaining samples.
        //----------------------------------------------------------------------

        if (!encodeRemainingSamples(
            context,
            frame_size,
            fixed_frame_size
        )) {

            return AudioFileError::EncodingFailed;
        }

        //----------------------------------------------------------------------
        // Flush the encoder.
        //----------------------------------------------------------------------

        if (avcodec_send_frame(
            context.codec,
            nullptr
        ) < 0) {

            return AudioFileError::EncodingFailed;
        }

        if (!writePackets(context)) {
            return AudioFileError::WriteFailed;
        }

        //----------------------------------------------------------------------
        // Finalize the output container.
        //----------------------------------------------------------------------

        if (av_write_trailer(context.format) < 0) {
            return AudioFileError::WriteFailed;
        }

        return AudioFileError::None;
    }
    catch (...) {

        return AudioFileError::AllocationFailed;
    }
}


} // namespace Grain::Internal
