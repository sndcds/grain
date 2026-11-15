
#include "../../include/Grain.hpp"

#include <cstdio>

using namespace Grain;


int main() {

    const json document = json::parse(R"({
        "event": {
            "uuid": "019d2eed-a25a-7caf-a762-a2a71917c6a1",
            "title": "Open Source Workshop",
            "active": true,
            "price": 12.50
        },
        "tags": [
            "Open Source",
            "Workshop"
        ]
    })");


    JSONAccessor root{document};


    const auto event = root.at("event");

    const auto uuid = event.at("uuid").asString();
    const auto title = event.at("title").asString();
    const auto active = event.at("active").asBoolean();
    const auto price = event.at("price").asDouble();


    std::printf("UUID: %s\n", uuid.c_str());
    std::printf("Title: %s\n", title.c_str());
    std::printf("Active: %s\n", active ? "true" : "false");
    std::printf("Price: %.2f\n", price);


    const auto tags = root.at("tags");

    for (std::size_t i = 0; i < tags.size(); ++i) {

        const auto tag = tags.at(i).asString();

        std::printf("Tag: %s\n", tag.c_str());
    }


    return 0;
}
