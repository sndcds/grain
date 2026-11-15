
#include <grain/Script/Lua.hpp>

#include <exception>
#include <cmath>


namespace Grain {


namespace {

//------------------------------------------------------------------------------
//  Push Lua's math.random function onto the stack.
//------------------------------------------------------------------------------

auto pushMathRandom(lua_State* state) -> bool {

    lua_getglobal(state, "math");

    if (!lua_istable(state, -1)) {
        lua_pop(state, 1);
        return false;
    }

    lua_getfield(state, -1, "random");

    if (!lua_isfunction(state, -1)) {
        lua_pop(state, 2);
        return false;
    }

    lua_remove(state, -2);

    return true;
}


//------------------------------------------------------------------------------
//  Push a random integer in the inclusive range [minimum, maximum].
//  Leaves the result on the stack.
//------------------------------------------------------------------------------

auto pushRandomInteger(
    lua_State* state,
    lua_Integer minimum,
    lua_Integer maximum) -> bool {

    if (!pushMathRandom(state)) {
        return false;
    }

    lua_pushinteger(state, minimum);
    lua_pushinteger(state, maximum);

    return lua_pcall(state, 2, 1, 0) == LUA_OK;
}


//------------------------------------------------------------------------------
//  Push a random number in [0, 1).
//  Leaves the result on the stack.
//------------------------------------------------------------------------------

auto pushRandomUnit(lua_State* state) -> bool {

    if (!pushMathRandom(state)) {
        return false;
    }

    return lua_pcall(state, 0, 1, 0) == LUA_OK;
}


} // anonymous namespace


Lua::Lua(
    LuaInitAction init_action
) noexcept {

    initialize(init_action);
}


Lua::~Lua() noexcept {

    close();
}


//------------------------------------------------------------------------------
//  Error handling
//------------------------------------------------------------------------------

LuaError Lua::setError(
    LuaError error,
    const char* message
) noexcept {

    last_error_ = error;
    error_message_ = message != nullptr ? message : "";

    return error;
}


LuaError Lua::lastError() const noexcept {

    return last_error_;
}


void Lua::clearError() noexcept {

    error_message_ = "";
}


//------------------------------------------------------------------------------
//  Initialization
//------------------------------------------------------------------------------

LuaError Lua::initialize(
    LuaInitAction init_action
) noexcept {

    // Already initialized.
    if (state_ != nullptr) {
        clearError();
        return LuaError::None;
    }

    clearError();

    // Create Lua virtual machine.
    state_ = luaL_newstate();

    if (state_ == nullptr) {
        return setError(
            LuaError::InitializationFailed,
            "Unable to create Lua state."
        );
    }

    // Open standard Lua libraries.
    luaL_openlibs(state_);

    // Execute optional initialization callback.
    if (init_action != nullptr) {

        try {
            init_action(this);
        }
        catch (const std::exception& exception) {

            close();

            return setError(
                LuaError::InitializationFailed,
                exception.what()
            );
        }
        catch (...) {

            close();

            return setError(
                LuaError::InitializationFailed,
                "Unknown exception during Lua initialization."
            );
        }
    }

    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Libraries
//------------------------------------------------------------------------------

LuaError Lua::addLib(
    LuaLib* lib
) noexcept {

    if (state_ == nullptr) {
        return setError(
            LuaError::NoContext,
            "Lua is not initialized."
        );
    }

    if (lib == nullptr) {
        return setError(
            LuaError::InvalidArgument,
            "Lua library pointer is null."
        );
    }

    const LuaError error = lib->init(this);

    if (error != LuaError::None) {
        return setError(
            error,
            "Lua library initialization failed."
        );
    }

    clearError();

    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------

auto Lua::setGlobal(const char* name, bool value) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid global name.");
    }

    lua_pushboolean(state_, value);
    lua_setglobal(state_, name);

    clearError();
    return LuaError::None;
}


auto Lua::setGlobal(const char* name, int64_t value) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid global name.");
    }

    lua_pushinteger(state_, static_cast<lua_Integer>(value));
    lua_setglobal(state_, name);

    clearError();
    return LuaError::None;
}


auto Lua::setGlobal(const char* name, double value) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid global name.");
    }

    lua_pushnumber(state_, static_cast<lua_Number>(value));
    lua_setglobal(state_, name);

    clearError();
    return LuaError::None;
}


