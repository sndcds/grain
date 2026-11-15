#include <cstdint>
#include <iostream>

#include "../../include/Grain.hpp"


using namespace Grain;

//------------------------------------------------------------------------------
//  Custom function callbacks
//------------------------------------------------------------------------------

static auto grainLog(lua_State* state) -> int {
    const char* message = luaL_checkstring(state, 1);
    std::printf("[Lua] %s\n", message);
    return 0;
}


static auto grainAdd(lua_State* state) -> int {
    const lua_Integer a = luaL_checkinteger(state, 1);
    const lua_Integer b = luaL_checkinteger(state, 2);
    lua_pushinteger(state, a + b);
    return 1;
}


static auto grainGetVersion(lua_State* state) -> int {
    lua_pushstring(state, "Grain 1.0");
    return 1;
}

auto registerGrainLibrary(Lua& lua) -> LuaError {

    static const luaL_Reg functions[] = {
        {"log",        grainLog},
        {"add",        grainAdd},
        {"getVersion", grainGetVersion},
        {nullptr,      nullptr}
    };

    return lua.registerLibrary("grain", functions);
}


//------------------------------------------------------------------------------
//  Custom Lua library
//------------------------------------------------------------------------------

class DemoLuaLib : public LuaLib {

public:

    LuaError init(Lua* lua) noexcept override {

        if (lua == nullptr || !lua->isInitialized()) {
            return LuaError::NoContext;
        }

        std::cout << "DemoLuaLib initialized.\n";

        return LuaError::None;
    }
};

//------------------------------------------------------------------------------
//  Initialization callback
//------------------------------------------------------------------------------

void initializeLua(Lua* lua) {

    if (lua == nullptr) {
        return;
    }

    std::cout << "Lua initialization callback executed.\n";
}


//------------------------------------------------------------------------------
//  Demo 1: Core and custom library
//------------------------------------------------------------------------------

