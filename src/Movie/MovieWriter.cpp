#include <grain/Movie/MovieWriter.hpp>

#include <grain/String/String.hpp>
#include <grain/Image/Image.hpp>
#include <grain/Image/ImageAccess.hpp>
// #include <grain/Signal/Signal.hpp>

#include <algorithm>
#include <string>
#include <utility>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libswscale/swscale.h>
}


namespace Grain {


namespace {

struct MovieWriterResources {

    AVFormatContext* format_context = nullptr;

    AVCodecContext* video_context = nullptr;
    AVCodecContext* audio_context = nullptr;

    AVFrame* video_frame = nullptr;
    AVFrame* intermediate_frame = nullptr;
    AVFrame* rgb_frame = nullptr;
    AVFrame* audio_frame = nullptr;

    AVPacket* packet = nullptr;

    SwsContext* rgb_to_yuv = nullptr;
    SwsContext* intermediate_to_yuv = nullptr;

    AVDictionary* options = nullptr;

    bool io_open = false;

    ~MovieWriterResources() {

        sws_freeContext(rgb_to_yuv);
        sws_freeContext(intermediate_to_yuv);

        av_frame_free(&rgb_frame);
        av_frame_free(&video_frame);
        av_frame_free(&intermediate_frame);
        av_frame_free(&audio_frame);

        av_packet_free(&packet);

        av_dict_free(&options);

        avcodec_free_context(&video_context);
        avcodec_free_context(&audio_context);

        if (format_context != nullptr) {
            if (io_open) {
                avio_closep(&format_context->pb);
            }

            avformat_free_context(format_context);
        }
    }
};


static MovieWriterError writePackets(
    AVCodecContext* codec_context,
    AVStream* stream,
    AVFormatContext* format_context,
    AVPacket* packet
) noexcept {

    while (true) {

        const int result = avcodec_receive_packet(
            codec_context,
            packet
        );

        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            return MovieWriterError::None;
        }

        if (result < 0) {
            return MovieWriterError::EncoderError;
        }

        av_packet_rescale_ts(
            packet,
            codec_context->time_base,
            stream->time_base
        );

        packet->stream_index = stream->index;

        const int write_result = av_interleaved_write_frame(
            format_context,
            packet
        );

        av_packet_unref(packet);

        if (write_result < 0) {
            return MovieWriterError::WriteError;
        }
    }
}


static MovieWriterError encodeFrame(
    AVCodecContext* codec_context,
    AVStream* stream,
    AVFormatContext* format_context,
    AVPacket* packet,
    AVFrame* frame
) noexcept {

    const int result = avcodec_send_frame(
        codec_context,
        frame
    );

    if (result < 0) {
        return MovieWriterError::EncoderError;
    }

    return writePackets(
        codec_context,
        stream,
        format_context,
        packet
    );
}


static MovieWriterError flushEncoder(
    AVCodecContext* codec_context,
    AVStream* stream,
    AVFormatContext* format_context,
    AVPacket* packet
) noexcept {

    const int result = avcodec_send_frame(
        codec_context,
        nullptr
    );

    if (result < 0 && result != AVERROR_EOF) {
        return MovieWriterError::EncoderError;
    }

    return writePackets(
        codec_context,
        stream,
        format_context,
        packet
    );
}

} // namespace


MovieWriter::~MovieWriter() {
    delete video_frame_buffer_;
}


MovieWriter::MovieWriter(MovieWriter&& other) noexcept
    : video_frame_buffer_(std::exchange(
          other.video_frame_buffer_,
          nullptr
          )),
      ref_ptr_(std::exchange(
          other.ref_ptr_,
          nullptr
          )) {
}


MovieWriter& MovieWriter::operator = (MovieWriter&& other) noexcept {

    if (this != &other) {
        delete video_frame_buffer_;

        video_frame_buffer_ = std::exchange(
            other.video_frame_buffer_,
            nullptr
        );

        ref_ptr_ = std::exchange(
            other.ref_ptr_,
            nullptr
        );
    }

    return *this;
}


