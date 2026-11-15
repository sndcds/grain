#pragma once

#include <cstdint>
#include <string>
#include <vector>


namespace Grain {


enum class HTTPMethod : uint8_t {
    Get = 0,
    Post,
    Put,
    Patch,
    Delete,
    Head
};


struct HTTPRequest {

    HTTPMethod method = HTTPMethod::Get;

    std::string url;

    std::vector<std::string> headers;

    std::string body;

    uint32_t timeout = 30000;

    uint32_t connectTimeout = 10000;

    bool followRedirects = true;

};


}