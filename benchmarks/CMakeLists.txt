
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
