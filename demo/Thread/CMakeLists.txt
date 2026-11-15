add_executable(
        thread_demo
        ThreadDemo.cpp
)

target_link_libraries(
        thread_demo
        PRIVATE
        Grain
)