MovieWriterError MovieWriter::writeVideoWithAudio(
    const String& file_path,
    std::int64_t video_frame_count,
    const MovieWriterConfig& config,
    MovieWriterFrameCallbackFunc frame_callback,
    Signal* audio_signal,
    void* ref
) {

    if (frame_callback == nullptr ||
        file_path.utf8() == nullptr ||
        file_path.utf8()[0] == '\0' ||
        video_frame_count <= 0 ||
        config.width <= 0 ||
        config.height <= 0 ||
        config.frame_rate_num <= 0 ||
        config.frame_rate_den <= 0) {

        return MovieWriterError::InvalidArgument;
    }

    const bool has_audio = config.has_audio && audio_signal != nullptr;

    if (config.has_audio && audio_signal == nullptr) {
        return MovieWriterError::InvalidArgument;
    }

    ref_ptr_ = ref;





    //--------------------------------------------------------------------------
    //  Create the video frame buffer.
    //--------------------------------------------------------------------------

    delete video_frame_buffer_;

    video_frame_buffer_ = new Image();
    if (video_frame_buffer_ == nullptr) {
        return MovieWriterError::OutOfMemory;
    }

    if (!video_frame_buffer_->create(
        config.width,
        config.height,
        4,
        Grain::Image::PixelType::Float32
    )) {
        delete video_frame_buffer_;
        return MovieWriterError::OutOfMemory;
    }

    MovieWriterResources resources;

    AVFormatContext*& fmt_ctx = resources.format_context;
    AVCodecContext*& video_ctx = resources.video_context;
    AVCodecContext*& audio_ctx = resources.audio_context;

    AVFrame*& frame_yuv = resources.video_frame;
    AVFrame*& frame_yuv_intermediate = resources.intermediate_frame;
    AVFrame*& frame_rgb = resources.rgb_frame;
    AVFrame*& audio_frame = resources.audio_frame;

    AVPacket*& packet = resources.packet;

    SwsContext*& sws_rgb_to_yuv = resources.rgb_to_yuv;
    SwsContext*& sws_444_to_yuv = resources.intermediate_to_yuv;

    AVDictionary*& opts = resources.options;


    //--------------------------------------------------------------------------
    //  Format.
    //--------------------------------------------------------------------------

    if (avformat_alloc_output_context2(
        &fmt_ctx,
        nullptr,
        nullptr,
        file_path.utf8()
    ) < 0 || fmt_ctx == nullptr) {

        return MovieWriterError::OutOfMemory;
    }


    //--------------------------------------------------------------------------
    //  Video codec.
    //--------------------------------------------------------------------------

    AVCodecID video_codec_id = AV_CODEC_ID_NONE;

    switch (config.video_codec) {
        case MovieVideoCodec::H264:
            video_codec_id = AV_CODEC_ID_H264;
            break;

        case MovieVideoCodec::ProRes:
            video_codec_id = AV_CODEC_ID_PRORES;
            break;

        default:
            return MovieWriterError::UnsupportedCodec;
    }

    const AVCodec* video_codec = avcodec_find_encoder(video_codec_id);

    if (video_codec == nullptr) {
        return MovieWriterError::UnsupportedCodec;
    }

    AVStream* video_stream = avformat_new_stream(fmt_ctx, nullptr);

    if (video_stream == nullptr) {
        return MovieWriterError::OutOfMemory;
    }

    video_ctx = avcodec_alloc_context3(video_codec);

    if (video_ctx == nullptr) {
        return MovieWriterError::OutOfMemory;
    }

    video_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    video_ctx->width = config.width;
    video_ctx->height = config.height;

    video_ctx->time_base = AVRational{
        config.frame_rate_den,
        config.frame_rate_num
    };

    video_ctx->framerate = AVRational{
        config.frame_rate_num,
        config.frame_rate_den
    };

    video_ctx->gop_size = config.frame_rate_num / config.frame_rate_den;
    video_ctx->max_b_frames = 2;


    //--------------------------------------------------------------------------
    //  Video pixel format and codec options.
    //--------------------------------------------------------------------------

    AVPixelFormat intermediate_fmt = AV_PIX_FMT_NONE;

    if (config.video_codec == MovieVideoCodec::H264) {

        video_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

        video_ctx->color_range = AVCOL_RANGE_MPEG;
        video_ctx->color_primaries = AVCOL_PRI_BT709;
        video_ctx->color_trc = AVCOL_TRC_BT709;
        video_ctx->colorspace = AVCOL_SPC_BT709;

        av_dict_set(&opts, "preset", "fast", 0);

        const std::string quality = std::to_string(
            config.video_quality
        );

        av_dict_set(
            &opts,
            "crf",
            quality.c_str(),
            0
        );

    } else if (config.video_codec == MovieVideoCodec::ProRes) {

        const std::int32_t profile = config.video_quality;

        if (profile < 0 || profile > 5) {
            return MovieWriterError::InvalidArgument;
        }

        const std::string profile_string = std::to_string(profile);

        av_dict_set(
            &opts,
            "profile",
            profile_string.c_str(),
            0
        );

        switch (profile) {

            case 0:
            case 1:
            case 2:
            case 3:
                video_ctx->pix_fmt = AV_PIX_FMT_YUV422P10LE;
                break;

            case 4:
                video_ctx->pix_fmt = AV_PIX_FMT_YUV444P10LE;
                break;

            case 5:
                video_ctx->pix_fmt = AV_PIX_FMT_YUVA444P10LE;
                break;

            default:
                return MovieWriterError::InvalidArgument;
        }
    }


    // RGBF32 -> YUV422P10 and RGBF32 -> YUV420P
    // are handled through an intermediate YUV444P10 frame.

    if (video_ctx->pix_fmt == AV_PIX_FMT_YUV422P10LE ||
        video_ctx->pix_fmt == AV_PIX_FMT_YUV420P) {

        intermediate_fmt = AV_PIX_FMT_YUV444P10LE;
    }


    if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        video_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if (avcodec_open2(video_ctx, video_codec, &opts) < 0) {
        return MovieWriterError::UnsupportedFormat;
    }

    av_dict_free(&opts);

    if (avcodec_parameters_from_context(
        video_stream->codecpar,
        video_ctx
    ) < 0) {

        return MovieWriterError::EncoderError;
    }

    video_stream->time_base = video_ctx->time_base;


    //--------------------------------------------------------------------------
    //  Audio codec.
    //--------------------------------------------------------------------------

    AVStream* audio_stream = nullptr;
/*
    if (has_audio) {

        AVCodecID audio_codec_id = AV_CODEC_ID_NONE;

        switch (config.audio_codec) {
            case MovieAudioCodec::AAC:
                audio_codec_id = AV_CODEC_ID_AAC;
                break;

            default:
                return MovieWriterError::UnsupportedCodec;
        }

        const AVCodec* audio_codec = avcodec_find_encoder(
            audio_codec_id
        );

        if (audio_codec == nullptr) {
            return MovieWriterError::UnsupportedCodec;
        }

        audio_stream = avformat_new_stream(fmt_ctx, nullptr);

        if (audio_stream == nullptr) {
            return MovieWriterError::OutOfMemory;
        }

        audio_ctx = avcodec_alloc_context3(audio_codec);

        if (audio_ctx == nullptr) {
            return MovieWriterError::OutOfMemory;
        }

        audio_ctx->codec_type = AVMEDIA_TYPE_AUDIO;
        audio_ctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
        audio_ctx->sample_rate = audio_signal->sampleRate();
        audio_ctx->bit_rate = config.audio_bitrate;

        audio_ctx->time_base = AVRational{
            1,
            audio_ctx->sample_rate
        };

        av_channel_layout_default(
            &audio_ctx->ch_layout,
            audio_signal->channelCount()
        );

        if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            audio_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (avcodec_open2(audio_ctx, audio_codec, nullptr) < 0) {
            return MovieWriterError::UnsupportedFormat;
        }

        if (avcodec_parameters_from_context(
            audio_stream->codecpar,
            audio_ctx
        ) < 0) {

            return MovieWriterError::EncoderError;
        }

        audio_stream->time_base = audio_ctx->time_base;
    }
*/

    //--------------------------------------------------------------------------
    //  Output file.
    //--------------------------------------------------------------------------

    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {

        if (avio_open(
            &fmt_ctx->pb,
            file_path.utf8(),
            AVIO_FLAG_WRITE
        ) < 0) {

            return MovieWriterError::FileError;
        }

        resources.io_open = true;
    }

    if (avformat_write_header(fmt_ctx, nullptr) < 0) {
        return MovieWriterError::WriteError;
    }


    //--------------------------------------------------------------------------
    //  Allocate video frames.
    //--------------------------------------------------------------------------

    frame_yuv = av_frame_alloc();

    if (frame_yuv == nullptr) {
        return MovieWriterError::OutOfMemory;
    }

    frame_yuv->format = video_ctx->pix_fmt;
    frame_yuv->width = config.width;
    frame_yuv->height = config.height;

    if (av_frame_get_buffer(frame_yuv, 32) < 0) {
        return MovieWriterError::OutOfMemory;
    }


    if (intermediate_fmt != AV_PIX_FMT_NONE) {

        frame_yuv_intermediate = av_frame_alloc();

        if (frame_yuv_intermediate == nullptr) {
            return MovieWriterError::OutOfMemory;
        }

        frame_yuv_intermediate->format = intermediate_fmt;
        frame_yuv_intermediate->width = config.width;
        frame_yuv_intermediate->height = config.height;

        if (av_frame_get_buffer(frame_yuv_intermediate, 32) < 0) {
            return MovieWriterError::OutOfMemory;
        }
    }


    frame_rgb = av_frame_alloc();

    if (frame_rgb == nullptr) {
        return MovieWriterError::OutOfMemory;
    }

    frame_rgb->format = AV_PIX_FMT_RGBF32LE;
    frame_rgb->width = config.width;
    frame_rgb->height = config.height;

    if (av_frame_get_buffer(frame_rgb, 32) < 0) {
        return MovieWriterError::OutOfMemory;
    }


    //--------------------------------------------------------------------------
    //  RGB -> YUV conversion.
    //--------------------------------------------------------------------------

    const AVPixelFormat conversion_format =
        intermediate_fmt != AV_PIX_FMT_NONE
            ? intermediate_fmt
            : video_ctx->pix_fmt;

    sws_rgb_to_yuv = sws_getContext(
        config.width,
        config.height,
        AV_PIX_FMT_RGBF32LE,
        config.width,
        config.height,
        conversion_format,
        SWS_LANCZOS | SWS_ACCURATE_RND,
        nullptr,
        nullptr,
        nullptr
    );

    if (sws_rgb_to_yuv == nullptr) {
        return MovieWriterError::UnsupportedFormat;
    }

    if (config.video_codec == MovieVideoCodec::H264) {

        const int* coefficients = sws_getCoefficients(
            SWS_CS_ITU709
        );

        if (sws_setColorspaceDetails(
            sws_rgb_to_yuv,
            coefficients,
            1,
            coefficients,
            0,
            0,
            1 << 16,
            1 << 16
        ) < 0) {

            return MovieWriterError::UnsupportedFormat;
        }
    }


    if (intermediate_fmt != AV_PIX_FMT_NONE) {

        sws_444_to_yuv = sws_getContext(
            config.width,
            config.height,
            intermediate_fmt,
            config.width,
            config.height,
            video_ctx->pix_fmt,
            SWS_LANCZOS | SWS_ACCURATE_RND,
            nullptr,
            nullptr,
            nullptr
        );

        if (sws_444_to_yuv == nullptr) {
            return MovieWriterError::UnsupportedFormat;
        }
    }


    //--------------------------------------------------------------------------
    //  Packet.
    //--------------------------------------------------------------------------

    packet = av_packet_alloc();

    if (packet == nullptr) {
        return MovieWriterError::OutOfMemory;
    }


    //--------------------------------------------------------------------------
    //  Encode video frames.
    //--------------------------------------------------------------------------

    for (std::int64_t frame_index = 0;
         frame_index < video_frame_count;
         ++frame_index) {

        if (!frame_callback(this, frame_index, ref)) {
            return MovieWriterError::Cancelled;
        }

        if (av_frame_make_writable(frame_rgb) < 0 ||
            av_frame_make_writable(frame_yuv) < 0) {

            return MovieWriterError::EncoderError;
        }

        if (frame_yuv_intermediate != nullptr &&
            av_frame_make_writable(frame_yuv_intermediate) < 0) {

            return MovieWriterError::EncoderError;
        }


        // Copy RGB components from the Grain image buffer.

        ImageAccess image_access(*video_frame_buffer_);

        float* pixel = image_access.transferPtr();

        for (std::int32_t y = 0; y < config.height; ++y) {

            image_access.beginRow(y);

            float* row = reinterpret_cast<float*>(
                frame_rgb->data[0] +
                y * frame_rgb->linesize[0]
            );

            std::int32_t x = 0;

            do {
                image_access.read();

                row[x + 0] = pixel[0];
                row[x + 1] = pixel[1];
                row[x + 2] = pixel[2];

                x += 3;

            } while (image_access.nextX());
        }

        // RGB -> YUV444P10 or final YUV format.

        AVFrame* conversion_target =
            frame_yuv_intermediate != nullptr
                ? frame_yuv_intermediate
                : frame_yuv;

        if (sws_scale(
            sws_rgb_to_yuv,
            frame_rgb->data,
            frame_rgb->linesize,
            0,
            config.height,
            conversion_target->data,
            conversion_target->linesize
        ) != config.height) {

            return MovieWriterError::EncoderError;
        }


        // YUV444P10 -> final format, when required.

        if (frame_yuv_intermediate != nullptr) {

            if (sws_scale(
                sws_444_to_yuv,
                frame_yuv_intermediate->data,
                frame_yuv_intermediate->linesize,
                0,
                config.height,
                frame_yuv->data,
                frame_yuv->linesize
            ) != config.height) {

                return MovieWriterError::EncoderError;
            }
        }

        frame_yuv->pts = frame_index;

        const MovieWriterError encode_result = encodeFrame(
            video_ctx,
            video_stream,
            fmt_ctx,
            packet,
            frame_yuv
        );

        if (encode_result != MovieWriterError::None) {
            return encode_result;
        }
    }


    //--------------------------------------------------------------------------
    //  Encode audio.
    //--------------------------------------------------------------------------
/*
    if (has_audio) {

        audio_frame = av_frame_alloc();

        if (audio_frame == nullptr) {
            return MovieWriterError::OutOfMemory;
        }

        audio_frame->format = audio_ctx->sample_fmt;
        audio_frame->sample_rate = audio_ctx->sample_rate;

        if (av_channel_layout_copy(
            &audio_frame->ch_layout,
            &audio_ctx->ch_layout
        ) < 0) {

            return MovieWriterError::OutOfMemory;
        }

        const int channels = audio_ctx->ch_layout.nb_channels;
        const std::int64_t total_samples = audio_signal->sampleCount();

        const int frame_size = audio_ctx->frame_size;

        if (frame_size <= 0) {
            return MovieWriterError::UnsupportedFormat;
        }

        audio_frame->nb_samples = frame_size;

        if (av_frame_get_buffer(audio_frame, 0) < 0) {
            return MovieWriterError::OutOfMemory;
        }

        std::int64_t sample_position = 0;

        while (sample_position < total_samples) {

            const int sample_count = static_cast<int>(
                std::min<std::int64_t>(
                    frame_size,
                    total_samples - sample_position
                )
            );

            if (av_frame_make_writable(audio_frame) < 0) {
                return MovieWriterError::EncoderError;
            }

            audio_frame->nb_samples = sample_count;

            for (int channel = 0; channel < channels; ++channel) {

                float* destination = reinterpret_cast<float*>(
                    audio_frame->data[channel]
                );

                for (int i = 0; i < sample_count; ++i) {

                    destination[i] = audio_signal->readFloat(
                        channel,
                        sample_position + i
                    );
                }
            }

            audio_frame->pts = sample_position;

            const MovieWriterError encode_result = encodeFrame(
                audio_ctx,
                audio_stream,
                fmt_ctx,
                packet,
                audio_frame
            );

            if (encode_result != MovieWriterError::None) {
                return encode_result;
            }

            sample_position += sample_count;
        }
    }
*/

    //--------------------------------------------------------------------------
    //  Flush encoders.
    //--------------------------------------------------------------------------

    MovieWriterError result = flushEncoder(
        video_ctx,
        video_stream,
        fmt_ctx,
        packet
    );

    if (result != MovieWriterError::None) {
        return result;
    }

    if (has_audio) {

        result = flushEncoder(
            audio_ctx,
            audio_stream,
            fmt_ctx,
            packet
        );

        if (result != MovieWriterError::None) {
            return result;
        }
    }


    //--------------------------------------------------------------------------
    //  Finalize output.
    //--------------------------------------------------------------------------

    if (av_write_trailer(fmt_ctx) < 0) {
        return MovieWriterError::WriteError;
    }

    return MovieWriterError::None;
}


} // namespace Grain