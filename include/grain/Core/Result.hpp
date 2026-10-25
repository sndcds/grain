#pragma once

#include <cstdint>

namespace Grain {

enum class Result : std::int32_t {
    Success = 0,
    Error,
    InvalidArgument,
    NullPtr,
    OutOfMemory,
    NotFound,
    NotInitialized,
    Unsupported
};

}