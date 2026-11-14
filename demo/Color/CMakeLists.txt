add_executable(
        color_for_ui_demo
        ColorsForUI.cpp
)

target_link_libraries(
        color_for_ui_demo
        PRIVATE
        Grain
)
