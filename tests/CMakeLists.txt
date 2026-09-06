include(CTest)

add_executable(
        GrainTests
        Core/ObjectBaseTests.cpp
        Containers/ListTests.cpp
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

include("${Catch2_SOURCE_DIR}/extras/Catch.cmake")

catch_discover_tests(GrainTests)