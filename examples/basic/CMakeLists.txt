add_executable(
        grain_basic
        main.cpp
)

target_link_libraries(
        grain_basic
        PRIVATE
        Grain
)