include(CTest)

add_executable(
        GrainTests
        Type/TypeTests.cpp
        Containers/ListTests.cpp
        Math/MathTests.cpp
        Math/InterpolationTests.cpp
        Math/Vec2Tests.cpp
        Math/Vec3Tests.cpp
        Math/Mat3Tests.cpp
        Math/QuadrilateralTests.cpp
        Math/EasingTests.cpp
        Math/FunctionsTests.cpp
        Math/GeoTests.cpp
        Math/OpticsTests.cpp
        Math/QuantizationTests.cpp
        Math/SphereTests.cpp
        Math/PolynomialTests.cpp
        Geometry/SizeTests.cpp
        Geometry/LineTests.cpp
        Geometry/RectTests.cpp
        Geometry/RectLayoutTests.cpp
        Geometry/RemapRectTests.cpp
        Geometry/BezierTests.cpp
        Graphics/GraphicPathPointTests.cpp
        Graphics/GraphicPathTests.cpp
        Graphics/FontTests.cpp
        String/StringTests.cpp
        String/StringUtf8Tests.cpp
        Input/InputTests.cpp
        DSP/FFTTests.cpp
        DSP/PartialsTests.cpp
        DSP/ConvolutionTests.cpp
        DSP/ConvolutionStreamTests.cpp
)

target_link_libraries(
        GrainTests
        PRIVATE
        Grain
        Catch2::Catch2WithMain
)

target_compile_features(
        GrainTests
        PRIVATE
        cxx_std_23
)

add_executable(
        GrainConvolutionStreamAllocationTests
        DSP/ConvolutionStreamAllocationTests.cpp
)

target_link_libraries(
        GrainConvolutionStreamAllocationTests
        PRIVATE
        Grain
)

target_compile_features(
        GrainConvolutionStreamAllocationTests
        PRIVATE
        cxx_std_23
)

add_test(
        NAME GrainConvolutionStreamAllocationTests
        COMMAND GrainConvolutionStreamAllocationTests
)

if(APPLE)
    # target_sources(GrainTests PRIVATE Platform/GrainRootViewTests.mm)
elseif(UNIX AND GRAIN_LINUX_BACKEND STREQUAL "X11")
    # target_sources(GrainTests PRIVATE Platform/CairoGraphicContextTests.cpp)
endif()

include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

catch_discover_tests(GrainTests)