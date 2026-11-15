# Image demo
add_executable(
        image_demo
        ImageDemo.cpp
)

target_link_libraries(
        image_demo
        PRIVATE
        Grain
)


# Image draw demo
add_executable(
        image_draw_demo
        ImageDrawDemo.cpp
)

target_link_libraries(
        image_draw_demo
        PRIVATE
        Grain
)