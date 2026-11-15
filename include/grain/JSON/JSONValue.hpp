
#pragma once

#include "JSON.hpp"
#include <grain/String/String.hpp>


namespace Grain {


class JSONValue {

public:

    explicit JSONValue(const json* value) noexcept;


    // Type checks

    [[nodiscard]]
    auto isNull() const noexcept -> bool;

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


    // Value accessors

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

    const json* value_ = nullptr;

};


}
