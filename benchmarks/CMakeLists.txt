
add_executable(
        GrainFFTBenchmarks
        FFTBenchmark.cpp
)

target_link_libraries(
        GrainFFTBenchmarks
        PRIVATE
        Grain
)

target_compile_features(
        GrainFFTBenchmarks
        PRIVATE
        cxx_std_23
)


add_executable(
        GrainConvolutionStreamBenchmarks
        ConvolutionStreamBenchmark.cpp
)

target_link_libraries(
        GrainConvolutionStreamBenchmarks
        PRIVATE
        Grain
)

target_compile_features(
        GrainConvolutionStreamBenchmarks
        PRIVATE
        cxx_std_23
)