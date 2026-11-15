
#include "../../include/grain/JSON/JSONWalker.hpp"

#include <fstream>
#include <exception>


namespace Grain {


//--------------------------------------------------------------------------
// Loading
//--------------------------------------------------------------------------

auto JSONWalker::loadFromString(
    const char* jsonString
) -> bool {

    last_error_ = "";

    if (!jsonString) {
        last_error_ = "JSON string is null";
        json_root_ = nullptr;
        return false;
    }

    try {
        json_root_ = json::parse(jsonString);
        return true;
    }
    catch (const std::exception& e) {
        json_root_ = nullptr;
        last_error_ = e.what();
        return false;
    }
}


auto JSONWalker::loadFromFile(
    const String& filePath
) -> bool {

    last_error_ = "";
    json_root_ = nullptr;

    std::ifstream file{filePath.utf8()};

    if (!file.is_open()) {
        last_error_ = "Could not open JSON file";
        return false;
    }

    try {
        json_root_ = json::parse(file);
        return true;
    }
    catch (const std::exception& e) {
        json_root_ = nullptr;
        last_error_ = e.what();
        return false;
    }
}


//--------------------------------------------------------------------------
// Traversal
//--------------------------------------------------------------------------

auto JSONWalker::traverse() -> void {

    if (json_root_.is_null()) {
        return;
    }

    walkValue(json_root_, "");
}


auto JSONWalker::walkValue(
    const json& value,
    const std::string& path
) -> void {

    if (value.is_object()) {

        const String callbackPath{path.c_str()};

        onObjectStart(callbackPath);

        for (auto it = value.begin(); it != value.end(); ++it) {

            std::string childPath;

            if (path.empty()) {
                childPath = it.key();
            }
            else {
                childPath = path + "." + it.key();
            }

            walkValue(it.value(), childPath);
        }

        onObjectEnd(callbackPath);
    }

    else if (value.is_array()) {

        const String callbackPath{path.c_str()};

        onArrayStart(callbackPath);

        for (std::size_t i = 0; i < value.size(); ++i) {

            const std::string childPath =
                path + "[" + std::to_string(i) + "]";

            walkValue(value[i], childPath);
        }

        onArrayEnd(callbackPath);
    }

    else {

        const String propertyPath{path.c_str()};

        const JSONValue propertyValue{&value};

        addProperty(propertyPath, propertyValue);
    }
}


//--------------------------------------------------------------------------
// State
//--------------------------------------------------------------------------

auto JSONWalker::lastError() const noexcept -> const String& {
    return last_error_;
}


auto JSONWalker::jsonRoot() const noexcept -> const json& {
    return json_root_;
}


//--------------------------------------------------------------------------
// Default callbacks
//--------------------------------------------------------------------------

auto JSONWalker::onObjectStart(
    const String& path
) -> void {
}


auto JSONWalker::onObjectEnd(
    const String& path
) -> void {
}


auto JSONWalker::onArrayStart(
    const String& path
) -> void {
}


auto JSONWalker::onArrayEnd(
    const String& path
) -> void {
}


auto JSONWalker::addProperty(
    const String& path,
    const JSONValue& value
) -> void {
}


}
