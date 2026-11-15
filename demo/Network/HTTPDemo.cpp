
#include "../../include/Grain.hpp"

#include <cstdio>
#include <exception>
#include <string>


using namespace Grain;


int main() {

    HTTPClient client;

    try {

        HTTPRequest request;

        request.method = HTTPMethod::Get;
        request.url = "https://api.kulturbytes.de/api/events?limit=10";

        request.headers = {
            "Accept: application/json",
            "User-Agent: Grain HTTPClient/1.0"
        };

        request.timeout = 30000;
        request.connectTimeout = 10000;
        request.followRedirects = true;


        const auto response = client.fetch(request);


        if (response.isError()) {

            std::fprintf(
                stderr,
                "HTTP request failed: %s\n",
                response.errorMessage.c_str()
            );

            return 1;
        }


        std::printf(
            "Status: %ld\n",
            response.statusCode
        );

        std::printf(
            "Content-Type: %s\n",
            response.contentType.c_str()
        );

        std::printf(
            "Response size: %zu bytes\n",
            response.size()
        );


        if (!response.isSuccess()) {

            std::fprintf(
                stderr,
                "HTTP error: %ld\n",
                response.statusCode
            );

            std::fprintf(
                stderr,
                "%s\n",
                response.body.c_str()
            );

            return 1;
        }


        std::printf(
            "\nResponse body:\n%s\n",
            response.body.c_str()
        );

    }
    catch (const std::exception& e) {

        std::fprintf(
            stderr,
            "HTTP client exception: %s\n",
            e.what()
        );

        return 1;
    }


    return 0;
}
