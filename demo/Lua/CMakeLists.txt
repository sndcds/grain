add_executable(
        lua_demo
        LuaDemo.cpp
)

target_link_libraries(
        lua_demo
        PRIVATE
        Grain
)
