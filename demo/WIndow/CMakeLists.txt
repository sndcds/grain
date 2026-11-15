add_executable(
        window_demo
        WindowDemo.cpp
)

target_link_libraries(
        window_demo
        PRIVATE
        Grain
)