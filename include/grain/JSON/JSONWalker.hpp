
#pragma once

#include <string>

#include "JSONAccessor.hpp"


namespace Grain {


class JSONWalker {

public:

    virtual ~JSONWalker() = default;


    // Loading

    [[nodiscard]]
    auto loadFromString(const char* jsonString) -> bool;

    [[nodiscard]]
    auto loadFromFile(const String& filePath) -> bool;


    // Traversal

    auto traverse() -> void;


    // State

    [[nodiscard]]
    auto lastError() const noexcept -> const String&;

    [[nodiscard]]
    auto jsonRoot() const noexcept -> const json&;


protected:

    // Callbacks

    virtual auto onObjectStart(const String& path) -> void;

    virtual auto onObjectEnd(const String& path) -> void;

    virtual auto onArrayStart(const String& path) -> void;

    virtual auto onArrayEnd(const String& path) -> void;

    virtual auto addProperty(
        const String& path,
        const JSONValue& value
    ) -> void;


    json json_root_;
    String last_error_;


private:

    auto walkValue(
        const json& value,
        const std::string& path
    ) -> void;

};


}
