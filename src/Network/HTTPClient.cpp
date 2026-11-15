
#include <grain/Network/HTTPClient.hpp>

#include <curl/curl.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <string>


namespace Grain {


namespace {

class CurlGlobal {

public:

    CurlGlobal()
        : result_(curl_global_init(CURL_GLOBAL_DEFAULT)) {}

    ~CurlGlobal() {
        if (result_ == CURLE_OK) {
            curl_global_cleanup();
        }
    }

    CurlGlobal(const CurlGlobal&) = delete;
    auto operator = (const CurlGlobal&) -> CurlGlobal& = delete;

    [[nodiscard]]
    auto isInitialized() const noexcept -> bool {
        return result_ == CURLE_OK;
    }

private:

    CURLcode result_;

};


auto curlGlobal() -> CurlGlobal& {
    static CurlGlobal instance;
    return instance;
}



//------------------------------------------------------------------------------
//  Constants
//------------------------------------------------------------------------------

constexpr std::size_t MAX_RESPONSE_SIZE = 64 * 1024 * 1024;


//------------------------------------------------------------------------------
//  Curl handle
//------------------------------------------------------------------------------

using CurlHandle = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;

using CurlHeaders = std::unique_ptr<
    curl_slist,
    decltype(&curl_slist_free_all)
>;


//------------------------------------------------------------------------------
//  String helpers
//------------------------------------------------------------------------------

auto trim(std::string value) -> std::string {

    const auto isSpace = [](unsigned char c) -> bool {
        return std::isspace(c) != 0;
    };

    const auto first = std::find_if_not(
        value.begin(),
        value.end(),
        isSpace
    );

    const auto last = std::find_if_not(
        value.rbegin(),
        value.rend(),
        isSpace
    ).base();

    if (first >= last) {
        return {};
    }

    return std::string(first, last);
}


auto lower(std::string value) -> std::string {

    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) -> char {
            return static_cast<char>(std::tolower(c));
        }
    );

    return value;
}


//------------------------------------------------------------------------------
//  Response body callback
//------------------------------------------------------------------------------

struct CallbackContext {

    HTTPResponse* response = nullptr;

    bool allocationFailed = false;

    bool responseTooLarge = false;
};


auto writeCallback(
    char* data,
    std::size_t size,
    std::size_t count,
    void* userData
) noexcept -> std::size_t {

    auto* context = static_cast<CallbackContext*>(userData);

    if (context == nullptr || context->response == nullptr) {
        return 0;
    }

    if (count != 0 &&
        size > std::numeric_limits<std::size_t>::max() / count) {
        return 0;
    }

    const std::size_t bytes = size * count;

    auto& body = context->response->body;

    if (bytes > MAX_RESPONSE_SIZE - body.size()) {
        context->responseTooLarge = true;
        return 0;
    }

    try {

        body.append(data, bytes);

        return bytes;

    }
    catch (...) {

        context->allocationFailed = true;

        return 0;
    }
}


//------------------------------------------------------------------------------
//  Response header callback
//------------------------------------------------------------------------------

auto headerCallback(
    char* data,
    std::size_t size,
    std::size_t count,
    void* userData
) noexcept -> std::size_t {

    auto* context = static_cast<CallbackContext*>(userData);

    if (context == nullptr || context->response == nullptr) {
        return 0;
    }

    if (count != 0 &&
        size > std::numeric_limits<std::size_t>::max() / count) {
        return 0;
    }

    const std::size_t bytes = size * count;

    try {

        std::string line(data, bytes);

        // Ignore HTTP status lines and empty lines.
        // This also handles intermediate responses from redirects.

        const auto separator = line.find(':');

        if (separator == std::string::npos) {
            return bytes;
        }

        const std::string name = lower(
            trim(line.substr(0, separator))
        );

        const std::string value = trim(
            line.substr(separator + 1)
        );

        if (!name.empty()) {
            context->response->headers[name] = value;
        }

        return bytes;

    }
    catch (...) {

        context->allocationFailed = true;

        return 0;
    }
}


//------------------------------------------------------------------------------
//  HTTP method
//------------------------------------------------------------------------------

