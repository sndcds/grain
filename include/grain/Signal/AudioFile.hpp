
#pragma once

#include <cstdint>
#include <filesystem>

namespace Grain {

class Signal;


enum class AudioFileFormat : uint8_t {
    Auto,
    AIFF,
    WAV,
    MP3,
    AAC,
    M4A,
    FLAC,
    OGG
};


enum class AudioFileError : int32_t {
    None,
    FileNotFound,
    FileOpenFailed,
    UnsupportedSampleType,
    UnsupportedFormat,
    UnsupportedCodec,
    InvalidAudioData,
    InvalidConfiguration,
    AllocationFailed,
    ReadFailed,
    WriteFailed,
    EncodingFailed,
    DecodingFailed
};


struct AudioFileOptions {

    AudioFileFormat format = AudioFileFormat::Auto;

    int32_t bit_rate = 192000;
    int32_t sample_rate = 0;
    int32_t channel_count = 0;

    bool overwrite = false;
};


class AudioFile {

public:

    [[nodiscard]]
    static auto load(
        Signal& signal,
        const std::filesystem::path& path,
        const AudioFileOptions& options = {}
    ) noexcept -> AudioFileError;


    [[nodiscard]]
    static auto save(
        const Signal& signal,
        const std::filesystem::path& path,
        const AudioFileOptions& options = {}
    ) noexcept -> AudioFileError;
};

}