auto demoCore(Lua& lua, DemoLuaLib& demo_lib) -> bool {

    std::cout << "\n";
    std::cout << "Demo 1: Core and custom library\n";
    std::cout << "--------------------------------\n";


    // Check Lua context.

    if (!lua.isInitialized()) {

        std::cerr << "Lua is not initialized: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Register custom library.

    LuaError error = lua.addLib(&demo_lib);

    if (error != LuaError::None) {

        std::cerr << "Library initialization failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Access Lua state.

    lua_State* state = lua.state();

    if (state == nullptr) {

        std::cerr << "Lua state is unavailable.\n";

        return false;
    }

    std::cout << "Lua state: " << state << '\n';


    // Check last error.

    if (lua.lastError() != LuaError::None) {

        std::cerr << "Lua error: "
                  << lua.errorMessage() << '\n';

        return false;
    }

    std::cout << "Core demo finished.\n";

    return true;
}


//------------------------------------------------------------------------------
//  Demo 2: Global variables
//------------------------------------------------------------------------------

auto demoGlobals(Lua& lua) -> bool {

    std::cout << "\n";
    std::cout << "Demo 2: Global variables\n";
    std::cout << "------------------------\n";


    // Set global variables.

    if (lua.setGlobal("app_name", "Grain Lua Demo") != LuaError::None ||
        lua.setGlobal("app_version", int64_t{1}) != LuaError::None ||
        lua.setGlobal("debug_mode", true) != LuaError::None ||
        lua.setGlobal("window_width", 1200.0) != LuaError::None) {

        std::cerr << "Setting global variables failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Retrieve global variables.

    const char* app_name = nullptr;
    int64_t app_version = 0;
    bool debug_mode = false;
    double window_width = 0.0;


    if (lua.getGlobal("app_name", app_name) != LuaError::None ||
        lua.getGlobal("app_version", app_version) != LuaError::None ||
        lua.getGlobal("debug_mode", debug_mode) != LuaError::None ||
        lua.getGlobal("window_width", window_width) != LuaError::None) {

        std::cerr << "Retrieving global variables failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Display values.

    std::cout << "App name:     " << app_name << '\n';
    std::cout << "App version:  " << app_version << '\n';
    std::cout << "Debug mode:   " << std::boolalpha << debug_mode << '\n';
    std::cout << "Window width: " << window_width << '\n';


    // Remove a global variable.

    if (lua.clearGlobal("debug_mode") != LuaError::None) {

        std::cerr << "Clearing global variable failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Verify removal.

    if (lua.getGlobal("debug_mode", debug_mode) == LuaError::GlobalNotFound) {

        std::cout << "debug_mode was successfully cleared.\n";

    } else {

        std::cerr << "Could not verify removal of debug_mode.\n";

        return false;
    }

    std::cout << "Global variables demo finished.\n";

    return true;
}


//------------------------------------------------------------------------------
//  Demo 3: Tables
//------------------------------------------------------------------------------

auto demoTables(Lua& lua) -> bool {

    std::cout << "\n";
    std::cout << "Demo 3: Tables\n";
    std::cout << "--------------\n";


    // Create a global table.

    if (lua.createTable("app_config") != LuaError::None) {

        std::cerr << "Creating table failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Set table values.

    if (lua.setTableValue("app_config", "width", int64_t{1200}) != LuaError::None ||
        lua.setTableValue("app_config", "height", int64_t{800}) != LuaError::None ||
        lua.setTableValue("app_config", "scale", 1.5) != LuaError::None ||
        lua.setTableValue("app_config", "fullscreen", false) != LuaError::None ||
        lua.setTableValue("app_config", "title", "Grain Application") != LuaError::None) {

        std::cerr << "Setting table values failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Open table.

    if (lua.openTable("app_config") != LuaError::None) {

        std::cerr << "Opening table failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }

    std::cout << "Table successfully opened.\n";


    // Close table.

    if (lua.closeTable() != LuaError::None) {

        std::cerr << "Closing table failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Retrieve table values.

    int64_t width = 0;
    int64_t height = 0;
    double scale = 0.0;
    bool fullscreen = false;

    if (lua.getTableValue("app_config", "width", width) != LuaError::None ||
        lua.getTableValue("app_config", "height", height) != LuaError::None ||
        lua.getTableValue("app_config", "scale", scale) != LuaError::None ||
        lua.getTableValue("app_config", "fullscreen", fullscreen) != LuaError::None) {

        std::cerr << "Retrieving table values failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Display values.

    std::cout << "Width:      " << width << '\n';
    std::cout << "Height:     " << height << '\n';
    std::cout << "Scale:      " << scale << '\n';
    std::cout << "Fullscreen: " << std::boolalpha << fullscreen << '\n';


    // Clear a table value.

    if (lua.clearTableValue("app_config", "fullscreen") != LuaError::None) {

        std::cerr << "Clearing table value failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Verify removal.

    if (lua.getTableValue("app_config", "fullscreen", fullscreen) ==
        LuaError::GlobalNotFound) {

        std::cout << "fullscreen was successfully cleared.\n";

    } else {

        std::cerr << "Could not verify removal of fullscreen.\n";

        return false;
    }

    std::cout << "Tables demo finished.\n";

    return true;
}


//------------------------------------------------------------------------------
//  Demo 4: Execution
//------------------------------------------------------------------------------

auto demoExecution(Lua& lua) -> bool {

    std::cout << "\n";
    std::cout << "Demo 4: Execution\n";
    std::cout << "-----------------\n";


    // Execute Lua source code.

    const char* source = R"(
        app_name = "Grain Lua Demo"

        function grain_demo()
            print("Hello from Lua!")
            print("Application: " .. app_name)
        end

        function grain_add(a, b)
            return a + b
        end
    )";

    if (lua.executeString(source) != LuaError::None) {

        std::cerr << "Executing Lua source failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }

    std::cout << "Lua source successfully executed.\n";


    // Call Lua function.

    if (lua.callFunction("grain_demo") != LuaError::None) {

        std::cerr << "Calling Lua function failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Test runtime error handling.

    if (lua.executeString("error('Intentional Lua error')") !=
        LuaError::RuntimeError) {

        std::cerr << "Expected Lua runtime error was not reported.\n";

        return false;
        }

    std::cout << "Runtime error successfully caught: "
              << lua.errorMessage() << '\n';


    // Verify that execution still works after an error.

    if (lua.executeString("print('Lua execution continues.')") !=
        LuaError::None) {

        std::cerr << "Lua execution failed after runtime error: "
                  << lua.errorMessage() << '\n';

        return false;
        }

    std::cout << "Execution demo finished.\n";

    return true;
}


//------------------------------------------------------------------------------
//  Demo 5: Utilities
//------------------------------------------------------------------------------

auto demoUtilities(Lua& lua) -> bool {

    std::cout << "\n";
    std::cout << "Demo 5: Utilities\n";
    std::cout << "-----------------\n";


    // Register random functions.

    if (lua.registerRandomFunctions() != LuaError::None) {

        std::cerr << "Registering random functions failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Add module path.

    if (lua.addModulePath("./lua") != LuaError::None) {

        std::cerr << "Adding module path failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }


    // Execute Lua test code.

    const char* source = R"(
        print("Random number:", grain.random())

        print("Random integer:", grain.randomInt(1, 100))

        print("Normal distribution:", grain.randomDist(10.0, 2.0))

        local values = { "red", "green", "blue", "yellow" }

        print("Random choice:", grain.randomChoice(values))

        grain.shuffle(values)

        print("Shuffled values:")
        for index, value in ipairs(values) do
            print(index, value)
        end

        print("Chance 50%:", grain.chance(0.5))
        print("Chance 100%:", grain.chance(1.0))
        print("Chance 0%:", grain.chance(0.0))
    )";

    if (lua.executeString(source) != LuaError::None) {

        std::cerr << "Executing utility demo failed: "
                  << lua.errorMessage() << '\n';

        return false;
    }

    std::cout << "Utilities demo finished.\n";

    return true;
}


//------------------------------------------------------------------------------
//  Demo 6: Register some functions
//------------------------------------------------------------------------------

auto demoRegistration(Lua& lua) -> bool {

    std::printf("\n--- Lua C++ Registration ---\n");

    if (registerGrainLibrary(lua) != LuaError::None) {
        std::printf("Failed to register Grain library: %s\n", lua.errorMessage());
        return false;
    }

    const char* script = R"(
        grain.log("Hello from Lua!")

        local result = grain.add(20, 22)

        grain.log("20 + 22 = " .. result)

        grain.log("Version: " .. grain.getVersion())
    )";

    if (lua.executeString(script) != LuaError::None) {
        std::printf("Failed to register Grain library: %s\n", lua.errorMessage());
    }

    return true;
}


//------------------------------------------------------------------------------
//  Main
//------------------------------------------------------------------------------

int main() {

    std::cout << "Grain Lua Demo\n";
    std::cout << "--------------\n";


    // Create one Lua VM and execute initialization callback.

    Lua lua(initializeLua);


    // Check initialization.

    if (!lua.isInitialized()) {

        std::cerr << "Lua initialization failed: "
                  << lua.errorMessage() << '\n';

        return 1;
    }

    std::cout << "Lua VM successfully initialized.\n";


    // Create custom library.

    DemoLuaLib demo_lib;


    // Run both demos using the same Lua context.

    if (!demoCore(lua, demo_lib)) {
        return 1;
    }

    if (!demoGlobals(lua)) {
        return 1;
    }

    if (!demoTables(lua)) {
        return 1;
    }

    if (!demoExecution(lua)) {
        return 1;
    }

    if (!demoUtilities(lua)) {
        return 1;
    }

    if (!demoRegistration(lua)) {
        return 1;
    }


    // Explicit shutdown.

    lua.close();

    std::cout << "\nLua VM closed.\n";


    // Calling close() again is safe.

    lua.close();

    std::cout << "Demo finished.\n";

    return 0;
}