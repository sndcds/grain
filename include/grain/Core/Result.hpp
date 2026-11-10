#pragma once

#include <cstdint>

namespace Grain {

enum class Result : int32_t {
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