auto methodName(HTTPMethod method) noexcept -> const char* {

    switch (method) {

        case HTTPMethod::Get:
            return "GET";

        case HTTPMethod::Post:
            return "POST";

        case HTTPMethod::Put:
            return "PUT";

        case HTTPMethod::Patch:
            return "PATCH";

        case HTTPMethod::Delete:
            return "DELETE";

        case HTTPMethod::Head:
            return "HEAD";
    }

    return nullptr;
}


//------------------------------------------------------------------------------
//  Error handling
//------------------------------------------------------------------------------

auto setError(
    HTTPResponse& response,
    HTTPError error,
    const char* message
) noexcept -> void {

    response.error = error;

    try {
        response.errorMessage = message != nullptr ? message : "Unknown error";
    }
    catch (...) {
        // Preserve the error code even if allocating the message fails.
    }
}

} // anonymous namespace


//------------------------------------------------------------------------------
//  HTTPClient::fetch
//------------------------------------------------------------------------------

auto HTTPClient::fetch(
    const HTTPRequest& request
) -> HTTPResponse {

    HTTPResponse response;

    auto& global = curlGlobal();

    if (!global.isInitialized()) {
        response.error = HTTPError::InitializationFailed;
        response.errorMessage = "Failed to initialize libcurl.";
        return response;
    }


    //--------------------------------------------------------------------------
    //  Validate request
    //--------------------------------------------------------------------------

    if (request.url.empty()) {

        setError(
            response,
            HTTPError::InvalidURL,
            "URL is empty"
        );

        return response;
    }

    const char* method = methodName(request.method);

    if (method == nullptr) {

        setError(
            response,
            HTTPError::TransferFailed,
            "Unsupported HTTP method"
        );

        return response;
    }


    //--------------------------------------------------------------------------
    //  Initialize curl
    //
    //  curl_global_init should be called once before using this client.
    //  See the initialization note below.
    //--------------------------------------------------------------------------

    CurlHandle curl(
        curl_easy_init(),
        &curl_easy_cleanup
    );

    if (!curl) {

        setError(
            response,
            HTTPError::InitializationFailed,
            "Could not initialize libcurl"
        );

        return response;
    }


    //--------------------------------------------------------------------------
    //  Callback context
    //--------------------------------------------------------------------------

    CallbackContext context;

    context.response = &response;


    //--------------------------------------------------------------------------
    //  Configure URL
    //--------------------------------------------------------------------------

    CURLcode result = curl_easy_setopt(
        curl.get(),
        CURLOPT_URL,
        request.url.c_str()
    );

    if (result != CURLE_OK) {

        setError(
            response,
            HTTPError::InvalidURL,
            curl_easy_strerror(result)
        );

        return response;
    }


    //--------------------------------------------------------------------------
    //  Configure response callbacks
    //--------------------------------------------------------------------------

    curl_easy_setopt(
        curl.get(),
        CURLOPT_WRITEFUNCTION,
        writeCallback
    );

    curl_easy_setopt(
        curl.get(),
        CURLOPT_WRITEDATA,
        &context
    );

    curl_easy_setopt(
        curl.get(),
        CURLOPT_HEADERFUNCTION,
        headerCallback
    );

    curl_easy_setopt(
        curl.get(),
        CURLOPT_HEADERDATA,
        &context
    );


    //--------------------------------------------------------------------------
    //  Timeouts
    //--------------------------------------------------------------------------

    curl_easy_setopt(
        curl.get(),
        CURLOPT_TIMEOUT_MS,
        static_cast<long>(request.timeout)
    );

    curl_easy_setopt(
        curl.get(),
        CURLOPT_CONNECTTIMEOUT_MS,
        static_cast<long>(request.connectTimeout)
    );


    //--------------------------------------------------------------------------
    //  Redirects
    //--------------------------------------------------------------------------

    curl_easy_setopt(
        curl.get(),
        CURLOPT_FOLLOWLOCATION,
        request.followRedirects ? 1L : 0L
    );

    curl_easy_setopt(
        curl.get(),
        CURLOPT_MAXREDIRS,
        10L
    );

    curl_easy_setopt(
        curl.get(),
        CURLOPT_PROTOCOLS_STR,
        "http,https"
    );

    curl_easy_setopt(
        curl.get(),
        CURLOPT_REDIR_PROTOCOLS_STR,
        "http,https"
    );


    //--------------------------------------------------------------------------
    //  TLS verification
    //--------------------------------------------------------------------------

    curl_easy_setopt(
        curl.get(),
        CURLOPT_SSL_VERIFYPEER,
        1L
    );

    curl_easy_setopt(
        curl.get(),
        CURLOPT_SSL_VERIFYHOST,
        2L
    );


    //--------------------------------------------------------------------------
    //  HTTP method
    //--------------------------------------------------------------------------

    if (request.method == HTTPMethod::Head) {

        curl_easy_setopt(
            curl.get(),
            CURLOPT_NOBODY,
            1L
        );

    }
    else if (request.method == HTTPMethod::Get) {

        curl_easy_setopt(
            curl.get(),
            CURLOPT_HTTPGET,
            1L
        );

    }
    else if (request.method == HTTPMethod::Post) {

        curl_easy_setopt(
            curl.get(),
            CURLOPT_POST,
            1L
        );

    }
    else {

        curl_easy_setopt(
            curl.get(),
            CURLOPT_CUSTOMREQUEST,
            method
        );
    }


    //--------------------------------------------------------------------------
    //  Request body
    //--------------------------------------------------------------------------

    if (
        request.method != HTTPMethod::Get &&
        request.method != HTTPMethod::Head &&
        !request.body.empty()
    ) {

        curl_easy_setopt(
            curl.get(),
            CURLOPT_POSTFIELDS,
            request.body.data()
        );

        curl_easy_setopt(
            curl.get(),
            CURLOPT_POSTFIELDSIZE_LARGE,
            static_cast<curl_off_t>(request.body.size())
        );
    }


    //--------------------------------------------------------------------------
    //  Request headers
    //--------------------------------------------------------------------------

    curl_slist* rawHeaders = nullptr;

    for (const auto& header : request.headers) {

        curl_slist* next = curl_slist_append(
            rawHeaders,
            header.c_str()
        );

        if (next == nullptr) {

            curl_slist_free_all(rawHeaders);

            setError(
                response,
                HTTPError::InitializationFailed,
                "Could not allocate HTTP headers"
            );

            return response;
        }

        rawHeaders = next;
    }

    CurlHeaders headers(
        rawHeaders,
        &curl_slist_free_all
    );

    if (headers) {

        curl_easy_setopt(
            curl.get(),
            CURLOPT_HTTPHEADER,
            headers.get()
        );
    }


    //--------------------------------------------------------------------------
    //  Execute request
    //--------------------------------------------------------------------------

    result = curl_easy_perform(curl.get());


    //--------------------------------------------------------------------------
    //  Handle transfer errors
    //--------------------------------------------------------------------------

    if (result != CURLE_OK) {

        if (context.responseTooLarge) {

            setError(
                response,
                HTTPError::TransferFailed,
                "Response exceeds maximum allowed size"
            );

        }
        else if (context.allocationFailed) {

            setError(
                response,
                HTTPError::InitializationFailed,
                "Could not allocate response memory"
            );

        }
        else if (result == CURLE_OPERATION_TIMEDOUT) {

            setError(
                response,
                HTTPError::Timeout,
                curl_easy_strerror(result)
            );

        }
        else if (
            result == CURLE_URL_MALFORMAT ||
            result == CURLE_UNSUPPORTED_PROTOCOL
        ) {

            setError(
                response,
                HTTPError::InvalidURL,
                curl_easy_strerror(result)
            );

        }
        else {

            setError(
                response,
                HTTPError::TransferFailed,
                curl_easy_strerror(result)
            );
        }

        return response;
    }


    //--------------------------------------------------------------------------
    //  HTTP status code
    //--------------------------------------------------------------------------

    curl_easy_getinfo(
        curl.get(),
        CURLINFO_RESPONSE_CODE,
        &response.statusCode
    );


    //--------------------------------------------------------------------------
    //  Content type
    //--------------------------------------------------------------------------

    char* contentType = nullptr;

    curl_easy_getinfo(
        curl.get(),
        CURLINFO_CONTENT_TYPE,
        &contentType
    );

    if (contentType != nullptr) {
        response.contentType = contentType;
    }


    //--------------------------------------------------------------------------
    //  Success
    //--------------------------------------------------------------------------

    return response;
}


} // namespace Grain
