
#pragma once

#include "Grain.hpp"
#include <grain/String/String.hpp>

#include <cstdint>

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}


namespace Grain {


class Lua;


enum class LuaError : uint8_t {
    None = 0,
    NoContext,
    InitializationFailed,
    RuntimeError,
    MissingArguments,
    UnknownCommand,
    UidOutOfRange,
    ResourceNotFound,
    InvalidArgument,
    InvalidFunction,
    InvalidTable,
    FileNotFound,
    GlobalNotFound,
    TypeMismatch,
};


using LuaInitAction = void (*)(Lua* lua);


class LuaLib {

public:
    LuaLib() noexcept = default;
    ~LuaLib() noexcept = default;

    virtual LuaError init(Lua* lua) noexcept {
        return LuaError::None;
    }
};


class Lua {

protected:
    lua_State* state_ = nullptr;
    String error_message_;
    LuaError last_error_ = LuaError::None;

    LuaError setError(
        LuaError error,
        const char* message
    ) noexcept;

public:

    //--------------------------------------------------------------------------
    //  Construction / destruction
    //--------------------------------------------------------------------------

    explicit Lua(
        LuaInitAction init_action = nullptr
    ) noexcept;

    ~Lua() noexcept;

    Lua(const Lua&) = delete;
    auto operator = (const Lua&) -> Lua& = delete;

    Lua(Lua&&) = delete;
    auto operator = (Lua&&) -> Lua& = delete;


    //--------------------------------------------------------------------------
    //  State
    //--------------------------------------------------------------------------

    bool isInitialized() const noexcept {
        return state_ != nullptr;
    }

    lua_State* state() noexcept {
        return state_;
    }

    const lua_State* state() const noexcept {
        return state_;
    }


    //--------------------------------------------------------------------------
    //  Error handling
    //--------------------------------------------------------------------------

    LuaError lastError() const noexcept;

    const char* errorMessage() const noexcept {
        return error_message_.utf8();
    }

    void clearError() noexcept;


    //--------------------------------------------------------------------------
    //  Initialization
    //--------------------------------------------------------------------------

    LuaError initialize(
        LuaInitAction init_action = nullptr
    ) noexcept;

    LuaError addLib(
        LuaLib* lib
    ) noexcept;


    //--------------------------------------------------------------------------
    //  Global variables
    //--------------------------------------------------------------------------

    auto setGlobal(const char* name, bool value) noexcept -> LuaError;
    auto setGlobal(const char* name, int64_t value) noexcept -> LuaError;
    auto setGlobal(const char* name, double value) noexcept -> LuaError;
    auto setGlobal(const char* name, const char* value) noexcept -> LuaError;

    auto getGlobal(const char* name, bool& value) noexcept -> LuaError;
    auto getGlobal(const char* name, int64_t& value) noexcept -> LuaError;
    auto getGlobal(const char* name, double& value) noexcept -> LuaError;
    auto getGlobal(const char* name, const char*& value) noexcept -> LuaError;

    auto clearGlobal(const char* name) noexcept -> LuaError;


    //--------------------------------------------------------------------------
    //  Tables
    //--------------------------------------------------------------------------

    auto createTable(const char* name) noexcept -> LuaError;
    auto openTable(const char* name) noexcept -> LuaError;
    auto closeTable() noexcept -> LuaError;
    auto pushTable(const char* name) noexcept -> LuaError;

    auto setTableValue(const char* table, const char* key, bool value) noexcept -> LuaError;
    auto setTableValue(const char* table, const char* key, int64_t value) noexcept -> LuaError;
    auto setTableValue(const char* table, const char* key, double value) noexcept -> LuaError;
    auto setTableValue(const char* table, const char* key, const char* value) noexcept -> LuaError;

    auto getTableValue(const char* table, const char* key, bool& value) noexcept -> LuaError;
    auto getTableValue(const char* table, const char* key, int64_t& value) noexcept -> LuaError;
    auto getTableValue(const char* table, const char* key, double& value) noexcept -> LuaError;
    auto getTableValue(const char* table, const char* key, const char*& value) noexcept -> LuaError;

    auto clearTableValue(const char* table, const char* key) noexcept -> LuaError;


    //--------------------------------------------------------------------------
    //  Execution
    //--------------------------------------------------------------------------

    auto executeFile(const char* file_path) noexcept -> LuaError;
    auto executeString(const char* source) noexcept -> LuaError;
    auto callFunction(const char* name) noexcept -> LuaError;


    //--------------------------------------------------------------------------
    //  Shutdown
    //--------------------------------------------------------------------------

    void close() noexcept;


    //--------------------------------------------------------------------------
    //  Stack conversion
    //--------------------------------------------------------------------------

    static auto toBool(lua_State* state, int index) noexcept -> bool;
    static auto toInt32(lua_State* state, int index) noexcept -> int32_t;
    static auto toInt64(lua_State* state, int index) noexcept -> int64_t;
    static auto toFloat(lua_State* state, int index) noexcept -> float;
    static auto toDouble(lua_State* state, int index) noexcept -> double;

    static auto getInteger(
        lua_State* state,
        int index,
        int64_t& value) noexcept -> bool;

    static auto getNumber(
        lua_State* state,
        int index,
        double& value) noexcept -> bool;


    //--------------------------------------------------------------------------
    //  Stack push
    //--------------------------------------------------------------------------

    static auto pushBool(lua_State* state, bool value) noexcept -> void;
    static auto pushInt32(lua_State* state, int32_t value) noexcept -> void;
    static auto pushInt64(lua_State* state, int64_t value) noexcept -> void;
    static auto pushFloat(lua_State* state, float value) noexcept -> void;
    static auto pushDouble(lua_State* state, double value) noexcept -> void;


    //--------------------------------------------------------------------------
    //  Random functions
    //--------------------------------------------------------------------------

    auto registerRandomFunctions() noexcept -> LuaError;


    //--------------------------------------------------------------------------
    //  Module paths
    //--------------------------------------------------------------------------

    auto addModulePath(const char* path) noexcept -> LuaError;
    auto setModulePath(const char* path) noexcept -> LuaError;


    //--------------------------------------------------------------------------
    //  C++ function registration
    //--------------------------------------------------------------------------

    auto registerFunction(
        const char* name,
        lua_CFunction function
    ) noexcept -> LuaError;

    auto registerLibrary(
        const char* name,
        const luaL_Reg* functions
    ) noexcept -> LuaError;


private:

    //--------------------------------------------------------------------------
    //  Lua random callbacks
    //--------------------------------------------------------------------------

    static auto luaRandom(lua_State* state) -> int;
    static auto luaRandomInt(lua_State* state) -> int;
    static auto luaRandomDist(lua_State* state) -> int;
    static auto luaRandomChoice(lua_State* state) -> int;
    static auto luaShuffle(lua_State* state) -> int;
    static auto luaChance(lua_State* state) -> int;
};


} // namespace Grain
