#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>


namespace Grain {


enum class HTTPError : uint8_t {
    None = 0,
    InvalidURL,
    InitializationFailed,
    ConnectionFailed,
    Timeout,
    TransferFailed
};


struct HTTPResponse {

    HTTPError error = HTTPError::None;

    std::string errorMessage;

    long statusCode = 0;

    std::string contentType;

    std::unordered_map<std::string, std::string> headers;

    std::string body;


    [[nodiscard]]
    auto isError() const noexcept -> bool {
        return error != HTTPError::None;
    }

    [[nodiscard]]
    auto isSuccess() const noexcept -> bool {
        return !isError() && statusCode >= 200 && statusCode < 300;
    }

    [[nodiscard]]
    auto isRedirect() const noexcept -> bool {
        return statusCode >= 300 && statusCode < 400;
    }

    [[nodiscard]]
    auto size() const noexcept -> std::size_t {
        return body.size();
    }

};


}