auto Lua::setGlobal(const char* name, const char* value) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0' || value == nullptr) {
        return setError(LuaError::InvalidArgument, "Invalid global name or value.");
    }

    lua_pushstring(state_, value);
    lua_setglobal(state_, name);

    clearError();
    return LuaError::None;
}


// MARK: - Get global variables

auto Lua::getGlobal(const char* name, bool& value) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid global name.");
    }

    const int type = lua_getglobal(state_, name);

    if (type == LUA_TNIL) {
        lua_pop(state_, 1);
        return setError(LuaError::GlobalNotFound, "Lua global does not exist.");
    }

    if (type != LUA_TBOOLEAN) {
        lua_pop(state_, 1);
        return setError(LuaError::TypeMismatch, "Lua global is not a boolean.");
    }

    value = lua_toboolean(state_, -1) != 0;

    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


auto Lua::getGlobal(const char* name, int64_t& value) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid global name.");
    }

    const int type = lua_getglobal(state_, name);

    if (type == LUA_TNIL) {
        lua_pop(state_, 1);
        return setError(LuaError::GlobalNotFound, "Lua global does not exist.");
    }

    if (type != LUA_TNUMBER || !lua_isinteger(state_, -1)) {
        lua_pop(state_, 1);
        return setError(LuaError::TypeMismatch, "Lua global is not an integer.");
    }

    value = static_cast<int64_t>(lua_tointeger(state_, -1));

    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


auto Lua::getGlobal(const char* name, double& value) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid global name.");
    }

    const int type = lua_getglobal(state_, name);

    if (type == LUA_TNIL) {
        lua_pop(state_, 1);
        return setError(LuaError::GlobalNotFound, "Lua global does not exist.");
    }

    if (type != LUA_TNUMBER) {
        lua_pop(state_, 1);
        return setError(LuaError::TypeMismatch, "Lua global is not a number.");
    }

    value = static_cast<double>(lua_tonumber(state_, -1));

    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


auto Lua::getGlobal(const char* name, const char*& value) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid global name.");
    }

    const int type = lua_getglobal(state_, name);

    if (type == LUA_TNIL) {
        lua_pop(state_, 1);
        return setError(LuaError::GlobalNotFound, "Lua global does not exist.");
    }

    if (type != LUA_TSTRING) {
        lua_pop(state_, 1);
        return setError(LuaError::TypeMismatch, "Lua global is not a string.");
    }

    value = lua_tostring(state_, -1);

    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


// MARK: - Clear global variable

auto Lua::clearGlobal(const char* name) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid global name.");
    }

    lua_pushnil(state_);
    lua_setglobal(state_, name);

    clearError();
    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Tables
//------------------------------------------------------------------------------

auto Lua::createTable(const char* name) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table name.");
    }

    lua_newtable(state_);
    lua_setglobal(state_, name);

    clearError();
    return LuaError::None;
}


auto Lua::pushTable(const char* name) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table name.");
    }

    const int type = lua_getglobal(state_, name);

    if (type == LUA_TNIL) {
        lua_pop(state_, 1);
        return setError(LuaError::GlobalNotFound, "Lua table does not exist.");
    }

    if (type != LUA_TTABLE) {
        lua_pop(state_, 1);
        return setError(LuaError::InvalidTable, "Lua global is not a table.");
    }

    return LuaError::None;
}


auto Lua::openTable(const char* name) noexcept -> LuaError {

    const LuaError error = pushTable(name);

    if (error != LuaError::None) {
        return error;
    }

    clearError();
    return LuaError::None;
}


auto Lua::closeTable() noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (lua_gettop(state_) == 0) {
        return setError(LuaError::InvalidTable, "Lua stack is empty.");
    }

    if (lua_type(state_, -1) != LUA_TTABLE) {
        return setError(LuaError::InvalidTable, "Top of Lua stack is not a table.");
    }

    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


//--------------------------------------------------------------------------
//  Set table values
//--------------------------------------------------------------------------

auto Lua::setTableValue(
    const char* table,
    const char* key,
    bool value) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table key.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    lua_pushboolean(state_, value);
    lua_setfield(state_, -2, key);
    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


auto Lua::setTableValue(
    const char* table,
    const char* key,
    int64_t value) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table key.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    lua_pushinteger(state_, static_cast<lua_Integer>(value));
    lua_setfield(state_, -2, key);
    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


