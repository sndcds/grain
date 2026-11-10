#pragma once

#include <grain/Movie/Movie.hpp>

namespace Grain {

    class Signal;
    class Image;
    class String;
    class MovieWriter;


    enum class MovieWriterError : std::uint8_t {
        None = 0,
        InvalidArgument,
        Cancelled,
        UnsupportedCodec,
        UnsupportedFormat,
        OutOfMemory,
        FileError,
        EncoderError,
        WriteError
    };


    using MovieWriterFrameCallbackFunc = bool (*)(
        MovieWriter* movie_writer,
        int64_t frame_index,
        void* ref
    );


    struct MovieWriterConfig {

        // Video
        int32_t width = 1920;
        int32_t height = 1080;

        int32_t frame_rate_num = 25;
        int32_t frame_rate_den = 1;

        MovieVideoCodec video_codec = MovieVideoCodec::H264;

        // H.264: CRF value (lower means better quality).
        // ProRes: codec-specific quality setting.
        int32_t video_quality = 23;

        // Audio
        bool has_audio = true;
        MovieAudioCodec audio_codec = MovieAudioCodec::AAC;
        int32_t audio_bitrate = 128000;
    };


    class MovieWriter {
    public:
        MovieWriter() = default;
        ~MovieWriter();

        MovieWriter(const MovieWriter&) = delete;
        MovieWriter& operator = (const MovieWriter&) = delete;

        MovieWriter(MovieWriter&&) noexcept;
        MovieWriter& operator = (MovieWriter&&) noexcept;


        MovieWriterError writeVideoWithAudio(
            const String& file_path,
            int64_t video_frame_count,
            const MovieWriterConfig& config,
            MovieWriterFrameCallbackFunc frame_callback,
            Signal* audio_signal = nullptr,
            void* ref = nullptr
        );


        // The frame buffer is owned by MovieWriter.
        // Valid only during the frame callback.
        [[nodiscard]]
        Image* videoFrameBufferPtr() {
            return video_frame_buffer_;
        }


        // User-defined callback reference.
        [[nodiscard]]
        void* refPtr() {
            return ref_ptr_;
        }


    private:
        Image* video_frame_buffer_ = nullptr;
        void* ref_ptr_ = nullptr;
    };

} // namespace Grain