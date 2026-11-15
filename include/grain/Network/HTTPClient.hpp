#pragma once

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"


namespace Grain {


class HTTPClient {

public:

    HTTPClient() = default;

    ~HTTPClient() = default;

    HTTPClient(const HTTPClient&) = delete;
    auto operator=(const HTTPClient&) -> HTTPClient& = delete;

    HTTPClient(HTTPClient&&) noexcept = default;
    auto operator=(HTTPClient&&) noexcept -> HTTPClient& = default;


    [[nodiscard]]
    auto fetch(const HTTPRequest& request) -> HTTPResponse;

};


}