auto Lua::setTableValue(
    const char* table,
    const char* key,
    double value) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table key.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    lua_pushnumber(state_, static_cast<lua_Number>(value));
    lua_setfield(state_, -2, key);
    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


auto Lua::setTableValue(
    const char* table,
    const char* key,
    const char* value) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0' || value == nullptr) {
        return setError(LuaError::InvalidArgument, "Invalid table key or value.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    lua_pushstring(state_, value);
    lua_setfield(state_, -2, key);
    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Get table values
//------------------------------------------------------------------------------

auto Lua::getTableValue(
    const char* table,
    const char* key,
    bool& value) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table key.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    const int type = lua_getfield(state_, -1, key);

    if (type == LUA_TNIL) {
        lua_pop(state_, 2);
        return setError(LuaError::GlobalNotFound, "Table field does not exist.");
    }

    if (type != LUA_TBOOLEAN) {
        lua_pop(state_, 2);
        return setError(LuaError::TypeMismatch, "Table field is not a boolean.");
    }

    value = lua_toboolean(state_, -1) != 0;

    lua_pop(state_, 2);

    clearError();
    return LuaError::None;
}


auto Lua::getTableValue(
    const char* table,
    const char* key,
    int64_t& value) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table key.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    const int type = lua_getfield(state_, -1, key);

    if (type == LUA_TNIL) {
        lua_pop(state_, 2);
        return setError(LuaError::GlobalNotFound, "Table field does not exist.");
    }

    if (type != LUA_TNUMBER || !lua_isinteger(state_, -1)) {
        lua_pop(state_, 2);
        return setError(LuaError::TypeMismatch, "Table field is not an integer.");
    }

    value = static_cast<int64_t>(lua_tointeger(state_, -1));

    lua_pop(state_, 2);

    clearError();
    return LuaError::None;
}


auto Lua::getTableValue(
    const char* table,
    const char* key,
    double& value) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table key.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    const int type = lua_getfield(state_, -1, key);

    if (type == LUA_TNIL) {
        lua_pop(state_, 2);
        return setError(LuaError::GlobalNotFound, "Table field does not exist.");
    }

    if (type != LUA_TNUMBER) {
        lua_pop(state_, 2);
        return setError(LuaError::TypeMismatch, "Table field is not a number.");
    }

    value = static_cast<double>(lua_tonumber(state_, -1));

    lua_pop(state_, 2);

    clearError();
    return LuaError::None;
}


auto Lua::getTableValue(
    const char* table,
    const char* key,
    const char*& value) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table key.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    const int type = lua_getfield(state_, -1, key);

    if (type == LUA_TNIL) {
        lua_pop(state_, 2);
        return setError(LuaError::GlobalNotFound, "Table field does not exist.");
    }

    if (type != LUA_TSTRING) {
        lua_pop(state_, 2);
        return setError(LuaError::TypeMismatch, "Table field is not a string.");
    }

    value = lua_tostring(state_, -1);

    lua_pop(state_, 2);

    clearError();
    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Clear table value
//------------------------------------------------------------------------------

