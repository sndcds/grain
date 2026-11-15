
#pragma once

#include "JSONValue.hpp"


namespace Grain {


class JSONAccessor {

public:

    explicit JSONAccessor(const json& root) noexcept;


    // State

    [[nodiscard]]
    auto isValid() const noexcept -> bool;

    [[nodiscard]]
    auto isNull() const noexcept -> bool;

    [[nodiscard]]
    auto isObject() const noexcept -> bool;

    [[nodiscard]]
    auto isArray() const noexcept -> bool;

    [[nodiscard]]
    auto isString() const noexcept -> bool;

    [[nodiscard]]
    auto isInt() const noexcept -> bool;

    [[nodiscard]]
    auto isUInt() const noexcept -> bool;

    [[nodiscard]]
    auto isFloat() const noexcept -> bool;

    [[nodiscard]]
    auto isBoolean() const noexcept -> bool;

    [[nodiscard]]
    auto isNumber() const noexcept -> bool;


    // Size

    [[nodiscard]]
    auto size() const noexcept -> std::size_t;


    // Navigation

    [[nodiscard]]
    auto at(const char* key) const -> JSONAccessor;

    [[nodiscard]]
    auto at(std::size_t index) const noexcept -> JSONAccessor;


    // Value access

    [[nodiscard]]
    auto asString() const -> String;

    [[nodiscard]]
    auto asInt64() const -> int64_t;

    [[nodiscard]]
    auto asInt32() const -> int32_t;

    [[nodiscard]]
    auto asDouble() const -> double;

    [[nodiscard]]
    auto asFloat() const -> float;

    [[nodiscard]]
    auto asBoolean() const -> bool;


    // Raw access

    [[nodiscard]]
    auto raw() const noexcept -> const json*;


private:

    explicit JSONAccessor(const json* value) noexcept;

    const json* value_ = nullptr;

};


}
