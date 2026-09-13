include(CTest)

add_executable(
        GrainTests
        Core/ObjectBaseTests.cpp
        Containers/ListTests.cpp
        Math/Vec2Tests.cpp
        Math/Vec3Tests.cpp
        Math/Mat3Tests.cpp
        Math/QuadrilateralTests.cpp
        Geometry/LineTests.cpp
        Geometry/RectTests.cpp
        Geometry/RectLayoutTests.cpp
        Geometry/BezierTests.cpp
        Graphics/GraphicPathPointTests.cpp
        Graphics/GraphicPathTests.cpp
        String/StringTests.cpp
        String/StringUtf8Tests.cpp
        Input/InputTests.cpp
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

if(APPLE)
    target_sources(GrainTests PRIVATE Platform/GrainRootViewTests.mm)
endif()

include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

catch_discover_tests(GrainTests)

# Exercise the real high-level App/Window with an in-memory platform backend.
add_executable(
        GrainWindowTests
        Platform/WindowTests.cpp
        ../src/Platform/App.cpp
        ../src/Platform/Window.cpp
        ../src/Input/Input.cpp
)
target_include_directories(GrainWindowTests PRIVATE ../include)
target_link_libraries(GrainWindowTests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(GrainWindowTests)

if(UNIX AND NOT APPLE)
    find_program(XVFB_RUN xvfb-run)
    if(XVFB_RUN)
        add_executable(GrainX11Tests Platform/X11WindowTests.cpp)
        target_link_libraries(GrainX11Tests PRIVATE Grain Catch2::Catch2WithMain)
        add_test(NAME "X11: window event identity"
                COMMAND "${XVFB_RUN}" -a $<TARGET_FILE:GrainX11Tests>)
        set_tests_properties("X11: window event identity" PROPERTIES TIMEOUT 15)
    endif()
endif()
