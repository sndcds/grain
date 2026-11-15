add_executable(
        http_demo
        HTTPDemo.cpp
)

target_link_libraries(
        http_demo
        PRIVATE
        Grain
)
