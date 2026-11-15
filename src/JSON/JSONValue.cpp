
#include <grain/JSON/JSONValue.hpp>


namespace Grain {


JSONValue::JSONValue(const json* value) noexcept
    : value_(value) {
}


//------------------------------------------------------------------------------
//  Type checks
//------------------------------------------------------------------------------

auto JSONValue::isNull() const noexcept -> bool {
    return !value_ || value_->is_null();
}


auto JSONValue::isString() const noexcept -> bool {
    return value_ && value_->is_string();
}


auto JSONValue::isInt() const noexcept -> bool {
    return value_ && value_->is_number_integer();
}


auto JSONValue::isUInt() const noexcept -> bool {
    return value_ && value_->is_number_unsigned();
}


auto JSONValue::isFloat() const noexcept -> bool {
    return value_ && value_->is_number_float();
}


auto JSONValue::isBoolean() const noexcept -> bool {
    return value_ && value_->is_boolean();
}


auto JSONValue::isNumber() const noexcept -> bool {
    return value_ && value_->is_number();
}


//------------------------------------------------------------------------------
//  Value accessors
//------------------------------------------------------------------------------

auto JSONValue::asString() const -> String {

    if (!value_) {
        return "";
    }

    if (value_->is_string()) {
        const auto result = value_->get<std::string>();
        return String{result.c_str()};
    }

    if (value_->is_boolean()) {
        return value_->get<bool>()
            ? String{"true"}
            : String{"false"};
    }

    if (value_->is_null()) {
        return String{"null"};
    }

    if (value_->is_number()) {
        const auto result = value_->dump();
        return String{result.c_str()};
    }

    return "";
}


auto JSONValue::asInt64() const -> int64_t {

    if (!value_) {
        return 0;
    }

    if (value_->is_number_integer()) {
        return value_->get<int64_t>();
    }

    if (value_->is_boolean()) {
        return value_->get<bool>() ? 1 : 0;
    }

    if (value_->is_number_float()) {
        return static_cast<int64_t>(value_->get<double>());
    }

    return 0;
}


auto JSONValue::asInt32() const -> int32_t {
    return static_cast<int32_t>(asInt64());
}


auto JSONValue::asDouble() const -> double {

    if (!value_) {
        return 0.0;
    }

    if (value_->is_number()) {
        return value_->get<double>();
    }

    if (value_->is_boolean()) {
        return value_->get<bool>() ? 1.0 : 0.0;
    }

    return 0.0;
}


auto JSONValue::asFloat() const -> float {
    return static_cast<float>(asDouble());
}


auto JSONValue::asBoolean() const -> bool {

    if (!value_) {
        return false;
    }

    if (value_->is_boolean()) {
        return value_->get<bool>();
    }

    if (value_->is_number()) {
        return asDouble() != 0.0;
    }

    if (value_->is_string()) {
        return !value_->get_ref<const std::string&>().empty();
    }

    return false;
}


//------------------------------------------------------------------------------
//  Raw access
//------------------------------------------------------------------------------

auto JSONValue::raw() const noexcept -> const json* {
    return value_;
}


}