auto Lua::clearTableValue(
    const char* table,
    const char* key) noexcept -> LuaError {

    if (key == nullptr || key[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid table key.");
    }

    const LuaError error = pushTable(table);

    if (error != LuaError::None) {
        return error;
    }

    lua_pushnil(state_);
    lua_setfield(state_, -2, key);
    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Execution: Lua file
//------------------------------------------------------------------------------

auto Lua::executeFile(const char* file_path) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (file_path == nullptr || file_path[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid Lua file path.");
    }


    // Load Lua file.

    int status = luaL_loadfile(state_, file_path);

    if (status != LUA_OK) {

        const char* message = lua_tostring(state_, -1);

        const LuaError error = (status == LUA_ERRFILE)
            ? LuaError::FileNotFound
            : LuaError::RuntimeError;

        setError(
            error,
            message != nullptr ? message : "Could not load Lua file."
        );

        lua_pop(state_, 1);

        return error;
    }


    // Execute loaded Lua chunk.

    status = lua_pcall(state_, 0, 0, 0);

    if (status != LUA_OK) {

        const char* message = lua_tostring(state_, -1);

        setError(
            LuaError::RuntimeError,
            message != nullptr ? message : "Lua file execution failed."
        );

        lua_pop(state_, 1);

        return LuaError::RuntimeError;
    }

    clearError();

    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Execution: Lua source string
//------------------------------------------------------------------------------

auto Lua::executeString(const char* source) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (source == nullptr) {
        return setError(LuaError::InvalidArgument, "Lua source is null.");
    }


    // Load Lua source.

    int status = luaL_loadstring(state_, source);

    if (status != LUA_OK) {

        const char* message = lua_tostring(state_, -1);

        setError(
            LuaError::RuntimeError,
            message != nullptr ? message : "Could not load Lua source."
        );

        lua_pop(state_, 1);

        return LuaError::RuntimeError;
    }


    // Execute loaded Lua chunk.

    status = lua_pcall(state_, 0, 0, 0);

    if (status != LUA_OK) {

        const char* message = lua_tostring(state_, -1);

        setError(
            LuaError::RuntimeError,
            message != nullptr ? message : "Lua source execution failed."
        );

        lua_pop(state_, 1);

        return LuaError::RuntimeError;
    }

    clearError();

    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Execution: Call global Lua function
//------------------------------------------------------------------------------

auto Lua::callFunction(const char* name) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (name == nullptr || name[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid Lua function name.");
    }


    // Retrieve global function.

    const int type = lua_getglobal(state_, name);

    if (type != LUA_TFUNCTION) {

        lua_pop(state_, 1);

        return setError(
            LuaError::InvalidFunction,
            "Lua global is not a function."
        );
    }


    // Call function without arguments or return values.

    const int status = lua_pcall(state_, 0, 0, 0);

    if (status != LUA_OK) {

        const char* message = lua_tostring(state_, -1);

        setError(
            LuaError::RuntimeError,
            message != nullptr ? message : "Lua function execution failed."
        );

        lua_pop(state_, 1);

        return LuaError::RuntimeError;
    }

    clearError();

    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Shutdown
//------------------------------------------------------------------------------

void Lua::close() noexcept {

    if (state_ != nullptr) {

        lua_close(state_);
        state_ = nullptr;
    }
}

//------------------------------------------------------------------------------
//  Stack conversion
//------------------------------------------------------------------------------

auto Lua::toBool(lua_State* state, int index) noexcept -> bool {
    return state != nullptr && lua_toboolean(state, index) != 0;
}


auto Lua::toInt32(lua_State* state, int index) noexcept -> int32_t {
    return state != nullptr
        ? static_cast<int32_t>(lua_tointeger(state, index))
        : 0;
}


auto Lua::toInt64(lua_State* state, int index) noexcept -> int64_t {
    return state != nullptr
        ? static_cast<int64_t>(lua_tointeger(state, index))
        : 0;
}


auto Lua::toFloat(lua_State* state, int index) noexcept -> float {
    return state != nullptr
        ? static_cast<float>(lua_tonumber(state, index))
        : 0.0f;
}


auto Lua::toDouble(lua_State* state, int index) noexcept -> double {
    return state != nullptr
        ? static_cast<double>(lua_tonumber(state, index))
        : 0.0;
}


//------------------------------------------------------------------------------
//  Strict stack conversion
//------------------------------------------------------------------------------

auto Lua::getInteger(
    lua_State* state,
    int index,
    int64_t& value) noexcept -> bool {

    if (state == nullptr || !lua_isinteger(state, index)) {
        return false;
    }

    value = static_cast<int64_t>(lua_tointeger(state, index));
    return true;
}


auto Lua::getNumber(
    lua_State* state,
    int index,
    double& value) noexcept -> bool {

    if (state == nullptr || !lua_isnumber(state, index)) {
        return false;
    }

    value = static_cast<double>(lua_tonumber(state, index));
    return true;
}


//------------------------------------------------------------------------------
//  Stack push
//------------------------------------------------------------------------------

auto Lua::pushBool(lua_State* state, bool value) noexcept -> void {
    if (state != nullptr) {
        lua_pushboolean(state, value);
    }
}


auto Lua::pushInt32(lua_State* state, int32_t value) noexcept -> void {
    if (state != nullptr) {
        lua_pushinteger(state, static_cast<lua_Integer>(value));
    }
}


auto Lua::pushInt64(lua_State* state, int64_t value) noexcept -> void {
    if (state != nullptr) {
        lua_pushinteger(state, static_cast<lua_Integer>(value));
    }
}


auto Lua::pushFloat(lua_State* state, float value) noexcept -> void {
    if (state != nullptr) {
        lua_pushnumber(state, static_cast<lua_Number>(value));
    }
}


auto Lua::pushDouble(lua_State* state, double value) noexcept -> void {
    if (state != nullptr) {
        lua_pushnumber(state, static_cast<lua_Number>(value));
    }
}


//------------------------------------------------------------------------------
//  grain.random()
//------------------------------------------------------------------------------

auto Lua::luaRandom(lua_State* state) -> int {

    if (!pushRandomUnit(state)) {
        return luaL_error(state, "grain.random(): math.random failed");
    }

    return 1;
}


//------------------------------------------------------------------------------
//  grain.randomInt(minimum, maximum)
//------------------------------------------------------------------------------

auto Lua::luaRandomInt(lua_State* state) -> int {

    const lua_Integer minimum = luaL_checkinteger(state, 1);
    const lua_Integer maximum = luaL_checkinteger(state, 2);

    if (minimum > maximum) {
        return luaL_error(state, "grain.randomInt(): invalid range");
    }

    if (!pushRandomInteger(state, minimum, maximum)) {
        return luaL_error(state, "grain.randomInt(): math.random failed");
    }

    return 1;
}


//------------------------------------------------------------------------------
//  grain.randomDist(mean, standardDeviation)
//------------------------------------------------------------------------------

auto Lua::luaRandomDist(lua_State* state) -> int {

    const lua_Number mean = luaL_checknumber(state, 1);
    const lua_Number deviation = luaL_checknumber(state, 2);

    if (deviation < 0.0) {
        return luaL_error(state, "grain.randomDist(): negative deviation");
    }

    if (deviation == 0.0) {
        lua_pushnumber(state, mean);
        return 1;
    }

    lua_Number u1 = 0.0;
    lua_Number u2 = 0.0;

    // Box-Muller transform.

    do {
        if (!pushRandomUnit(state)) {
            return luaL_error(state, "grain.randomDist(): random generation failed");
        }

        u1 = lua_tonumber(state, -1);
        lua_pop(state, 1);

    } while (u1 <= 0.0);

    if (!pushRandomUnit(state)) {
        return luaL_error(state, "grain.randomDist(): random generation failed");
    }

    u2 = lua_tonumber(state, -1);
    lua_pop(state, 1);

    const lua_Number z =
        std::sqrt(-2.0 * std::log(u1)) *
        std::cos(2.0 * std::acos(-1.0) * u2);

    lua_pushnumber(state, mean + deviation * z);

    return 1;
}


//------------------------------------------------------------------------------
//  grain.randomChoice(table)
//------------------------------------------------------------------------------

auto Lua::luaRandomChoice(lua_State* state) -> int {

    luaL_checktype(state, 1, LUA_TTABLE);

    const lua_Integer count =
        static_cast<lua_Integer>(lua_rawlen(state, 1));

    if (count == 0) {
        lua_pushnil(state);
        return 1;
    }

    if (!pushRandomInteger(state, 1, count)) {
        return luaL_error(state, "grain.randomChoice(): random generation failed");
    }

    const lua_Integer index = lua_tointeger(state, -1);

    lua_pop(state, 1);

    lua_rawgeti(state, 1, index);

    return 1;
}


//------------------------------------------------------------------------------
//  grain.shuffle(table)
//------------------------------------------------------------------------------

auto Lua::luaShuffle(lua_State* state) -> int {

    luaL_checktype(state, 1, LUA_TTABLE);

    const lua_Integer count =
        static_cast<lua_Integer>(lua_rawlen(state, 1));

    for (lua_Integer i = count; i > 1; --i) {

        if (!pushRandomInteger(state, 1, i)) {
            return luaL_error(state, "grain.shuffle(): random generation failed");
        }

        const lua_Integer j = lua_tointeger(state, -1);

        lua_pop(state, 1);

        if (i == j) {
            continue;
        }

        lua_rawgeti(state, 1, i);
        lua_rawgeti(state, 1, j);

        lua_pushvalue(state, -1);
        lua_rawseti(state, 1, i);

        lua_pushvalue(state, -2);
        lua_rawseti(state, 1, j);

        lua_pop(state, 2);
    }

    lua_pushvalue(state, 1);

    return 1;
}


//------------------------------------------------------------------------------
//  grain.chance(probability)
//------------------------------------------------------------------------------

auto Lua::luaChance(lua_State* state) -> int {

    const lua_Number probability = luaL_checknumber(state, 1);

    if (probability < 0.0 || probability > 1.0) {
        return luaL_error(state, "grain.chance(): probability must be between 0 and 1");
    }

    if (probability <= 0.0) {
        lua_pushboolean(state, false);
        return 1;
    }

    if (probability >= 1.0) {
        lua_pushboolean(state, true);
        return 1;
    }

    if (!pushRandomUnit(state)) {
        return luaL_error(state, "grain.chance(): random generation failed");
    }

    const bool result = lua_tonumber(state, -1) < probability;

    lua_pop(state, 1);

    lua_pushboolean(state, result);

    return 1;
}

//------------------------------------------------------------------------------
//  Register random functions
//------------------------------------------------------------------------------

auto Lua::registerRandomFunctions() noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    static const luaL_Reg functions[] = {
        { "random",       luaRandom       },
        { "randomInt",    luaRandomInt    },
        { "randomDist",   luaRandomDist   },
        { "randomChoice", luaRandomChoice },
        { "shuffle",      luaShuffle      },
        { "chance",       luaChance       },
        { nullptr, nullptr }
    };

    lua_newtable(state_);
    luaL_setfuncs(state_, functions, 0);
    lua_setglobal(state_, "grain");

    clearError();

    return LuaError::None;
}


//------------------------------------------------------------------------------
//  Module paths
//------------------------------------------------------------------------------

auto Lua::addModulePath(const char* path) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (path == nullptr || path[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid module path.");
    }

    // Get package.path.
    lua_getglobal(state_, "package");

    if (!lua_istable(state_, -1)) {
        lua_pop(state_, 1);
        return setError(LuaError::InvalidTable, "Lua package table is unavailable.");
    }

    lua_getfield(state_, -1, "path");

    if (!lua_isstring(state_, -1)) {
        lua_pop(state_, 2);
        return setError(LuaError::TypeMismatch, "Lua package.path is not a string.");
    }

    // Append the new module path.
    const char* current_path = lua_tostring(state_, -1);

    lua_pushfstring(state_, "%s;%s", current_path, path);
    lua_setfield(state_, -3, "path");

    // Remove package.path and package from the stack.
    lua_pop(state_, 2);

    clearError();
    return LuaError::None;
}


auto Lua::setModulePath(const char* path) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(LuaError::NoContext, "Lua is not initialized.");
    }

    if (path == nullptr || path[0] == '\0') {
        return setError(LuaError::InvalidArgument, "Invalid module path.");
    }

    lua_getglobal(state_, "package");

    if (!lua_istable(state_, -1)) {
        lua_pop(state_, 1);
        return setError(LuaError::InvalidTable, "Lua package table is unavailable.");
    }

    lua_pushstring(state_, path);
    lua_setfield(state_, -2, "path");

    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}


//------------------------------------------------------------------------------
//  C++ function registration
//------------------------------------------------------------------------------

auto Lua::registerFunction(
    const char* name,
    lua_CFunction function
) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(
            LuaError::NoContext,
            "Lua is not initialized."
        );
    }

    if (name == nullptr || name[0] == '\0' || function == nullptr) {
        return setError(
            LuaError::InvalidArgument,
            "Invalid Lua function registration."
        );
    }

    lua_pushcfunction(state_, function);
    lua_setglobal(state_, name);

    clearError();
    return LuaError::None;
}


auto Lua::registerLibrary(
    const char* name,
    const luaL_Reg* functions
) noexcept -> LuaError {

    if (state_ == nullptr) {
        return setError(
            LuaError::NoContext,
            "Lua is not initialized."
        );
    }

    if (name == nullptr || name[0] == '\0' || functions == nullptr) {
        return setError(
            LuaError::InvalidArgument,
            "Invalid Lua library registration."
        );
    }

    // Get an existing global table, or create a new one.

    lua_getglobal(state_, name);

    if (lua_isnil(state_, -1)) {

        lua_pop(state_, 1);
        lua_newtable(state_);

        lua_pushvalue(state_, -1);
        lua_setglobal(state_, name);

    } else if (!lua_istable(state_, -1)) {

        lua_pop(state_, 1);

        return setError(
            LuaError::InvalidTable,
            "The Lua library name already refers to a non-table value."
        );
    }

    // Register the functions in the table.

    luaL_setfuncs(state_, functions, 0);

    lua_pop(state_, 1);

    clearError();
    return LuaError::None;
}

} // namespace Grain
