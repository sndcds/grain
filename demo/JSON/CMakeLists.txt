add_executable(
        json_demo
        JSONDemo.cpp
)

target_link_libraries(
        json_demo
        PRIVATE
        Grain
)
