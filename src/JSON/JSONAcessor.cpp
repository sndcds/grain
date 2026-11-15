
#include "../../include/Grain/JSON/JSONAccessor.hpp"


namespace Grain {


JSONAccessor::JSONAccessor(const json& root) noexcept
    : value_(&root) {
}


JSONAccessor::JSONAccessor(const json* value) noexcept
    : value_(value) {
}


//--------------------------------------------------------------------------
// State
//--------------------------------------------------------------------------

auto JSONAccessor::isValid() const noexcept -> bool {
    return value_ != nullptr;
}


auto JSONAccessor::isNull() const noexcept -> bool {
    return !value_ || value_->is_null();
}


auto JSONAccessor::isObject() const noexcept -> bool {
    return value_ && value_->is_object();
}


auto JSONAccessor::isArray() const noexcept -> bool {
    return value_ && value_->is_array();
}


auto JSONAccessor::isString() const noexcept -> bool {
    return value_ && value_->is_string();
}


auto JSONAccessor::isInt() const noexcept -> bool {
    return value_ && value_->is_number_integer();
}


auto JSONAccessor::isUInt() const noexcept -> bool {
    return value_ && value_->is_number_unsigned();
}


auto JSONAccessor::isFloat() const noexcept -> bool {
    return value_ && value_->is_number_float();
}


auto JSONAccessor::isBoolean() const noexcept -> bool {
    return value_ && value_->is_boolean();
}


auto JSONAccessor::isNumber() const noexcept -> bool {
    return value_ && value_->is_number();
}


//--------------------------------------------------------------------------
// Size
//--------------------------------------------------------------------------

auto JSONAccessor::size() const noexcept -> std::size_t {

    if (!value_) {
        return 0;
    }

    if (!value_->is_object() && !value_->is_array()) {
        return 0;
    }

    return value_->size();
}


//--------------------------------------------------------------------------
// Navigation
//--------------------------------------------------------------------------

auto JSONAccessor::at(const char* key) const -> JSONAccessor {

    if (!value_ || !value_->is_object() || !key) {
        return JSONAccessor{nullptr};
    }

    const auto it = value_->find(key);

    if (it == value_->end()) {
        return JSONAccessor{nullptr};
    }

    return JSONAccessor{&(*it)};
}


auto JSONAccessor::at(std::size_t index) const noexcept -> JSONAccessor {

    if (!value_ || !value_->is_array()) {
        return JSONAccessor{nullptr};
    }

    if (index >= value_->size()) {
        return JSONAccessor{nullptr};
    }

    return JSONAccessor{&(*value_)[index]};
}


//--------------------------------------------------------------------------
// Value access
//--------------------------------------------------------------------------

auto JSONAccessor::asString() const -> String {
    return JSONValue{value_}.asString();
}


auto JSONAccessor::asInt64() const -> int64_t {
    return JSONValue{value_}.asInt64();
}


auto JSONAccessor::asInt32() const -> int32_t {
    return JSONValue{value_}.asInt32();
}


auto JSONAccessor::asDouble() const -> double {
    return JSONValue{value_}.asDouble();
}


auto JSONAccessor::asFloat() const -> float {
    return JSONValue{value_}.asFloat();
}


auto JSONAccessor::asBoolean() const -> bool {
    return JSONValue{value_}.asBoolean();
}


//--------------------------------------------------------------------------
// Raw access
//--------------------------------------------------------------------------

auto JSONAccessor::raw() const noexcept -> const json* {
    return value_;
}


}
