
#include <grain/Signal/AudioFile.hpp>
#include <grain/Signal/FFmpeg/FFmpegReader.hpp>
#include <grain/Signal/FFmpeg/FFmpegWriter.hpp>

#include <system_error>


namespace Grain {

auto AudioFile::load(
    Signal& signal,
    const std::filesystem::path& path,
    const AudioFileOptions& options
) noexcept -> AudioFileError {

    return Internal::FFmpegReader::load(
        signal,
        path,
        options
    );
}


auto AudioFile::save(
    const Signal& signal,
    const std::filesystem::path& path,
    const AudioFileOptions& options
) noexcept -> AudioFileError {

    return Internal::FFmpegWriter::save(
        signal,
        path,
        options
    );
}

} // namespace Grain
