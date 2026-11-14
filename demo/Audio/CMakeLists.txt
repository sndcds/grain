add_executable(
        audio_demo
        AudioDemo.cpp
)

target_link_libraries(
        audio_demo
        PRIVATE
        Grain
)
