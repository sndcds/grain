#pragma once

#include <cstdint>

namespace Grain {

enum class MovieVideoCodec : std::uint8_t {
    H264 = 0,
    ProRes
};


enum class MovieAudioCodec : std::uint8_t {
    AAC = 0
};


class Movie {
public:
    Movie() = default;
    ~Movie() = default;

    Movie(const Movie&) = delete;
    Movie& operator = (const Movie&) = delete;

    Movie(Movie&&) = default;
    Movie& operator = (Movie&&) = default;
};

} // namespace Grain
