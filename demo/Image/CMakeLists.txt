add_executable(
        image_demo
        ImageDemo.cpp
)

target_link_libraries(
        image_demo
        PRIVATE
        Grain
)
