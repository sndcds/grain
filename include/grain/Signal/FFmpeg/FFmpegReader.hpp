#pragma once

#include <filesystem>

#include <grain/Signal/AudioFile.hpp>
#include <grain/Signal/Signal.hpp>


namespace Grain::Internal {

class FFmpegReader {

public:

    [[nodiscard]]
    static auto load(
        Signal& signal,
        const std::filesystem::path& path,
        const AudioFileOptions& options
    ) noexcept -> AudioFileError;
};

}
