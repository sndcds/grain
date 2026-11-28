add_executable(
        audio_demo
        AudioDemo.cpp
)

target_link_libraries(
        audio_demo
        PRIVATE
        Grain
)


add_executable(
        convolution_demo
        ConvolutionDemo.cpp
)

target_link_libraries(
        convolution_demo
        PRIVATE
        Grain
)