# window_demo

# add_executable(
#         window_demo
#         WindowDemo.cpp
# )

# target_link_libraries(
#         window_demo
#         PRIVATE
#         Grain
# )


# view_layout_demo

add_executable(
        view_layout_demo
        ViewLayoutDemo.cpp
)

target_link_libraries(
        view_layout_demo
        PRIVATE
        Grain
)


# layout_demo

add_executable(
        layout_demo
        LayoutDemo.cpp
)

target_link_libraries(
        layout_demo
        PRIVATE
        Grain
)