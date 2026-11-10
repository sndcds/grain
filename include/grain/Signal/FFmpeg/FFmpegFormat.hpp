#pragma once

#include <grain/Signal/AudioFile.hpp>

#include <algorithm>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}


namespace Grain::Internal {

struct FFmpegFormatInfo {

    const char* container = nullptr;
    AVCodecID codec = AV_CODEC_ID_NONE;
};


[[nodiscard]]
inline auto getFFmpegFormatInfo(
    AudioFileFormat format
) noexcept -> FFmpegFormatInfo {

    switch (format) {

        case AudioFileFormat::AIFF:
            return {"aiff", AV_CODEC_ID_PCM_S16BE};

        case AudioFileFormat::WAV:
            return {"wav", AV_CODEC_ID_PCM_S16LE};

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


[[nodiscard]]
inline auto getAudioFileFormat(
    const std::filesystem::path& path
) -> AudioFileFormat {

    auto extension = path.extension().string();

    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    if (extension == ".aiff" || extension == ".aif") {
        return AudioFileFormat::AIFF;
    }

    if (extension == ".wav" || extension == ".wave") {
        return AudioFileFormat::WAV;
    }

    if (extension == ".mp3") {
        return AudioFileFormat::MP3;
    }

    if (extension == ".aac") {
        return AudioFileFormat::AAC;
    }

    if (extension == ".m4a") {
        return AudioFileFormat::M4A;
    }

    if (extension == ".flac") {
        return AudioFileFormat::FLAC;
    }

    if (extension == ".ogg" || extension == ".oga") {
        return AudioFileFormat::OGG;
    }

    return AudioFileFormat::Auto;
}

} // namespace Grain::Internal
